const char* ssid     = "Darkflow-Lora";
const char* password = NULL;

IPAddress local_ip(192,168,0,1);
IPAddress gateway(192,168,0,1);
IPAddress subnet(255,255,255,0);

struct 
{
    String WiFiScan[9];
    String id_ = "default";
    String nodos = "default";
}input;

struct
{
    String ssid;
    String password;
    String gprs = "off";
    String wifi = "off";
}config;

