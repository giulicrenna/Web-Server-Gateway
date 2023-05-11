#include <mStandars.h>
#include <Arduino.h>
#include <vector>
#include <string.h>
#include <IPAddress.h>
#include <HardwareSerial.h>
#include "global.hpp"
#include "wifi.hpp"
#include "interpreteSerial.hpp"
// 15681544
Interpretator myInterprete;

void task1()
{
    for (;;)
    {
        myInterprete.interpretateCommandTask();
        switch (currentState)
        {
        case START_STA:
        {
            WiFiSetter::setupWifiSta();
            bool status = WiFi.isConnected();
            if (status)
            {
#ifdef DEBUG
                Serial.println("[*] Killing AP");
#endif
                server.end();
                currentState = START_INTERPRETATOR_LOCAL_SERVER;
                break;
            }
            else
            {
                currentState = START_AP;
                break;
            }
        }

        case START_AP:
        {
            WiFiSetter::startServer();
        }

        case START_INTERPRETATOR_LOCAL_SERVER:
        {

            WiFiSetter::startServer(true);
            currentState = START_INTERPRETATOR;
            break;
        }

        case START_INTERPRETATOR:
        {
            mqttClient.poll();
            break;
        }

        default:
            break;
        }

        if (millis() - lastTimeCleanData_l4 > 5000)
        {
            input.l4 = "";
        }
    }
}

void setup()
{
    Serial.begin(115200);
    // Serial.onReceiveError(handleUART);
    topics_.startDataSet();
    myMessages_.startDataSet();
#ifdef DEBUG
    Serial.println("INICIADO\r\n");
#endif
}

void loop()
{
    task1();
}