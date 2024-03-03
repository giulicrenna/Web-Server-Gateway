#include <mStandars.h>
#include <Arduino.h>
#include <vector>
#include <string.h>
#include <IPAddress.h>
#include <HardwareSerial.h>
#include "global.hpp"
#include "wifi.hpp"
#include "interpreteSerial.hpp"

Interpretator myInterprete;

void task1()
{
    switch (currentState)
    {
    case START_STA:
    {
        printTest("[*] Killing AP\n");
        WiFiSetter::setupWifiSta();
        bool status = WiFi.isConnected();
        if (status)
        {
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
        printTest("START_AP\n");
        startAP();
        WiFiSetter::startServer();
        currentState = WAIT_FOR_CONNECTION;
        break;
    }

    case WAIT_FOR_CONNECTION:
    {
        break;
    }

    case START_INTERPRETATOR_LOCAL_SERVER:
    {
        printTest("\nSTART_INTERPRETATOR_LOCAL_SERVER");
        currentState = START_INTERPRETATOR;
        break;
    }

    case START_INTERPRETATOR:
    {
        if(!WiFi.isConnected()){
           currentState = START_AP; 
        }
        
        mqttClient.poll();
        break;
    }

    default:
        break;
    
    }
}

void setup()
{
    Serial.begin(baudrate);
    // Serial.onReceiveError(handleUART);
    LittleFS.begin();
    topics_.startDataSet();
    myMessages_.startDataSet();
    sendings.startDataSet();
    
    Serial.println("INICIADO\r\n");
}


void loop()
{
    myInterprete.interpretateNoExcept();
    task1();
}
