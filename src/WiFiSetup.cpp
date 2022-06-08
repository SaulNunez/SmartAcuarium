
#ifdef ESP32
#include <WiFi.h>
#else
#include <ESP8266WiFi.h>
#endif

#include <FS.h>
#ifdef ESP32
#include "SPIFFS.h"
#endif

#include<WiFiSetup.h>
#include <ESPAsyncWebServer.h>

void WiFiSetup::setupWifiAP()
{
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP("IntelliPlug1");
  Serial.println("AP address:" + WiFi.softAPIP().toString());
}

// Setup code necesary for setup mode
// We need to enable stuff to discover networks before creating a soft AP
// In which the user can give their network credential before normal usage
void WiFiSetup::startWiFiSetup()
{
  this->setupWifiAP();

  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(SPIFFS, "/wifi_info.html", String(), false); });

  server->on("/available_networks", HTTP_GET, [](AsyncWebServerRequest *request)
             {
              String returnObj = "{ \"wifiNetworks\": [";
              int networksFound = WiFi.scanNetworks();
              Serial.println("Networks found: " + networksFound);
              for (int i = 0; i < networksFound; i++)
              {
                returnObj.concat("\"" + WiFi.SSID(i) + "\"");
                if (i != networksFound - 1)
                {
                  returnObj.concat(",");
                }
              }
              returnObj.concat("] }");
              Serial.println(returnObj);

              request->send(200, "application/json", returnObj); });

  server->on("/wifi_info", HTTP_POST, [](AsyncWebServerRequest *request)
             {
              String ssid;
              String password;

              if (request->hasParam("ssid", true))
              {
                AsyncWebParameter *ssidParam = request->getParam("ssid", true);
                ssid = ssidParam->value();
              }
              else
              {
                request->send(400, "text/plain", "SSID parameter not found");
                return;
              }

              if (request->hasParam("password", true))
              {
                AsyncWebParameter *passwordParam = request->getParam("password", true);
                password = passwordParam->value();
              }
              else
              {
                request->send(400, "text/plain", "Password parameter not found");
                return;
              }

              Serial.println(("ssid:" + ssid + "\n" + "password:" + password).c_str());

              File f = SPIFFS.open("/wifi.conf", "w");
              if (!f)
              {
                request->send(500);
                return;
              }
              Serial.println("Saving connection parameters...");
              f.print(("ssid:" + ssid + "\n" + "password:" + password).c_str());
              f.close();
              request->send(200);
              ESP.restart(); });
  server->begin();
  Serial.println("HTTP server started");
}

bool WiFiSetup::connect()
{
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return false;
  }

  if (!SPIFFS.exists("/wifi.conf"))
  {
    return false;
  }

  // Encender wifi
  WiFi.mode(WIFI_STA);
  File file = SPIFFS.open("/wifi.conf", "r");
  String wifiFileContents = file.readString();
  file.close();

  int newLineSeparator = wifiFileContents.indexOf('\n');
  String ssidFull = wifiFileContents.substring(0, newLineSeparator);
  String passwordFull = wifiFileContents.substring(newLineSeparator + 1);

  String ssid = ssidFull.substring(5);
  String password = passwordFull.substring(9);
  Serial.println(("ssid:" + ssid + "\n" + "password:" + password).c_str());
  WiFi.begin(ssid.c_str(), password.c_str());

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP Address: ");
  Serial.println(WiFi.localIP());

  return true;
}

WiFiSetup::WiFiSetup(AsyncWebServer *server)
{
    if(server == nullptr){
        server = new AsyncWebServer(80);
    }
}

WiFiSetup::~WiFiSetup()
{
    
}