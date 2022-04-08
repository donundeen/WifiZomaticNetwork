/*
 * Board types
 * ESP32 Arduino -> Adafruit ESP32 Feather
 * ESP32 Adruino -> ESP32 Dev Module
 */

#include <ESP32Servo.h>


/*#include <Arduino.h>
#include <WiFi.h>
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
  "sprout",
  "cyberpoop",
  "barkcycle",
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

    setup_mqtt();

    setup_sensor();

  
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

  sensor_sub_callback(String(topic), messageTemp);
  
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection to "+String(host)+" as "+thishumanname+" ..." );
    // Attempt to connect
    client.setKeepAlive(60);        
    char humanname_c[thishumanname.length() + 1];
    thishumanname.toCharArray(humanname_c, thishumanname.length() + 1); 
    if (client.connect(humanname_c)) {
      Serial.println("connected");
      // Subscribe
      setup_subs();
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      fastblink(4);    
      delay(4000);
    }
  }
}

int count = 0;
void loop() {
  
    connect_wifi();
    if (!client.connected()) {
      reconnect();
    }    
    client.loop();

    loop_sensor();
}


void sendMessage(char* topic, int part1){
    connect_wifi();  
    Serial.println("sending " + String(topic) + ":"+String(part1) );
    char buf[5];
    sprintf(buf, "%04i", part1);
    client.publish(topic, buf);
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
int redServoPin = A1;
int blackServoPin = A0;
Servo redServo;  // create servo object to control a servo
Servo blackServo;  // create servo object to control a servo

int redFlexPin = A4;
int blackFlexPin = A3;
int redReading  = 3;      // the analog reading from the FSR resistor divider
int blackReading  = 3;      // the analog reading from the FSR resistor divider
int redTrigger = 900;
int blackTrigger = 200;
int redTriggered = false;
int blackTriggered = false;


int redStopSpeed = 93;
int blackStopSpeed = 92;
int stopInc = 0;
int slowRedWalk = 8;
int fastRedWalk = 10;
int slowBlackWalk = -6;
int fastBlackWalk = -10;

int poopSpeedPlus = 0;

String dir = "red";


void pre_setup_sensor(){
  // this runs BEFORE the regular setup.

}

void setup_subs(){
  Serial.println("subscribing ..." );
    client.subscribe("/poop",1);
    client.subscribe("/danger",1);
  Serial.println("subscribed!" );
}


void sensor_sub_callback(String topic, String message){
  if (topic == "/poop") {
    onPoopMessageReceived(message);
  }
  else if(topic == "/danger"){
    onDangerMessageReceived(message);
  }
}

void setup_sensor(){
    Serial.println("setup_sensor");
    

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

  stopWalk(); // begin in stopped mode
  
}



void loop_sensor(){
  //Serial.println("loop_sensor");
  //calibrate_servo();
  read_flex();
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

int selfDangers = 0;
void read_flex(){
  redReading = analogRead(redFlexPin);

  Serial.print("Red Analog reading = ");
  Serial.println(redReading);

  blackReading = analogRead(blackFlexPin);
  Serial.print("Black Analog reading = ");
  Serial.println(blackReading);

  if(!redTriggered && redReading < redTrigger){
    redTriggered = true;
    selfDangers++;
    sendMessage("/danger", 1);
    moveBlack(); // move towards black
  }
  if(!blackTriggered && blackReading < blackTrigger){
    blackTriggered = true;
    selfDangers++;
    sendMessage("/danger", 1);    
    moveRed(); // move towards red
  }
  if(blackTriggered && blackReading > blackTrigger){
    blackTriggered = false;
  }
  if(redTriggered && redReading > redTrigger){
    redTriggered = false;
  }

}

void onPoopMessageReceived(String message) {
  Serial.println("poop message received!");
  // increase poop value
  poopSpeedPlus = constrain(poopSpeedPlus + 2, 0, 12);
}

void onDangerMessageReceived(String message) {
  // danger message received, reverse direction;
  if(selfDangers > 0){ // we probably sent this message
    selfDangers--;
    return;
  }
  Serial.println("got danger message!");

  // reduce poop value:
  poopSpeedPlus = constrain(poopSpeedPlus - 1, 0, 12);

  if(dir == "red"){
    moveBlack();
  }else if (dir == "black"){
    moveRed();
  }
}

void onWaterMessageReceived(String message) {
// nothing planned here yet
/*
Value out of soil: 4095
Value in moist soil: ~2000, each reading +/-50 (range of 100)
drier soil: ~3000
 */  
}


void moveRed(){
  dir = "red";
  Serial.println("moving red");
  // turn wheels towards Red sensor
  int extraSpeed= poopSpeedPlus;
  if(slowRedWalk < 0){
    extraSpeed = extraSpeed * -1;
  }
  int redSpeed = redStopSpeed + slowRedWalk + extraSpeed;
  int blackSpeed = blackStopSpeed + slowRedWalk + extraSpeed;
  Serial.println("slow speeds: " + String(redSpeed) + ": " + String(blackSpeed));
  redServo.write(redSpeed);
  blackServo.write(blackSpeed); 
}

void moveBlack(){
  dir = "black";
  Serial.println("moving black");
  int extraSpeed= poopSpeedPlus;
  if(slowBlackWalk < 0){
    extraSpeed = extraSpeed * -1;
  }
    Serial.println(extraSpeed);

  // turn wheels towards Black sensor
  int redSpeed = redStopSpeed + slowBlackWalk + extraSpeed;
  int blackSpeed = blackStopSpeed + slowBlackWalk + extraSpeed;
  Serial.println("slow black speeds: " + String(redSpeed) + ": " + String(blackSpeed));
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
