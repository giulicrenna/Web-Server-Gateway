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

void task2()
{
    for (;;)
    {
        switch (currentState)
        {
        case START_STA:
        {
            myInterprete.interpretateCommandTask();
        }

        case START_INTERPRETATOR_LOCAL_SERVER:
        {
            myInterprete.interpretateCommandTask();
        }

        case START_INTERPRETATOR:
        {
            myInterprete.interpretateCommandTask();
        }

        default:
            break;
        }
    }
}

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
                server.reset();
                currentState = START_INTERPRETATOR_LOCAL_SERVER;
                break;
            }else{
                currentState = START_AP;
                break;
            }
        }

        case START_AP:
        {
            WiFiSetter::setupApMode();
        }

        case START_INTERPRETATOR_LOCAL_SERVER:
        {
            WiFiSetter::setupLocalServer();
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

        if(millis() - lastTimeCleanData_l4  > 5000){
            input.l4 = "";
        }
    }
}

void setup()
{
    Serial.begin(115200);
    #ifdef DEBUG
    delay(100);
    #endif
    Serial1.begin(115200);
    //Serial.onReceiveError(handleUART);
    beginEEPROM();
    loadData();
    #ifdef DEBUG
    Serial.println(config.ssid + ";" + config.password + ";" + config.gprs + ";" + config.wifi);
    #endif
    topics_.startDataSet();
    myMessages_.startDataSet();
}

void loop() { 
    task1();
    task2();
    delay(1);
}