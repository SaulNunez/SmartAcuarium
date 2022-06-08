#ifndef WiFiSetup_h
#define MyClass_h

#include<Arduino.h>
#include<ESPAsyncWebServer.h>

class WiFiSetup
{
private:
    AsyncWebServer *server;
    void setupWifiAP();
public:
    WiFiSetup(AsyncWebServer *server);
    ~WiFiSetup();
    void startWiFiSetup();
    bool connect();
};

#endif