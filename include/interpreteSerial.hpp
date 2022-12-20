#include <Arduino.h>
#include <WiFi.h>
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

    // private:
    void interpretateCommand()
    {
        String send[10] = {"", "", "", "", "", "", "", "", "", ""};
        String cmd[5] = {"", "", "", "", ""};
        String str = Serial2.readStringUntil('\n');
        std::vector<std::string> cmd_v = mstd::strip(str.c_str(), '\t');

        for(int k=0; k < cmd_v.size(); k++){cmd[k] = String(cmd_v.at(k).c_str());}

        if (cmd[0] != "")
        {
            Serial.println(">" + cmd[0]);
            Serial.println(">" + cmd[1]);
            if (cmd[0] == command0)
            {
                if (WiFi.isConnected())
                {
                    send[0] = String("1");
                    send[1] = String(WiFi.localIP());
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
                if (mqttSetup(cmd[1].c_str(), 1883, "") == 1)
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
                int couldSubs;
                couldSubs = mqttSubscribe();
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
                int result = mqttOnLoop(payload.c_str(),
                                        1883,
                                        mqttCredentials.pubTopic.c_str());
                switch (result)
                {
                case 1:
                    send[0] = "1";
                case 0:
                    send[0] = "0";
                }
            }
            if (cmd[0] == command4)
            {
                for (int i = 0; i < 9; i += 2)
                {
                    send[i] = String(topics.at(i).c_str());
                    send[i + 1] = String(messages.at(i).c_str());
                }
            }
            if (cmd[0] == command5)
            {
                String data;
                if (config.gprs == "on")
                {
                    data += "1";
                    data += "/";
                }
                else
                {
                    data += "0";
                    data += "/";
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

                    setupWifiSta();

                    putData();
                }
            }
            for (int i = 0; i < 10; i++)
            {
                if (send[i] != "")
                {
                    Serial.print(send[i] + "\t");
                }
            }
            Serial.print("\r\n");
        }
    }

    int giveRSSI()
    {
        return 0;
    }
};