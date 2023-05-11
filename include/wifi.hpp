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
    static void startServer(bool isClient = false)
    {
        if(!isClient){
            startAP();
        }
        // ESPAsyncWebServer Setup
        // Web Server Root URL
        server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/style.css", "text/css"); });
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/index.html", "text/html"); });

        server.on("/allvalues", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", processor()); });
        server.on("/logo", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/logo.png", "image/png"); });
        server.on("/pub", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
                    String input;
                    if(request->hasParam("cmd")){
                        input = request->getParam("cmd")->value();
                        // ¿Que hago con ese input?
                    } });
        server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
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
            if (request->hasParam("gprs"))
            {
            inputMessage = request->getParam("gprs")->value();
            inputParam = "gprs";
            config.gprs = inputMessage;
            }else{
            config.gprs = "off";
            }
            if (request->hasParam("wifi"))
            {
            inputMessage = request->getParam("wifi")->value();
            inputParam = "wifi";
            config.wifi = inputMessage;
            }else{
            config.wifi = "off";
            }
            if (request->hasParam("apn")){
                inputMessage = request->getParam("apn")->value();
                config.apn = inputMessage;
            }
            if(request->hasParam("host")){
                inputMessage = request->getParam("host")->value();
                mqttCredentials.mqttBroker = inputMessage;
                }
            if(request->hasParam("port")){
                inputMessage = request->getParam("port")->value();
                mqttCredentials.port = inputMessage.toInt();
                }
            if(request->hasParam("user")){
                inputMessage = request->getParam("user")->value();
                mqttCredentials.user = inputMessage;
                }
            if(request->hasParam("pass")){
                inputMessage = request->getParam("pass")->value();
                mqttCredentials.password = inputMessage;
                }

            currentState = START_STA;

            request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                            + inputParam + ") with value: " + inputMessage +
                                            "<br><a href=\"/\">Return to Home Page</a>"); 
            
            Serial.print("C\t" + config.ssid + "\t" + config.password + "\r\n");

            delay(5);
            
            String gprsState, wifiState; 
            
            if(config.gprs == "on"){
                gprsState = "1";
            }else{
                gprsState = "0";
            }
            if(config.wifi == "on"){
                wifiState = "1";
            }else{
                wifiState = "0";
            }
            Serial.print("H\t" + gprsState + "/" + wifiState + "\r\n");
            delay(5);

            Serial.print("APN\t" + config.apn + "\r\n"); });

        server.begin();
    }

    static void setupWifiSta()
    {
        if (config.ssid != "")
        {
            int lastTimeTimeOut = 0;
#ifdef DEBUG
            Serial.println(String(WiFi.getMode()));
#endif
            WiFi.mode(WIFI_STA);
            WiFi.begin(config.ssid.c_str(), config.password.c_str());
            delay(2000);

            while (WiFi.status() != WL_CONNECTED)
            {
#ifdef DEBUG
                Serial.print(".");
#endif
                delay(50);

                if (millis() - lastTimeTimeOut > 10000)
                {
#ifdef DEBUG
                    Serial.println("\nCould not connect to the WiFi network...\n");
#endif
                    break;

                    lastTimeTimeOut = millis();
                }
            }
            if (WiFi.status() == WL_CONNECTED)
            {
#ifdef DEBUG
                Serial.print("\nDevice Connected to the network: " + WiFi.localIP().toString() + " is the current ip\n");
#endif
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
    Serial.println("\n[*] Creating AP");
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_ip, gateway, subnet);
    WiFi.softAP(ssid, password);

    int numberOfWifi = WiFi.scanNetworks();

    for (int i = 1; i < numberOfWifi; i++)
    {
        if (i <= 9)
        {
            input.WiFiScan[i] = WiFi.SSID(i);
            Serial1.println(input.WiFiScan[i]);
            Serial1.println(WiFi.softAPIP());
        }
    }
}

String processor()
{
    String output = "";
    output += input.l1 + ";" + input.l2 + ";" + input.WiFiScan[0] + ";" + input.WiFiScan[1] + ";" + input.WiFiScan[2] + ";" + input.WiFiScan[3] + ";" + input.WiFiScan[4] + ";" + input.WiFiScan[5] + ";" + input.l3 + ";" + input.cantNodos + ";" + input.statusJSON;

    return output;
}