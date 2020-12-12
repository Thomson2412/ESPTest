#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

IPAddress local_IP(192, 168, 0, 2);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

void handleRoot();
void handleLogin();
void handleNotFound();

ESP8266WebServer server(80);
const String ssid = "ESPx";

void setup() {
    Serial.begin(115200);

    Serial.print("Configuring SoftAP Configuration...");
    Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

    Serial.print("Setting SoftAP...");
    String macAddr = WiFi.macAddress();
    macAddr.replace(":", "");
    Serial.println(WiFi.softAP(ssid + macAddr) ? "Ready" : "Failed!");

    Serial.println();
    Serial.print("connect to the IP Address given below...");
    Serial.println(WiFi.softAPIP());

    server.on("/", HTTP_GET, handleRoot);
    server.on("/connect", HTTP_POST, handleLogin);
    server.onNotFound(handleNotFound);

    server.begin();
}

void loop() {
    server.handleClient();
}

void handleRoot() {
    Serial.println("handleRoot");
    server.send(200, "text/html",
                R"(<form action="/connect" method="POST"><input type="text" name="ssid" placeholder="Ssid"></br><input type="password" name="password" placeholder="Password"></br><input type="submit" value="Login"></form>)");
}

void handleLogin() {
    Serial.println("handleLogin");
    if (!server.hasArg("ssid") && !server.hasArg("password")) {
        if (server.arg("ssid") == nullptr || server.arg("password") == nullptr) {
            server.send(400, "text/plain","400: Invalid Request");
            return;
        }
    }
    String ssid_to_connect = server.arg("ssid");
    String pass_to_connect = server.arg("password");
    server.send(200, "text/html", "<h1>Connecting to " + server.arg("ssid") + "</h1>");
    WiFi.begin(ssid_to_connect, pass_to_connect);
    while (WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(500);
    }
    Serial.println("\nConnected tot WiFi");
    WiFi.softAPdisconnect(true);
    server.close();
}

void handleNotFound() {
    Serial.println("handleNotFound");
    server.send(404, "text/plain","404: Not found");
}