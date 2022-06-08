#include <Arduino.h>

#include <OneWire.h>
#include <DallasTemperature.h>
#include<WiFiSetup.h>
#include <ESPAsyncWebServer.h>

// GPIO where the DS18B20 is connected to
const int oneWireBus = 4;     

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(oneWireBus);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

AsyncWebServer server(80);
WiFiSetup wifiSetup(&server);

float currentTemperatureC = 0;
float currentTemperatureF = 0;

void startTermometer(){
  sensors.begin();
}

void setup() {
  Serial.begin(115200);
  
  startTermometer();

  if(wifiSetup.connect()){
    server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
      requestTemp();

      if(currentTemperatureC == -127.00){
        request->send(503, "Service unavailable: Temperature sensor failed to get reading");
      }

      String resultJson = "{";
      resultJson.concat("\"celcius\":");
      resultJson.concat(currentTemperatureC);
      resultJson.concat(",");
      resultJson.concat("\"fahrenheit\":");
      resultJson.concat(currentTemperatureF);
      resultJson.concat("}");

      Serial.println(resultJson);

      request->send(200, "application/json", resultJson);
    });

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){ 
      request->send(SPIFFS, "/dashboard.html", String(), false); 
    });
  } else {
    wifiSetup.startWiFiSetup();
  }
}

void requestTemp(){
  sensors.requestTemperatures(); 
  currentTemperatureC = sensors.getTempCByIndex(0);
  currentTemperatureF = sensors.getTempFByIndex(0);
}

void loop() {
  // put your main code here, to run repeatedly:
}