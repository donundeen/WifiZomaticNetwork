/*
 * Board types
 * ESP32 Arduino -> Adafruit ESP32 Feather
 * ESP32 Adruino -> ESP32 Dev Module
 */

/*#include <Arduino.h>
*/
#include <WiFi.h>
#include <PubSubClient.h>
// #include <ArduinoOSC.h> // you can use this if your borad supports only WiFi or Ethernet

WiFiClient espClient;
PubSubClient client(espClient);

// WiFi stuff
//const char* ssid = "JJandJsKewlPad";
//const char* pwd = "WeL0veLettuce";
//const char* host = "10.0.0.162"; // dev laptop

const char* ssid = "log.local";
const char* pwd = "";
//const char* host = "10.0.0.74"; // rpi when on log.local
const char* host = "10.0.0.203"; // what you're sending messages TO

//const IPAddress ip(10, 0, 0, 225);
IPAddress ip;  // THIS device's IP  (need to tie to consistent mac addresses, with a table)
const IPAddress gateway(10, 0, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

int port = 1883;





String arduinomacs[]= { 
"40:F5:20:44:B1:3C",
"40:F5:20:45:D5:14",
"40:F5:20:45:D0:18",
"C4:DD:57:9C:DC:A4",
"40:F5:20:45:D0:A4",
"C4:DD:57:9C:CA:20",
"3C:61:05:4A:5F:C0",
"rpi:mac:address",
"ap:mac:address",
};

int arduinoips[] = {
  224,
  225,
  226,
  227,
  228,
  229,
  230,
  74,
  203,
};

int numplants = 8;

String humannames[] = { 
  "stick",
  "pinecone",
  "dirt",
  "branch",
  "seedling",
  "leaf",
  "root",
  "mothertree",
  "accesspoint"
};
String ipprefix  = "10.0.0.";
String thisarduinomac = "";
String thishumanname = "";
int thisarduinoip = 0;
int sendcount = 0;

long lastMsg = 0;
char msg[50];
int value = 0;

void setup() {

    Serial.begin(115200);

    float batteryLevel = (analogRead(A13) / 4095.0) * (2.0 * 3.3 * 1.1);
    Serial.println("-+-+-+-+-+-+- battery level is " + String (batteryLevel));

    Serial.println(LED_BUILTIN);
    Serial.println(A8);
    pinMode(LED_BUILTIN, OUTPUT);
      
    delay(2000);

    fastblink(3);
    
    thisarduinomac = WiFi.macAddress();
    Serial.print("this mac address is ");
    Serial.println(thisarduinomac);
    resolveids();


    // WiFi stuff (no timeout setting for WiFi)
    Serial.print("connecting to SSID ");
    Serial.println(ssid);
  
//    client.setClient(WiFi);
   
    connect_wifi();

    setup_mqtt();
    
  
}


void setup_mqtt(){
  client.setServer(host, port);
  client.setCallback(callback);
}

void connect_wifi(){
   if(WiFi.status() != WL_CONNECTED){
      Serial.println("connecting to wifi");
#ifdef ESP_PLATFORM
      WiFi.disconnect(true, true);  // disable wifi, erase ap info
      delay(1000);
      WiFi.mode(WIFI_STA);
#endif
  
      WiFi.begin(ssid, pwd);
      WiFi.config(ip, gateway, subnet);
      
      while (WiFi.status() != WL_CONNECTED) {
          Serial.print(".");
          fastblink(2);
  //        delay(500);
      }
      
      Serial.print("WiFi connected, IP = ");
      Serial.println(WiFi.localIP());
   }
  
}

// beware of returning messages that were just sent.
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
    }
    else if(messageTemp == "off"){
      Serial.println("off");
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    client.setKeepAlive(60);        
    char humanname_c[thishumanname.length() + 1];
    thishumanname.toCharArray(humanname_c, thishumanname.length() + 1); 
    if (client.connect(humanname_c)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("Test");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  long now = millis();
  if (now - lastMsg > 5000) {
    Serial.println("sending");
    lastMsg = now;
    char buf[4];
    int humString  = random(0,1000);
    sprintf(buf, "%03i", humString);
    client.publish("Test", buf);
  }
}


void fastblink(int times){
  for(int i = 0; i< times; i++){
    
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(250);                       // wait for a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(250);  
  }
}


void resolveids(){
  int foundindex = -1;
  Serial.print("arudinomacs size is ");
  Serial.println(sizeof(arduinomacs));
  for (int index = 0; index < numplants; index++) {
    if(thisarduinomac == arduinomacs[index]){
      foundindex = index;
      break;
    }
  }
  if(foundindex >= 0){
    thisarduinoip = arduinoips[foundindex];
    thishumanname = humannames[foundindex];
  }
  ip = IPAddress(10, 0, 0, thisarduinoip);
  Serial.print("arduinomac : " );
  Serial.println(thisarduinomac);
  Serial.print("ip : ");
  Serial.println(thisarduinoip);
  Serial.print("human name : " );
  Serial.println(thishumanname);
}
