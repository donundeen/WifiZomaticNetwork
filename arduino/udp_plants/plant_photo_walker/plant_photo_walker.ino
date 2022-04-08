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

    float batteryLevel = (analogRead(A13) / 4095.0) * (2.0 * 3.3 * 1.1);
    Serial.println("-+-+-+-+-+-+- battery level is " + String (batteryLevel));

   
    
    pre_setup_sensor();

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


/*
 * A4/36 ( 8 up from bottom on long side) - this is an analog input A4 and also GPI #36. Note it is _not_ an output-capable pin! It uses ADC #1
*  A3/39 (9 up from bottom on long side)- this is an analog input A3 and also GPI #39. Note it is _not_ an output-capable pin! It uses ADC #1 
 * 
 */
int fsrAnalogPin = A4;
int fsrAnalogPin2 = A3;
int fsrReading  = 3;      // the analog reading from the FSR resistor divider
int fsrReading2  = 3;      // the analog reading from the FSR resistor divider


// this plant can move a branch with a directed light detector. 
// in "search" mode it moves around until it finds the max light level, then points at the max light
// when it's in alert mode, if it detects a change in light, it switches to "alert" mode and sends an OSC 'danger' message
// when it RECEIVES a 'danger' message, it goes into 'search' mode.
// IOW, it doesn't respond to its OWN danger messages.
String mode = "wait"; // 'search' or 'alerting' or 'wait'

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
int redServoPin = A0;
int blackServoPin = A1;
Servo redServo;  // create servo object to control a servo
Servo blackServo;  // create servo object to control a servo
int redStopSpeed = 97;
int blackStopSpeed = 95;
int redFlexPin = A3;
int blackFlexPin = A4;
int redReading  = 3;      // the analog reading from the FSR resistor divider
int blackReading  = 3;      // the analog reading from the FSR resistor divider
int redTrigger = 900;
int blackTrigger = 200;
int redTriggered = false;
int blackTriggered = false;

void pre_setup_sensor(){
  // this runs BEFORE the regular setup.

}

void setup_sensor(){
    Serial.println("setup_sensor");
    OscWiFi.subscribe(recv_port, "/danger", onDangerMessageReceived);
    Serial.println("subscribed");

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
  redServo.attach(redServoPin);  // attaches the servo on pin servoPin to the servo object
  blackServo.attach(blackServoPin);  // attaches the servo on pin servoPin2 to the servo object
 // mode = "search";
  // set servo stop spee depedning on servo/device
  /*
  if( thisarduinomac == "40:F5:20:45:D5:14"){
    stopSpeed=92;
    Serial.println(stopSpeed);
  }
  if( thisarduinomac == "C4:DD:57:9C:DC:A4"){
    stopSpeed=90;
    Serial.println(stopSpeed);
  }
  */
//  test_walk();
//  test_move();
//  seek_light();
  moveRed(); // start in one direction
  
}



void loop_sensor(){
  //Serial.println("loop_sensor");
  //calibrate_servo();
  read_flex();
  /*
  if(mode == "search"){
    seek_light();
  }else if (mode == "wait"){
    myservo.write(stopSpeed);  // stop motor
    OscWiFi.update();  // should be called to receive + send osc
    detect_danger();
  }
  */
  delay(100);
  
}

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


void read_flex(){
  redReading = analogRead(redFlexPin);
  Serial.print("Red Analog reading = ");
  Serial.println(redReading);

  blackReading = analogRead(blackFlexPin);
  Serial.print("Black Analog reading = ");
  Serial.println(blackReading);

  if(!redTriggered && redReading < redTrigger){
    redTriggered = true;
    sendToAll("/danger", 1);
    moveBlack(); // move towards black
  }
  if(!blackTriggered && blackReading < blackTrigger){
    blackTriggered = true;
    sendToAll("/danger", 1);    
    moveRed(); // move towards red
  }
  if(blackTriggered && blackReading > blackTrigger){
    redTriggered = false;
  }
  if(blackTriggered && blackReading > blackTrigger){
    redTriggered = false;
  }

}

/*
int cwmove = stopSpeed - 10;
int ccwmove = stopSpeed + 10;
int dir = cwmove; // initial direction

// this function is where the servo seeks a light source
int cwmove_count = 0;
int ccwmove_count = 0;
*/


void onDangerMessageReceived(const OscMessage& m) {
  // danger message received, go into search mode;
  Serial.println("got danger message!");
  
}



int stopInc = 0;
int slowRedWalk = 3;
int fastRedWalk = 8;
int slowBlackWalk = -8;
int fastBlackWalk = -10;


void moveRed(){
  Serial.println("moving red");
  // turn wheels towards Red sensor
  int redSpeed = redStopSpeed+slowRedWalk;
  int blackSpeed = blackStopSpeed+slowRedWalk;
  Serial.println("slow speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed); 
}

void moveBlack(){
  Serial.println("moving black");
  // turn wheels towards Black sensor
  int redSpeed = redStopSpeed+slowBlackWalk;
  int blackSpeed = blackStopSpeed+slowBlackWalk;
  Serial.println("slow back speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed); 
}

void stopWalk(){
   int redSpeed = redStopSpeed; // stop?
  int blackSpeed = blackStopSpeed;
  Serial.println("stop");
  Serial.println("speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed);
}

void test_walk(){
  Serial.println("test_move");
  int pos = 90;
  int MIN_SERVO_VALUE = 0;
  int MAX_SERVO_VALUE = 180;

  int redSpeed = redStopSpeed; // stop?
  int blackSpeed = blackStopSpeed;
  Serial.println("stop");
  Serial.println("speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed);
  delay(3000);

  redSpeed = redStopSpeed+slowRedWalk;
  blackSpeed = blackStopSpeed+slowRedWalk;
  Serial.println("slow speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed);
  delay(3000);
  
  redSpeed = redStopSpeed+fastRedWalk;
  blackSpeed = blackStopSpeed+fastRedWalk;
  Serial.println("fast speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed);
  delay(3000);

  redSpeed = redStopSpeed; // stop?
  blackSpeed = blackStopSpeed;
  Serial.println("stop");
  Serial.println("speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed);
  delay(3000);

  redSpeed = redStopSpeed+slowBlackWalk;
  blackSpeed = blackStopSpeed+slowBlackWalk;
  Serial.println("slow back speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed);
  delay(3000);

  redSpeed = redStopSpeed+fastBlackWalk;
  blackSpeed = blackStopSpeed+fastBlackWalk;
  Serial.println("fast back speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed);
  delay(3000);

  Serial.println("done test_move");

}
