//
// Created by thomas on 13-12-20.
//

#ifndef ESPTEST_SOFTAPSERVER_H
#define ESPTEST_SOFTAPSERVER_H

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>

class SoftAPServer {
public:
    static void startSoftAp();
    static void processRequest();
    static bool needsToProcess();

private:
    static void handleRoot();
    static void handleLogin();
    static void handleNotFound();
    static bool captivePortal();

    static bool isIp(const String& str);
};


#endif //ESPTEST_SOFTAPSERVER_H
