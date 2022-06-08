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
//WiFiSetup setup(server);

void setup() {
  Serial.begin(115200);
  
  startTermometer();
}

void startTermometer(){
    sensors.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
}