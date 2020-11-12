# eps8266-can-t-upload-certificate

//-----webserver define-----//
#include "FS.h"
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
const char* ssid = "MANIK";
const char* password = "manik123";


//-----webserver define end-----//

//-----pzem define-----//
#include <PZEM004Tv30.h>
PZEM004Tv30 pzem(2, 0);
//-----pzem define end-----//

//-----AWS define-----//

#include <PubSubClient.h>

const char* AWS_endpoint = "xxxxx-ats.iot.ap-southeast-1.amazonaws.com"; //MQTT broker ip
//-----AWS define end-----//
void callback(char* topic, byte* payload, unsigned int length) {
Serial.print("Message arrived [");
Serial.print(topic);
Serial.print("] ");
for (int i = 0; i < length; i++) {
Serial.print((char)payload[i]);
}
Serial.println();

}

WiFiClientSecure espClient;
PubSubClient client(AWS_endpoint, 8883,espClient); //set MQTT port number to 8883 as per //standard
long lastMsg = 0;
char msg[90];
int value = 0;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);



 espClient.setBufferSizes(512, 512);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

//Initialize File system
if(SPIFFS.begin())
{
Serial.println("SPIFFS got Initialized successfully");
}
else
{
Serial.println("SPIFFS Initialization is failed");
}
//Format File system
if(SPIFFS.format())
{
Serial.println("File system Formatted");
}
else
{
Serial.println("File system formatting error");
}

  // Load certificate file
  File cert = SPIFFS.open("cert.der", "r"); //replace cert.crt eith your uploaded file name
  if (!cert) {
   Serial.println("Failed to open cert file");
  }
  else
  Serial.println("Success to open cert file");

  delay(1000);

  if (espClient.loadCertificate(cert))
  Serial.println("cert loaded");
  else
  Serial.println("cert not loaded");

  // Load private key file
  File private_key = SPIFFS.open("/private.cer", "r"); //replace private eith your uploaded file name
  if (!private_key) {
  Serial.println("Failed to open private  file");
  }
  else
  Serial.println("Success to open private  file");

  delay(1000);

  if (espClient.loadPrivateKey(private_key))
  Serial.println("private key loaded");
  else
  Serial.println("private key not loaded");

  // Load CA file
  File ca = SPIFFS.open("/CA.der", "r"); //replace ca eith your uploaded file name
  if (!ca) {
  Serial.println("Failed to open ca ");
  }
  else
  Serial.println("Success to open ca");

  delay(1000);

  if(espClient.loadCACert(ca))
  Serial.println("ca loaded");
  else
  Serial.println("ca not loaded");

  Serial.print("Heap: "); Serial.println(ESP.getFreeHeap());
}

void loop() {
          
  //-----pzem-----//
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();
    if(voltage == NAN || current == NAN || voltage == NAN || power == NAN || energy == NAN || frequency == NAN || pf == NAN){ Serial.println("Reading failed.");}
    else{
    //publishing data
    snprintf (msg, 75, "{\"Voltage\": #%ld\"}", voltage);
    snprintf (msg, 75, "{\"Current\": #%ld\"}", current);
    snprintf (msg, 75, "{\"Power\": #%ld\"}", power);
    snprintf (msg, 75, "{\"Energy\": #%ld\"}", energy);
    snprintf (msg, 75, "{\"Frequency\": #%ld\"}", frequency);
    snprintf (msg, 75, "{\"PF\": #%ld\"}", pf);
    client.publish("outTopic", msg);
    Serial.println("Publishing:- ");
    Serial.print("Voltage: "); Serial.print(voltage); Serial.println("V");
    Serial.print("Current: "); Serial.print(current); Serial.println("A");
    Serial.print("Power: "); Serial.print(power); Serial.println("W");
    Serial.print("Energy: "); Serial.print(energy, 3); Serial.println("kWh");
    Serial.print("Frequency: "); Serial.print(frequency, 1); Serial.println("Hz");
    Serial.print("PF: "); Serial.println(pf);
     
      Serial.println("Success\n");
    }
   delay(10);
  }
