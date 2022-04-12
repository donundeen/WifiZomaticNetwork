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
  "water",
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

    if (!client.connected()) {
      reconnect();
    } 

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
  sensor_sub_callback(String(topic), messageTemp);
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



// this plant can move a branch up as if it's sprouting. maybe I'll attach an orchid..
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
int servoPin = A0;
Servo myservo;  // create servo object to control a servo

int waterLevel = 4500;
int poopLevel = 0 ;


void pre_setup_sensor(){
  // this runs BEFORE the regular setup.
  myservo.attach(servoPin);  // attaches the servo on pin 9 to the servo object

}


void setup_subs(){
    client.subscribe("/poop",1);
    client.subscribe("/danger",1);
    client.subscribe("/water",1);
}


void sensor_sub_callback(String topic, String message){
  if (topic == "/poop") {
    onPoopMessageReceived(message);
  }
  else if(topic == "/danger"){
    onDangerMessageReceived(message);
  }
  else if(topic == "/water"){
    onWaterMessageReceived(message);
  }
}


int triggerPin1 =  A0 ; //(GPIO 26) // 5 down on long side
int val1 = LOW;

void setup_sensor(){
    pinMode(triggerPin1, OUTPUT);

}

void loop_sensor(){
  //Serial.println("loop_sensor");
  //calibrate_servo();

  /*
   * // testing it
  rustle_leaves();
  poopLevel++;
  waterLevel = waterLevel - 250;
  delay(5000);
  */
}

// this servo has absolute positioning, just set the number 0-180


// the higher the threshold value, the MORE flipping that should happen
void randflip(int threshold){
  //Serial.println("randflip");
  int rand1 = random(1000);
  //Serial.println(rand1);
 
  if(rand1 < threshold){
  //  Serial.println(flipcount++);
    val1 = (val1 == LOW ? HIGH : LOW);
  //  Serial.print("val1 ");
   // Serial.println(val1);
    digitalWrite(triggerPin1, val1);    
  }
}


void rustle_leaves(){
  /*
   * Variables:
   * poopLevel
   * waterLevel
   * 
   * Things to mess with
   * speed of loop
   * threshold (likelihood of a switch)
   * duration of the rustling period
   * 
   * more poopLevel = more active rustling
   * more waterLevel = longer duration of rustling
   */

   /* Water
Value out of soil: 4095
Value in moist soil: ~2000, each reading +/-50 (range of 100)
drier soil: ~3000
 */ 
  int durationScale = constrain(map(waterLevel, 3000, 1500, 1000, 5000), 1000, 5000);
  float rateScale = constrain(map(poopLevel, 0.0, 20.0, 0, 1000), 0.0, 1000) / 1000.0; 
  Serial.println("water: "+String(waterLevel)+" -> duration " + String(durationScale));
  Serial.println("poop: "+ String(poopLevel)+" -> rateScale " +  String(rateScale));
  Serial.println(rateScale);
  int durationCount = 0;
  int threshold = rateScale * 1000;
  int delaytime = 550 - (rateScale * 500); 
  Serial.println("duration " + String(durationScale) + ", threshold "+String(threshold)+ ", delaytime " + String(delaytime));
  while (durationCount < durationScale){

    randflip(threshold);
    delay(delaytime);
    durationCount = durationCount + delaytime;
    
  }
  Serial.println("done rustling");
  
}

void onDangerMessageReceived(String message) {
  // danger message received, go into search mode;
 
  Serial.println("++++++++++++++++++++++++ got danger message!");
  rustle_leaves();
}

void onWaterMessageReceived(String message) {
/*
Value out of soil: 4095
Value in moist soil: ~2000, each reading +/-50 (range of 100)
drier soil: ~3000
 */  
  //water message received, adjust waterleve
  Serial.print("+++++++++++++++++++++++++++ got water message: ");
  Serial.println(message);
  waterLevel = message.toInt();
}

void onPoopMessageReceived(String message) {
  Serial.println("Ppppppppppppppppppppppppppp poop message received");
  poopLevel++;
}
