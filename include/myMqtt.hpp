#ifndef MYMQTT_H
#define MYMQTT_H
#include <ArduinoMqttClient.h>
#include <WiFiClient.h>
#include <string.h>
//#include <mStand

WiFiClient client_;
MqttClient mqttClient(client_);
mStructures::mQueue myMessages_(5);
mStructures::mQueue topics_(5);

/**
 * @brief This function handle the incomming messages from a particular topic
 *
 * @param messageSize
 */
void onMqttMessage(int messageSize)
{
    // we received a message, print out the topic and contents
    Serial1.print("Received a message with topic '");
    Serial1.print(mqttClient.messageTopic());
    Serial1.print("', length ");
    Serial1.print(messageSize);
    Serial1.println(" bytes:");

    String newContent;

    // use the Stream interface to print the contents
    while (mqttClient.available())
    {
        newContent += (char)mqttClient.read();
    }
    topics = topics_.manageDataSet(mqttClient.messageTopic().c_str());
    messages = myMessages_.manageDataSet(newContent.c_str());
}

int mqttSetup(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, const char *PATH_ALT = "")
{
    topics_.startDataSet();
    myMessages_.startDataSet();
    if (!mqttClient.connect(MQTT_SERVER, MQTT_PORT))
    {
        return 1;
    }
    else
    {
        mqttClient.setId(mqttCredentials.clientID.c_str());
        mqttClient.onMessage(onMqttMessage);
        Serial1.println("(MQTT instance) You're connected to the MQTT broker!");
        return 0;
    }
}

int mqttSubscribe()
{
    mqttClient.setUsernamePassword("", "");
    mqttClient.setCleanSession(true);
    if (mqttClient.subscribe(mqttCredentials.subsTopic.c_str(), 2) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
    return 2;
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
int mqttOnLoop(const char *MQTT_SERVER, uint16_t MQTT_PORT, const char *PATH, const char *MESSAGE = "")
{
    if (!mqttClient.connected())
    {
        mqttClient.connect(MQTT_SERVER, MQTT_PORT);
    }
    mqttClient.poll();

    if (!mqttClient.beginMessage(PATH))
    {
        Serial1.println("(MQTT instance) could not publish into the topic");
        return 0;
    }
    mqttClient.print(MESSAGE);
    mqttClient.endMessage();

    return 1;
}

#endif
