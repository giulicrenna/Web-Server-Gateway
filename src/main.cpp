#include <mStandars.h>
#include <Arduino.h>
#include <vector>
#include <string.h>
#include <IPAddress.h>
#include <HardwareSerial.h>
#include "global.hpp"
#include "wifi.hpp"
#include "interpreteSerial.hpp"

/*
15681544

T0rn4d0Pr1v4d0

C\tTornado Privado\tT0rn4d0Pr1v4d0\r\n

N\tNodo987889f89b7982b7897b298f89en7f98e2\r\n

M\t2\r\n

STATUS\t{"Status":"ok"}


*/
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
        printTest("\nSTART_INTERPRETATOR");
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
