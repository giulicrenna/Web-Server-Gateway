#include <Arduino.h>
#include <string.h>
#include <global.hpp>
#include "wifi.hpp"
#include "interpreteSerial.hpp"
// 15681544
WiFiSetter myServer;

void setup()
{   
    Serial.begin(115200);
    beginEEPROM();
    setupWifiSta();
    bool status = WiFi.isConnected();
    delay(3000);
    if (!status)
    {
        myServer.setupApMode();
    }
}

void loop()
{
    switch (currentState)
    {
    case ESP_AS_AP:
    {
        input.l1 = random(15000);
        input.l2 = random(40000);
        Serial.println(config.ssid + ";" + config.password + ";" + config.gprs + ";" + config.wifi);
    }
    break;

    case START_STA:
    {
        setupWifiSta();
        bool status = WiFi.isConnected();
        delay(3000);
        if (status)
        {
            Serial.println("Connection was successful...");
            currentState = START_INTERPRETATOR_LOCAL_SERVER;
            break;
        }
    }

    case START_INTERPRETATOR_LOCAL_SERVER:
    {
        myServer.setupLocalServer();
        currentState = START_INTERPRETATOR;
        break;
    }

    case START_INTERPRETATOR:
    {
        Serial.print(".");
        delay(10);
    }

    default:
        break;
    }
}