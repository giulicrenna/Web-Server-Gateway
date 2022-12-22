#ifndef MYMQTT_H
#define MYMQTT_H
#include <ArduinoMqttClient.h>
#include <WiFiClient.h>
#include <string.h>
//#include <mStand

WiFiClient client_;
MqttClient mqttClient(client_);

/**
 * @brief This function handle the incomming messages from a particular topic
 *
 * @param messageSize
 */
void onMqttMessage(int messageSize)
{
    // we received a message, print out the topic and contents
    String topic = mqttClient.messageTopic();
    #ifdef DEBUG
    Serial.print("Received a message with topic '");
    Serial.print(topic);
    Serial.print("', length ");
    Serial.println(messageSize + '\n');
    #endif

    String newContent;

    // use the Stream interface to print the contents
    while (mqttClient.available())
    {
        newContent += (char)mqttClient.read();
    }
    topics_.manageDataSet(topic.c_str());
    myMessages_.manageDataSet(newContent.c_str());
}

int mqttSetup(const char *MQTT_SERVER, uint16_t MQTT_PORT)
{
    if (mqttClient.connect(MQTT_SERVER, MQTT_PORT))
    {
        mqttClient.setId(mqttCredentials.clientID.c_str());
        mqttClient.setUsernamePassword("", "");
        mqttClient.setCleanSession(true);
        mqttClient.onMessage(onMqttMessage);
        #ifdef DEBUG
        Serial.println("(MQTT instance) You're connected to the MQTT broker!");
        #endif
        return 1;
    }
    else
    {
        #ifdef DEBUG
        Serial.println(std::to_string(mqttClient.connectError()).c_str());
        #endif
        return 0;
    }
}

int mqttSubscribe()
{
    if (!mqttClient.subscribe(mqttCredentials.subsTopic.c_str(), 0))
    {
        return 0;
    }
    else
    {
        return 1;
    }
    return 0;
}

/**
 * @brief mqttOnLoop receive all the neccesary data to send a MQTT message
 *
 * @param MQTT_SERVER MQTT host
 * @param MQTT_PORT MQTT port (1883 no SSL, 8883 SSL)
 * @param PATH Topic where the message will be sent
 * @param client WiFi Instance
 * @param PATH_ALT Alternative topic, Empty String by default
 * @param TOPIC System topic
 * @param MESSAGE Message to be sent
 */
int mqttOnLoop(const char *PATH, const char *MESSAGE = "")
{
    if (!mqttClient.connected())
    {
        mqttClient.connect(mqttCredentials.mqttBroker.c_str(), 1883);
    }
    mqttClient.poll();

    if (!mqttClient.beginMessage(PATH))
    {
        #ifdef DEBUG
        Serial.println("(MQTT instance) could not publish into the topic");
        #endif
        return 0;
    }else{
        #ifdef DEBUG
        Serial.println("(MQTT instance) message sent succesfully");
        #endif
        mqttClient.print(MESSAGE);
        mqttClient.endMessage();
        return 1;
    }
}

#endif
