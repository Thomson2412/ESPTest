#include <ESP8266WiFi.h>
#include "SoftAPServer.h"

const uint8_t LED_RED = D0;
const uint8_t LED_GREEN = D1;
const uint8_t LED_BLUE = D2;

const uint8_t BUTTON = D5;

WiFiUDP Udp;
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
char  replyPacket[] = "Hi there! Got the message :-)";  // a reply string to send back

bool startedUDP = false;

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
        while (WiFi.status() != WL_CONNECTED){
            Serial.print(".");
            delay(500);
        }
        Serial.println("\nConnected");
        Serial.println("IP: " + WiFi.localIP().toString());
    }
}

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize) {
        // receive incoming UDP packets
        Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
        int len = Udp.read(incomingPacket, 255);
        if (len > 0)
        {
            incomingPacket[len] = 0;
        }
        Serial.printf("UDP packet contents: %s\n", incomingPacket);

        switch (incomingPacket[0]) {
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

        // send back a reply, to the IP address and port we got the packet from
        Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
        Udp.write(replyPacket);
        Udp.endPacket();
    }

    if(SoftAPServer::needsToProcess()){
        SoftAPServer::processRequest();
    } else{
        if(!startedUDP){
            Udp.begin(localUdpPort);
            Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
            startedUDP = true;
        }
    }
}