#define DEBUG
#define isESP8266

const char *ssid = "Darkflow-Lora";
const char *password = NULL;

int baudrate = 115200;
int lastTimeCleanData_l4 = 0;
int lastTimeRead = 0;
int mQueueSize = 5;

typedef enum
{
    NONE,
    START_STA,
    START_AP,
    WAIT_FOR_CONNECTION,
    START_INTERPRETATOR_LOCAL_SERVER,
    START_INTERPRETATOR
} States;

States currentState = START_AP;

mStructures::mQueue myMessages_(mQueueSize);
mStructures::mQueue topics_(mQueueSize);
mStructures::mQueue sendings(mQueueSize);
std::vector<std::string> messages;
std::vector<std::string> topics;

IPAddress local_ip(192, 168, 0, 1);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);

struct
{
    String WiFiScan[9];
    String l1 = "default";
    String l2 = "default";
    String l3 = "default";
    String l4 = "default";
    String nodes = "default";
    String cantNodos = "0";
    String statusJSON = "{}";
} input;

struct
{
    String ssid = "";
    String password = "";
    String gprs = "off";
    String wifi = "off";
    String apn = "";
} config;

struct
{
    String mqttBroker = "";
    String clientID = "";
    String subsTopic = "testTopicAvoid/";
    String pubTopic = "";
    String user = "";
    String password = "";
    int port = 1883;
} mqttCredentials;