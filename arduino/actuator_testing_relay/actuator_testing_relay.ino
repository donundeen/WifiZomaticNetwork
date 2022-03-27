/*
 * Board types
 * ESP32 Arduino -> Adafruit ESP32 Feather
 * ESP32 Adruino -> ESP32 Dev Module
 */

//#include "Grove_Motor_Driver_TB6612FNG.h"
//#include <Wire.h>
 

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
int publish_port= 9002;
int bind_port = 9003;


// for ArduinoOSC
const int recv_port = 9003;
const int send_port = 55555;
// send / receive varibales

String arduinomacs[]= { 
"40:F5:20:44:B1:3C",
"40:F5:20:45:D5:14",
"40:F5:20:45:D0:18",
"C4:DD:57:9C:DC:A4",
"40:F5:20:45:D0:A4",
"30:AE:A4:9D:7C:44",
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
  74,
  203,
};

int numplants = 7;

String humannames[] = { 
  "stick",
  "pinecone",
  "dirt",
  "branch",
  "seedling",
  "devmodule",
  "mothertree",
  "accesspoint"
};

String ipprefix  = "10.0.0.";
String thisarduinomac = "";
String thishumanname = "";
int thisarduinoip = 0;
int sendcount = 0;

void onPlantMessageReceived(const OscMessage& m) {
  Serial.println("message received");
    fastblink(5);
    Serial.print(m.remoteIP());
    Serial.print(" ");
    Serial.print(m.remotePort());
    Serial.print(" ");
    Serial.print(m.size());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    // be mindful of the number of arguments to expect, and their type
    Serial.print(m.arg<int>(0));
    Serial.print(" ");
    Serial.print(m.arg<int>(1));
    /*
    Serial.print(" ");
    Serial.print(m.arg<String>(2));
    */
    Serial.println();

    sendToAll("/plantmessage", sendcount);
    sendcount++;  
}

void setup() {

    Serial.begin(115200);

    setup_actuator();

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

    // publish osc messages (default publish rate = 30 [Hz])



  // this listens for messages, sends results to onPlantMessageReceived function
    OscWiFi.subscribe(recv_port, "/plantmessage", onPlantMessageReceived);

  
}

int count = 0;
void loop() {

    loop_actuator();
  /*
    OscWiFi.update();  // should be called to receive + send osc
*/
  
    /*
    // just send message 5 times, for testing
    if(sendcount <= 5 || random(100) < 5){
//      sendPlantMessage(host, count, 456);
      sendToAll("/plantmessage", sendcount);
      sendcount++;
//      OscWiFi.send(host, publish_port, "/plantmessage", count, 456); // to publish osc
      delay(500);
    }
*/

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
        sendPlantMessage(fullip, message, 345);
    }
  }
}


void sendPlantMessage(String host, int part1, int part2){
  Serial.print("sending message to " );
  Serial.println(host);  
  OscWiFi.send(host, publish_port, "/plantmessage", part1, part2); // to publish osc  
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


/*

 * 
 */

int triggerPin =  A0 ; //(GPIO 26)

void setup_actuator(){
    // join I2C bus (I2Cdev library doesn't do this automatically)
 //   Wire.begin();
//    motor.init();
    pinMode(triggerPin, OUTPUT);


}

void loop_actuator(){
    // drive 2 dc motors at speed=255, clockwise
    Serial.println("off");
    
    digitalWrite(triggerPin, LOW);
    delay(50);
    Serial.println("on");

    digitalWrite(triggerPin, HIGH);
    delay(50);

}
