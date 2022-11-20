#include <Arduino.h>
#include <WiFi.h>
#include <myMqtt.hpp>
#include "myQueue.hpp"
//#include "global.hpp"

class Interpretator
{
public:
    String command0 = "I"; // I send 1 or 0 if im connected -> I send my local IP -> I send the SSID i connected from -> I send the RSSI
    String command1 = "A"; // PIC gives me the broker id -> PIC gives me clientID -> I send o or 1 if the connection was succesfull
    String command2 = "S";
    String command3 = "P";
    String command4 = "Q";
    String command5 = "H";
    String command6 = "L1";
    String command7 = "L2";
    String command8 = "L3";
    String lastComm = "None";
    int count = 0;

private:
    void interpretateCommand()
    {
        String cmd = readSerial();
        if (cmd == command0 || lastComm == command0)
        {
            lastComm = cmd;
            if (WiFi.isConnected())
            {
                Serial.print("1\r\n");
                delay(10);
                Serial.print(String(WiFi.localIP()) + "\r\n");
                delay(10);
                Serial.print(WiFi.SSID() + "\r\n");
                delay(10);
                Serial.print(String(giveRSSI()) + "\r\n");
            }
            else
            {
                Serial.print("0\r\n");
                delay(10);
                Serial.print("0.0.0.0\r\n");
                delay(10);
                Serial.print("None\r\n");
                delay(10);
                Serial.print("0\r\n");
            }
        }
        else if (cmd == command1 || lastComm == command1)
        {
            lastComm = cmd;
            
            if(cmd == lastComm && count == 0){
                count ++;
            }else if(count == 1){
                mqttCredentials.mqttBroker = cmd;
                count ++;
            }else if(count == 2){
                mqttCredentials.clientID = cmd;
                count ++;
            }else if(count == 3){
                if(mqttSetup(mqttCredentials.mqttBroker.c_str(), 1883, "") == 1){
                    Serial.print("1\r\n");
                }else{
                    Serial.print("0\r\n");
                }
                count = 0;
            }
        }
    }
    String readSerial(int serialNum = 0)
    {
        String data = Serial.readString();
        data.trim();
        return data;
    }
    int giveRSSI()
    {
        return 0;
    }
};