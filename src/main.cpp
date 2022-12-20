#include <Arduino.h>
#include <string.h>
#include <global.hpp>
#include <mStandars.h>
#include "wifi.hpp"
#include "interpreteSerial.hpp"
// 15681544
WiFiSetter myServer;
Interpretator myInterprete;

void task2(void *param)
{
    for (;;)
    {
        switch (currentState)
        {
        case START_STA:
        {
            myInterprete.interpretateCommand();
        }

        case START_INTERPRETATOR_LOCAL_SERVER:
        {
            myInterprete.interpretateCommand();
        }

        case START_INTERPRETATOR:
        {
        }

        default:
            break;
        }
        vTaskDelay(100);
    }
}

void task1(void *param)
{
    for (;;)
    {
        switch (currentState)
        {
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
            myInterprete.interpretateCommand();
        }

        default:
            break;
        }
        vTaskDelay(100);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial2.begin(115200);
    Serial2.setTimeout(2000);
    beginEEPROM();
    loadData();
    setupWifiSta();
    bool status = WiFi.isConnected();
    delay(3000);
    if (!status)
    {
        myServer.setupApMode();
    }
    Serial.println(config.ssid + ";" + config.password + ";" + config.gprs + ";" + config.wifi);

    xTaskCreatePinnedToCore(
        task1,
        "Task 1...",
        10000,
        NULL,
        1,
        NULL,
        0);
    xTaskCreatePinnedToCore(
        task2,
        "Task 2...",
        10000,
        NULL,
        1,
        NULL,
        1);
}

void loop() { delay(500000); }