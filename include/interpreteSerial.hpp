#ifdef isESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <Arduino.h>
#include <myMqtt.hpp>

class Interpretator
{
public:
    const char *command0 = "I"; // I send 1 or 0 if im connected -> I send my local IP -> I send the SSID i connected from -> I send the RSSI
    const char *command1 = "MQTT"; // PIC gives me the broker id -> PIC gives me clientID -> I send o or 1 if the connection was succesfull
    const char *command2 = "S"; // PIC send topic where to subscribe
    const char *command3 = "P"; // PIC send topic where to publish -> payload -> 1/0
    const char *command4 = "Q"; // I send recursively the messages
    const char *command5 = "H"; // (DEPRECADO) I send gprs-wifi config 
    const char *command6 = "L1"; // (DEPRECADO)
    const char *command7 = "L2"; // (DEPRECADO)
    const char *command8 = "L3";
    const char *command9 = "N";
    const char *command10 = "C"; // WiFi setup
    const char *command11 = "L4";
    const char *command12 = "ERASE";
    const char *command13 = "M"; // Cantidad de nodos
    const char *command14 = "STATUS"; // JSON
    // private:
    virtual void interpretateCommandTask(){
        try
        {
            interpretateNoExcept();
        }
        catch(const std::exception& e)
        {
            return;
        }
        
    }

    virtual void interpretateNoExcept()
    {
        String send[10] = {"", "", "", "", "", "", "", "", "", ""};
        String cmd[5] = {"", "", "", "", ""};

        String str = Serial.readStringUntil('\n');

        std::vector<std::string> cmd_v = mstd::strip(str.c_str(), '\t');

        for(unsigned int k=0; k < cmd_v.size(); k++){cmd[k] = String(cmd_v.at(k).c_str());cmd[k].trim();}

        if (cmd[0] != "")
        {
            printTest("\n----------------<Incomming Command>----------------");
            printTest("\n>" + cmd[0]);
            printTest("\n>" + cmd[1]);
            printTest("\n>" + cmd[2]);
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
                mqttCredentials.port = std::stoi(cmd[2].c_str());
                mqttCredentials.user = cmd[3];
                mqttCredentials.password = cmd[4];
                if (mqttSetup(mqttCredentials.mqttBroker, mqttCredentials.port) == 1)
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

                    if(mqttCredentials.lastMessage == ""){
                        Serial.print("0\r\n");
                        return;
                    }else{
                        Serial.print("1\r\n");
                        Serial.print(mqttCredentials.lastTopic);
                        delay(10);
                        Serial.print(mqttCredentials.lastMessage);
                        delay(10);
                        mqttCredentials.lastMessage = "";
                        return;
                    }
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
                    printTest("Could't open data set...");
                }
            }
            if (cmd[0] == command5)
            {
                return;
                String data;
                if (config.gprs == "on")
                {
                    data += "1";
                }
                else
                {
                    data += "0";
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
                        printTest("\nSTART_INTERPRETATOR");
                        send[0] = "1";
                    }else{
                        send[0] = "0";
                        currentState = START_AP;
                    }
                }
            }
            if (cmd[0] == command11){
                cmd[1] = input.l4;
            }
            if(cmd[0] == command12){
                ESP.restart();
            }

            if(cmd[0] == command13){
                input.cantNodos = cmd[1];
            }

            if(cmd[0] == command14){
                input.statusJSON = cmd[1];
            }

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