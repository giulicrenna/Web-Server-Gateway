#include <WiFi.h>
#include <LittleFS.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Preferences.h>
// #include "global.hpp"

Preferences myData;
AsyncWebServer server(80);

void beginEEPROM();
void setupWifiSta();
bool loadData();
String processor();

class WiFiSetter
{
protected:
public:
    void setupApMode()
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
        // ESPAsyncWebServer Setup
        // Web Server Root URL
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/index.html", "text/html"); });

        server.on("/allvalues", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", processor()); });
        server.on("/logo", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/www/logo.png", "image/png"); });
        server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request)
                  {
            String inputMessage, inputParam;
            if (request->hasParam("ssid"))
            {
            inputMessage = request->getParam("ssid")->value();
            inputParam = "ssid";
            config.ssid = inputMessage;
            myData.putString("ssid", inputMessage);
            }
            if (request->hasParam("pass"))
            {
            inputMessage = request->getParam("pass")->value();
            inputParam = "pass";
            config.password = inputMessage;
            myData.putString("pass", inputMessage);
            }
            if (request->hasParam("gprs"))
            {
            inputMessage = request->getParam("gprs")->value();
            inputParam = "gprs";
            config.gprs = inputMessage;
            myData.putString("gprs", inputMessage);
            }else{
            config.gprs = "off";
            myData.putString("gprs", "off");
            }
            if (request->hasParam("wifi"))
            {
            inputMessage = request->getParam("wifi")->value();
            inputParam = "wifi";
            config.wifi = inputMessage;
            myData.putString("wifi", inputMessage);
            }else{
            config.wifi = "off";
            myData.putString("wifi", "off");
            }

            currentState = START_STA;

            request->send(200, "text/html", "HTTP GET request sent to your ESP on input field (" 
                                            + inputParam + ") with value: " + inputMessage +
                                            "<br><a href=\"/\">Return to Home Page</a>"); });
        server.begin();
    }
    void setupLocalServer(){
        server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(LittleFS, "/wwwlocal/index.html", "text/html"); });

        server.on("/allvalues", HTTP_GET, [](AsyncWebServerRequest *request)
                  { request->send(200, "application/json", processor()); });

        //server.on("/logo", HTTP_GET, [](AsyncWebServerRequest *request)
        //          { request->send(LittleFS, "/www/logo.png", "image/png"); });
    
        server.begin();
    }
};

void setupWifiSta()
{
    WiFi.mode(WIFI_STA);
    WiFi.begin(config.ssid.c_str(), config.password.c_str());
    delay(2500);
    if (WiFi.isConnected())
    {
        Serial.println("Device Connected to the network: " + WiFi.localIP().toString() + " is the current ip\n");
        //currentState = START_INTERPRETATOR_LOCAL_SERVER;
    }
}

String processor()
{
    String output = "";
    output += input.l1 + ";" + input.l2 + ";" + input.WiFiScan[0] + ";" + input.WiFiScan[1] + ";" + input.WiFiScan[2] + ";" + input.WiFiScan[3] + ";" + input.WiFiScan[4] + ";" + input.WiFiScan[5] + ";" + input.WiFiScan[6] + ";" + input.WiFiScan[7] + ";" + input.WiFiScan[8] + ";" + input.WiFiScan[9];

    return output;
}

bool loadData()
{
    config.gprs = myData.getString("gprs", "None");
    config.wifi = myData.getString("wifi", "None");
    config.password = myData.getString("pass", "off");
    config.ssid = myData.getString("ssid", "off");

    return true;
}

bool putData(){
    myData.putString("gprs", config.gprs);
    myData.putString("wifi", config.wifi);
    myData.putString("pass", config.password);
    myData.putString("ssid", config.ssid);

    return true;
}

void beginEEPROM()
{
    myData.begin("credentials", false);
    if (loadData())
    {
        Serial.println("Data loaded succesfully");
    }
    if (LittleFS.begin())
    {
        Serial.println("Filesystem mounted");
    }
}