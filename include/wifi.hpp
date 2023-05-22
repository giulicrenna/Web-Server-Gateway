#ifdef isESP8266
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#else
#include <WiFi.h>
#include <AsyncTCP.h>
#endif
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
// #include "global.hpp"

AsyncWebServer server(80);

void printTest(String msg);
void beginEEPROM();
void setupWifiSta();
void startAP();
bool loadData();
String processor();
String processorLocal();

class WiFiSetter
{
protected:
public:
    static void startServer()
    {
        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/style.css", "text/css"); });
        server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/script.js", "text/javascript"); });
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/index.html", "text/html"); });
        server.on("/allvalues", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", processor()); });
        server.on("/logo", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/logo.png", "image/png"); });

        server.on("/pub", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      String input;
                      if (request->hasParam("cmd"))
                      {
                          input = request->getParam("cmd")->value();
                          Serial.print("CMD\t" + input + "\r\n");
                      } });

        server.on("/guardar_salir", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                    String gprsState, wifiState, inputMessage;

                    if (request->hasParam("wifi"))
                    {
                        inputMessage = request->getParam("wifi")->value();
                        config.wifi = inputMessage;
                    }
                    if (request->hasParam("gprs"))
                    {
                        inputMessage = request->getParam("gprs")->value();
                        config.gprs = inputMessage;
                    }
                    if (config.gprs == "on")
                    {
                        gprsState = "1";
                    }
                    else
                    {
                        gprsState = "0";
                    }

                    if (config.wifi == "on")
                    {
                        wifiState = "1";
                    }
                    else
                    {
                        wifiState = "0";
                    }

                    Serial.print("HABILITACIONES\t" + gprsState + "/" + wifiState + "\r\n");
                    request->send(200, "text/html", "WebServer cerrado."); 
                    server.end(); });

        server.on("/aplicar_mqtt", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                    String inputMessage, inputParam;

                    if (request->hasParam("host"))
                      {
                          inputMessage = request->getParam("host")->value();
                          mqttCredentials.mqttBroker = inputMessage;
                      }
                      if (request->hasParam("port"))
                      {
                          inputMessage = request->getParam("port")->value();
                          mqttCredentials.port = inputMessage.toInt();
                      }
                      if (request->hasParam("user"))
                      {
                          inputMessage = request->getParam("user")->value();
                          mqttCredentials.user = inputMessage;
                      }
                      if (request->hasParam("pass"))
                      {
                          inputMessage = request->getParam("pass")->value();
                          mqttCredentials.password = inputMessage;
                      } 
                      
                    Serial.print("MQTT\t" + mqttCredentials.mqttBroker + "\t" + mqttCredentials.port + "\t" + mqttCredentials.user + "\t" + mqttCredentials.password + "\r\n");
                      
                    request->send(200, "text/html", "Parametros establecidos correctamente. <br><a href=\"/\">Volver al panel de configuracion..</a>"); });

        server.on("/aplicar_gprs", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      String inputMessage, inputParam;

                      if (request->hasParam("gprs"))
                      {
                          inputMessage = request->getParam("gprs")->value();
                          inputParam = "gprs";
                          config.gprs = inputMessage;
                      }
                      else
                      {
                          config.gprs = "off";
                      }
                      if (request->hasParam("apn"))
                      {
                          inputMessage = request->getParam("apn")->value();
                          config.apn = inputMessage;
                      } 
                      
                        Serial.print("APN\t" + config.apn + "\r\n"); 
                        
                    request->send(200, "text/html", "Parametros establecidos correctamente. <br><a href=\"/\">Volver al panel de configuracion..</a>"); });
        server.on("/aplicar_wifi", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                      String inputMessage, inputParam;

                      if (request->hasParam("ssid"))
                      {
                          inputMessage = request->getParam("ssid")->value();
                          inputParam = "ssid";
                          config.ssid = inputMessage;
                      }
                      if (request->hasParam("pass"))
                      {
                          inputMessage = request->getParam("pass")->value();
                          inputParam = "pass";
                          config.password = inputMessage;
                      }
                      if (request->hasParam("wifi"))
                      {
                          inputMessage = request->getParam("wifi")->value();
                          inputParam = "wifi";
                          config.wifi = inputMessage;
                      }
                      else
                      {
                          config.wifi = "off";
                      }

                      Serial.print("C\t" + config.ssid + "\t" + config.password + "\r\n");
                    request->send(200, "text/html", "Parametros establecidos correctamente. <br><a href=\"/\">Volver al panel de configuracion..</a>"); });

        server.begin();
        printTest("[*] Web server started\n");
    }

    static void setupWifiSta()
    {
        if (config.ssid != "")
        {
            int count = 0;
            WiFi.mode(WIFI_STA);
            WiFi.begin(config.ssid.c_str(), config.password.c_str());

            while (!WiFi.isConnected())
            {
                if (count == 20000)
                {
                    printTest("Could not connect to wifi\n");
                    break;
                }
                count++;
                delay(1);
            }
            if (WiFi.status() == WL_CONNECTED)
            {
                printTest("\nDevice Connected to the network: " + WiFi.localIP().toString() + " is the current ip\n");
            }
        }
        else
        {
            currentState = START_AP;
        }
    }
};

void startAP()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password);

    int numberOfWifi = WiFi.scanNetworks();

    for (int i = 1; i < numberOfWifi; i++)
    {
        if (i <= 9)
        {
            input.WiFiScan[i] = WiFi.SSID(i);
        }
    }
}

String processor()
{
    String output = "";
    output += input.nodes + ";";
    output += input.cantNodos + ";";
    output += input.statusJSON + ";";
    output += config.ssid + ";";
    output += config.password + ";";
    output += config.apn + ";";
    output += mqttCredentials.mqttBroker + ";";
    output += String(mqttCredentials.port) + ";";
    output += mqttCredentials.user + ";";
    output += mqttCredentials.password + ";";

    return output;
}

void printTest(String msg)
{
#ifdef DEBUG
    Serial.print(msg);
#endif
}