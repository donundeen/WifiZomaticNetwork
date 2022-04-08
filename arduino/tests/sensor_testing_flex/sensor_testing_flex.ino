/*
 * Board types
 * ESP32 Arduino -> Adafruit ESP32 Feather
 * ESP32 Adruino -> ESP32 Dev Module
 */

/*#include <Arduino.h>
#include <WiFi.h>
*/
#include <ArduinoOSCWiFi.h>
// #include <ArduinoOSC.h> // you can use this if your borad supports only WiFi or Ethernet



// WiFi stuff
//const char* ssid = "JJandJsKewlPad";
//const char* pwd = "WeL0veLettuce";
//const char* host = "10.0.0.162"; // dev laptop

const char* ssid = "log.local";
const char* pwd = "";
//const char* host = "10.0.0.74"; // rpi when on log.local
const char* host = "10.0.0.225"; // what you're sending messages TO

//const IPAddress ip(10, 0, 0, 225);
IPAddress ip;  // THIS device's IP  (need to tie to consistent mac addresses, with a table)
const IPAddress gateway(10, 0, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

int i; float f; String s;

int port = 9002;

int publish_port= port;
int bind_port = port;


// for ArduinoOSC
const int recv_port = port;
const int send_port = port;
// send / receive variables

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
  226, // swinging tree
  227,
  228, // cyberpoop
  229,
  230,
  74,
  203,
};

int numplants = 9;

String humannames[] = { 
  "stick",
  "pinecone",
  "swingingtree",
  "branch",
  "cyberpoop",
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


void setup() {

    Serial.begin(115200);
    
    pre_setup_sensor();

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
 
    connect_wifi();

    setup_sensor();


    // publish osc messages (default publish rate = 30 [Hz])
  // this listens for messages, sends results to onPlantMessageReceived function
  //  OscWiFi.subscribe(recv_port, "/plantmessage", onPlantMessageReceived);
  
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


int count = 0;
void loop() {
    connect_wifi();
    OscWiFi.update();  // should be called to receive + send osc
    loop_sensor();
}

void sendToAll(String channel, int message){
  Serial.print("size is " );
  Serial.println(sizeof(arduinoips));
  for (int i = 0; i< numplants; i++){
    int rec_ip = arduinoips[i];
    Serial.print("rec_ip is " );
    Serial.println(rec_ip);
    if(rec_ip != thisarduinoip){
        String fullip = ipprefix+String(rec_ip);
        Serial.print("fullip is " );
        Serial.println(fullip);        
        sendMessage(fullip, channel, message);
    }
  }
}

void sendMessage(String host, String channel, int part1){
    connect_wifi();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("I'm not connected!");
    }else{
      Serial.println("I AM connected!");
    }
    Serial.println("sending " + host + channel + ":"+publish_port);
    OscWiFi.send(host, publish_port, channel, part1); // to publish osc  
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

int redFlexPin = A3;
int blackFlexPin = A4;
int redReading  = 3;      // the analog reading from the FSR resistor divider
int blackReading  = 3;      // the analog reading from the FSR resistor divider

void pre_setup_sensor(){

  
}

void setup_sensor(){

  // check battery level
  
/* A4 / 36 ( 8 up from bottom on long side) - 
 *  this is an analog input A4 and also GPI #36. 
 *  Note it is _not_ an output-capable pin! It uses ADC #1
A3 : 9 up from bottom on long side.

// 3V is 2nd down from top on long 
// gnd is 4 down on long side
*/
/*
 * Connect one end of Flex  to 5V, the other end to Analog 4.
Then connect one end of a 10K resistor from Analog 4 to ground

red flex: A3
range when unflexed is around 1085 to 1157
heavily flexed is around 800
so trigger at 900

black flex: A4
range when unflexed is around 291 to 316
heavily flexed is around 60
so trigger at 200


 */
}



int minFlex = 10000;
int maxFlex = 0;
void loop_sensor(){
  Serial.println(redFlexPin);
  redReading = analogRead(redFlexPin);
  if(redReading > maxFlex){
    maxFlex = redReading;
  }
  if(redReading < minFlex){
    minFlex = redReading;
  }
  Serial.print("Red Analog reading = ");
  Serial.println(redReading);
  Serial.println(String(minFlex)+":"+String(maxFlex));

  Serial.println(blackFlexPin);
  blackReading = analogRead(blackFlexPin);
  Serial.print("Black Analog reading = ");
  Serial.println(blackReading);

  delay(500);

}
