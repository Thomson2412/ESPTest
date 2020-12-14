#include <ESP8266WiFi.h>
#include "SoftAPServer.h"

const uint8_t LED_RED = D0;
const uint8_t LED_GREEN = D1;
const uint8_t LED_BLUE = D2;

const uint8_t BUTTON = D5;

WiFiUDP udp;
const unsigned int UDP_PORT = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Message received \n";  // a reply string to send back

bool startedUDP = false;
bool connectedToServer = false;

const IPAddress BROADCAST_ADDR = IPAddress(255, 255, 255, 255);
//Will not work after 50 days
unsigned long lastBroadcastMS = 0;

void doLedStuff(char c);

void setup() {
    Serial.begin(115200);
    Serial.println("\nSetup");

    pinMode(BUTTON, INPUT_PULLUP);

    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    digitalWrite(LED_RED, HIGH);
    digitalWrite(LED_GREEN, HIGH);
    digitalWrite(LED_BLUE, HIGH);

    if(digitalRead(BUTTON) == LOW) {
        Serial.println("Start soft ap");
        SoftAPServer::startSoftAp();
    } else{
        Serial.println("Connecting");
        uint8_t checkCount = 0;
        while (WiFi.status() != WL_CONNECTED && checkCount < 60){
            Serial.print(".");
            delay(1000);
            checkCount++;
        }
        //Common function? Callback? Used here and in softAPServer
        if(WiFi.status() != WL_CONNECTED){
            Serial.println("\nStart soft ap");
            SoftAPServer::startSoftAp();
        } else{
            Serial.println("\nConnected");
            Serial.println("IP: " + WiFi.localIP().toString());
        }
    }
}

void loop() {
    int packetSize = udp.parsePacket();
    if (packetSize) {
        // receive incoming UDP packets
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort());
        int len = udp.read(incomingPacket, 255);
        if (len > 0)
        {
            incomingPacket[len] = 0;
        }
        Serial.printf("UDP packet contents: %s\n", incomingPacket);

        doLedStuff(incomingPacket[0]);

        // send back a reply, to the IP address and port we got the packet from
        udp.beginPacket(udp.remoteIP(), udp.remotePort());
        udp.write(replyPacket);
        udp.endPacket();

        if(incomingPacket[0] == 't'){
            Serial.println("Got terminate: connectedToServer = true");
            connectedToServer = true;
        }
    }

    if(SoftAPServer::needsToProcess()){
        SoftAPServer::processRequest();
    } else{
        if(!startedUDP){
            udp.begin(UDP_PORT);
            Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), UDP_PORT);
            startedUDP = true;
        }
        if(!connectedToServer){
            if(millis() - lastBroadcastMS > 1000) {
                Serial.println("Send broadcast");
                udp.beginPacketMulticast(BROADCAST_ADDR, UDP_PORT, WiFi.localIP());
                udp.write("Hello I'm here");
                udp.endPacket();
                lastBroadcastMS = millis();
            }
        }
    }
}

void doLedStuff(char c){
    switch (c) {
        case 'r':
            digitalWrite(LED_RED, LOW);
            break;
        case 'g':
            digitalWrite(LED_GREEN, LOW);
            break;
        case 'b':
            digitalWrite(LED_BLUE, LOW);
            break;
        case 'd':
            for(uint8_t i = 0; i < 3; i++){
                digitalWrite(LED_RED, LOW);
                delay(500);
                digitalWrite(LED_RED, HIGH);
                digitalWrite(LED_GREEN, LOW);
                delay(500);
                digitalWrite(LED_GREEN, HIGH);
                digitalWrite(LED_BLUE, LOW);
                delay(500);
                digitalWrite(LED_BLUE, HIGH);


                digitalWrite(LED_RED, LOW);
                delay(500);
                digitalWrite(LED_GREEN, LOW);
                delay(500);
                digitalWrite(LED_BLUE, LOW);
                delay(500);

                digitalWrite(LED_RED, LOW);
                digitalWrite(LED_BLUE, LOW);
                delay(500);
                digitalWrite(LED_RED, HIGH);
                digitalWrite(LED_BLUE, HIGH);

                digitalWrite(LED_GREEN, LOW);
                digitalWrite(LED_BLUE, LOW);
                delay(500);
                digitalWrite(LED_GREEN, HIGH);
                digitalWrite(LED_BLUE, HIGH);

                digitalWrite(LED_RED, HIGH);
                digitalWrite(LED_GREEN, HIGH);
                digitalWrite(LED_BLUE, HIGH);

                for(uint8_t y = 0; y < 10; y++) {
                    digitalWrite(LED_RED, HIGH);
                    delay(50);
                    digitalWrite(LED_RED, LOW);
                    delay(50);
                }
                digitalWrite(LED_RED, HIGH);

                for(uint8_t y = 0; y < 10; y++) {
                    digitalWrite(LED_GREEN, HIGH);
                    delay(50);
                    digitalWrite(LED_GREEN, LOW);
                    delay(50);
                }
                digitalWrite(LED_GREEN, HIGH);

                for(uint8_t y = 0; y < 10; y++) {
                    digitalWrite(LED_BLUE, HIGH);
                    delay(50);
                    digitalWrite(LED_BLUE, LOW);
                    delay(50);
                }
                digitalWrite(LED_BLUE, HIGH);
            }
            break;
        default:
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_BLUE, HIGH);
            break;
    }
}