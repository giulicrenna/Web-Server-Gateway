#ifdef isESP8266
#include <esp8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <Arduino.h>
#include <myMqtt.hpp>

class Interpretator
{
public:
    const char *command0 = "I"; // I send 1 or 0 if im connected -> I send my local IP -> I send the SSID i connected from -> I send the RSSI
    const char *command1 = "A"; // PIC gives me the broker id -> PIC gives me clientID -> I send o or 1 if the connection was succesfull
    const char *command2 = "S"; // PIC send topic where to subscribe
    const char *command3 = "P"; // PIC send topic where to publish -> payload -> 1/0
    const char *command4 = "Q"; // I send recursively the messages
    const char *command5 = "H"; // I send gprs-wifi config
    const char *command6 = "L1";
    const char *command7 = "L2";
    const char *command8 = "L3";
    const char *command9 = "N";
    const char *command10 = "C";
    const char *command11 = "L4";

    // private:
    virtual void interpretateCommandTask()
    {
        String send[10] = {"", "", "", "", "", "", "", "", "", ""};
        String cmd[5] = {"", "", "", "", ""};
        #ifdef DEBUG
        String str = Serial.readStringUntil('\n');
        #else
        String str = Serial.readStringUntil('\n');
        #endif

        std::vector<std::string> cmd_v = mstd::strip(str.c_str(), '\t');

        for(int k=0; k < cmd_v.size(); k++){cmd[k] = String(cmd_v.at(k).c_str());}

        if (cmd[0] != "")
        {
            #ifdef DEBUG
            Serial.println("----------------<Incomming Command>----------------");
            Serial.println(">" + cmd[0]);
            Serial.println(">" + cmd[1]);
            Serial.println(">" + cmd[2]);
            #endif
            if (cmd[0] == command0)
            {
                if (WiFi.isConnected())
                {
                    send[0] = String("1");
                    send[1] = WiFi.localIP().toString();
                    send[2] = WiFi.SSID();
                    send[3] = String(giveRSSI());
                }
                else
                {
                    send[0] = "0";
                    send[1] = "0.0.0.0";
                    send[2] = "None";
                    send[3] = "0";
                }
            }
            if (cmd[0] == command1)
            {
                mqttCredentials.mqttBroker = cmd[1];
                mqttCredentials.clientID = cmd[2];
                mqttCredentials.port = std::stoi(cmd[3].c_str());
                if (mqttSetup(cmd[1], mqttCredentials.port) == 1)
                {
                    send[0] = "1";
                }
                else
                {
                    send[0] = "0";
                }
            }
            if (cmd[0] == command2)
            {
                mqttCredentials.subsTopic = cmd[1];
                int couldSubs = mqttSubscribe();
                if (couldSubs == 1)
                {
                    send[0] = "1";
                }
                else
                {
                    send[0] = "0";
                }
            }
            if (cmd[0] == command3)
            {
                mqttCredentials.pubTopic = cmd[1];
                String payload = cmd[2];
                int result = mqttOnLoop(mqttCredentials.pubTopic,
                                        payload);
                if (result == 1)
                {
                    send[0] = "1";
                }
                else if (result == 0)
                {
                    send[0] = "0";
                }
            }
            if (cmd[0] == command4)
            {
                try
                {
                    topics = topics_.giveDataSet();
                    messages = myMessages_.giveDataSet();

                    for (int i = 0; i < mQueueSize; i += 1)
                    {
                        String messagesStr = topics.at(i).c_str();
                        messagesStr += ":";
                        messagesStr += messages.at(i).c_str();
                        send[i] = messagesStr;
                    }
                }
                catch (...)
                {
#ifdef DEBUG
                Serial.println("Could't open data set...");
#endif
                }
            }
            if (cmd[0] == command5)
            {
                String data;
                if (config.gprs == "on")
                {
                    data += "1/";
                }
                else
                {
                    data += "0/";
                }
                if (config.wifi == "on")
                {
                    data += "1";
                }
                else
                {
                    data += "0";
                }

                send[0] = data;
            }
            if (cmd[0] == command6)
            {
                input.l1 = cmd[1];
            }
            if (cmd[0] == command7)
            {
                input.l2 = cmd[1];
            }
            if (cmd[0] == command8)
            {
                input.l3 = cmd[1];
            }
            if (cmd[0] == command9)
            {
                input.nodes = cmd[1];
            }
            if (cmd[0] == command10) 
            {
                if (!WiFi.isConnected())
                {
                    config.ssid = cmd[1];
                    config.password = cmd[2];
                    WiFiSetter::setupWifiSta();
                    if(WiFi.isConnected()){
                        currentState = START_INTERPRETATOR_LOCAL_SERVER;
                        send[0] = "1";
                        putData();
                    }else{
                        send[0] = "1";
                    }
                }
            }
            if (cmd[0] == command11){
                cmd[1] = input.l4;
            }
            #ifdef DEBUG
                delay(2000);
            #endif
            if (send[0] != ""){
                for (int i = 0; i < 10; i++){
                    Serial.print(send[i] + "\t");
                }
                Serial.print("\r\n");
            }
        }
    }

    int giveRSSI()
    {
        int signal = 0;
        int rssi = WiFi.RSSI();
        if((rssi >= -185) && (rssi <= -90)){
            signal = 0.111 * rssi + 15;
        }
        if((rssi >= -89) && (rssi <= -80)){
            signal = rssi + 95;
        }
        if((rssi >= -79) && (rssi <= -70)){
            signal = 1.5 * rssi + 135;
        }
        if((rssi >= -69) && (rssi <= -50)){
            signal = 2 * rssi + 170;
        }
        if((rssi >= -49) && (rssi <= -30)){
            signal = 1.25 * rssi + 132.5;
        }if((rssi >= -29) && (rssi <= -0)){
            signal = 0.167 * rssi + 100;
        }

        return signal;
    }
};