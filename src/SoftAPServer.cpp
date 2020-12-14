/*
   https://github.com/esp8266/Arduino/tree/master/libraries/DNSServer/examples/CaptivePortalAdvanced
 */
//
// Created by thomas on 13-12-20.
//

#include "SoftAPServer.h"

IPAddress local_IP(192, 168, 0, 2);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

ESP8266WebServer server(80);
DNSServer dnsServer;

static const byte DNS_PORT = 53;
static const String SSID = "ESPx";
static bool needsProcessing = false;


void SoftAPServer::startSoftAp(){
    Serial.print("Configuring SoftAP Configuration...");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Setting SoftAP...");
    String macAddr = WiFi.macAddress();
    macAddr.replace(":", "");
    Serial.println(WiFi.softAP(SSID + macAddr) ? "Ready" : "Failed!");

    Serial.println();
    Serial.print("connect to the IP Address given below...");
    Serial.println(WiFi.softAPIP());

    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    dnsServer.start(DNS_PORT, "*", local_IP);

    server.on("/", HTTP_GET, handleRoot);
    server.on("/connect", HTTP_POST, handleLogin);
    server.onNotFound(handleNotFound);

    server.begin();

    needsProcessing = true;
}

void SoftAPServer::processRequest() {
        dnsServer.processNextRequest();
        server.handleClient();
}

void SoftAPServer::handleRoot() {
    Serial.println("handleAPRoot");
    server.send(200, "text/html",
                R"( <form action="/connect" method="POST">
                            <input type="text" name="SSID" placeholder="Ssid">
                            </br>
                            <input type="password" name="password" placeholder="Password">
                            </br>
                            <input type="submit" value="Login"></form>)");
}

void SoftAPServer::handleLogin() {
    Serial.println("handleLogin");
    if(captivePortal()) {
        return;
    }
    if (!server.hasArg("SSID") && !server.hasArg("password")) {
        if (server.arg("SSID") == nullptr || server.arg("password") == nullptr) {
            server.send(400, "text/plain","400: Invalid Request");
            return;
        }
    }
    String ssid_to_connect = server.arg("SSID");
    String pass_to_connect = server.arg("password");
    server.send(200, "text/html", "<h1>Connecting to " + server.arg("SSID") + "</h1>");

    WiFi.begin(ssid_to_connect, pass_to_connect);
    uint8_t checkCount = 0;
    while (WiFi.status() != WL_CONNECTED && checkCount < 60){
        Serial.print(".");
        delay(1000);
        checkCount++;
    }
    if(WiFi.status() != WL_CONNECTED){
        Serial.println("\nStart soft ap");
        SoftAPServer::startSoftAp();
    } else{
        Serial.println("\nConnected");
        Serial.println("IP: " + WiFi.localIP().toString());
    }

    WiFi.softAPdisconnect(true);
    server.close();
    dnsServer.stop();
    needsProcessing = false;
}

void SoftAPServer::handleNotFound() {
    Serial.println("handleNotFound");
    if(captivePortal()) {
        return;
    }
    server.send(404, "text/plain","404: Not found");
}

bool SoftAPServer::isIp(const String& str) {
    for (size_t i = 0; i < str.length(); i++) {
        int c = (unsigned char)str.charAt(i);
        if (c != '.' && (c < '0' || c > '9')) {
            return false;
        }
    }
    return true;
}

bool SoftAPServer::captivePortal() {
    if (!isIp(server.hostHeader())) {
        server.sendHeader("Location", String("http://") + server.client().localIP().toString(), true);
        server.send(302, "text/plain", ""); // Empty content inhibits Content-length header so we have to close the socket ourselves.
        server.client().stop(); // Stop is needed because we sent no content length
        return true;
    }
    return false;
}

bool SoftAPServer::needsToProcess() {
    return needsProcessing;
}
