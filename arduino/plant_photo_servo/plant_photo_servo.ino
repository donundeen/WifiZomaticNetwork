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
#include <ESP32Servo.h>



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

    setup_sensor();

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

    loop_sensor();
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

int fsrAnalogPin = A4;
int fsrReading  = 3;      // the analog reading from the FSR resistor divider


// this plant can move a branch with a directed light detector. 
// in "search" mode it moves around until it finds the max light level, then points at the max light
// when it's in alert mode, if it detects a change in light, it switches to "alert" mode and sends an OSC 'danger' message
// when it RECEIVES a 'danger' message, it goes into 'search' mode.
// IOW, it doesn't respond to its OWN danger messages.
String mode = "wait"; // 'search' or 'alert' or 'wait'

/*
 * https://docs.arduino.cc/learn/electronics/servo-motors
 * Brown wire (black ) to ground 
 * middle wire (orange) to Power (3v on arduino?)
 * Yellow wire to Analog Out Pin: A0 - (5 down on long side) this is an analog input A0 and also an analog output DAC2. It can also be used as a GPIO #26. It uses ADC #2

 * 
 * * **A0** - (5 down on long side) this is an analog input A0 and also an analog output DAC2. It can also be used as a GPIO #26. It uses ADC #2
* **A1** - (6 down on long side) this is an analog input A1 and also an analog output DAC1. It can also be used as a GPIO #25. It uses ADC #2
* **3v** - 2 down on long side
* **GnD** - 4 down on long side
* Positions: for myservo.write(val);
* "90" (1.5ms pulse) is stop, 
* "180" (2ms pulse) is full speed forward, counterclockwise
* "0" (1ms pulse) clockwise
 * 
 * testing:
 * 80 is clockwise
 * 90 is slower clockwise
 * 
 */
int servoPin = A0;
Servo myservo;  // create servo object to control a servo
int stopSpeed = 95;

void setup_sensor(){
/* A4 / 36 ( 8 up from bottom on long side) - 
 *  this is an analog input A4 and also GPI #36. 
 *  Note it is _not_ an output-capable pin! It uses ADC #1

// 3V is 2nd down from top on long 
// gnd is 4 down on long side
*/
/*
 * Connect one end of photoresistor  to 5V, the other end to Analog 4 (gpio36).
Then connect one end of a 10K resistor from Analog 4 to ground
 */
   myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object

}

void loop_sensor(){
  Serial.println(fsrAnalogPin);
  fsrReading = analogRead(fsrAnalogPin);
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);
  move_servo();
}


void move_servo(){
 int speedDir = stopSpeed; // stop?
  Serial.println(speedDir);
  myservo.write(speedDir); 
  delay(1000); 
  speedDir = 0;
  Serial.println(speedDir);
  myservo.write(speedDir); 
  delay(1000); 
  speedDir = stopSpeed;
  Serial.println(speedDir);
  myservo.write(speedDir); 
  delay(1000); 
  speedDir = 180;
  Serial.println(speedDir);
  myservo.write(speedDir); 
  delay(1000); 
  speedDir = stopSpeed;
  Serial.println(speedDir);
  myservo.write(speedDir); 
  delay(1000); 
  
}

int read_light(){
  int lightvalue = analogRead(fsrAnalogPin);
  Serial.print("Analog reading = ");
  Serial.println(fsrReading);
  return lightvalue;
}
