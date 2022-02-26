/*
 * Board types
 * ESP32 Arduino -> Adafruit ESP32 Feather
 * ESP32 Adruino -> ESP32 Dev Module
 */

/*
 * Required libraries to install in the arduino IDE (use the Library Manager to find and install):
 * https://github.com/Hieromon/PageBuilder : PageBuilder
 * https://github.com/bblanchon/ArduinoJson : ArduinoJson
 * https://github.com/CNMAT/OSC : OSC
 * AutoConnect: https://hieromon.github.io/AutoConnect/index.html : instructions on how to install are here: 
 * follow the instructions under "Install the AutoConnect" if you can't just find it in the Library Manager
 */



// this is all the OSC libraries

#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
  SLIPEncodedUSBSerial SLIPSerial( thisBoardsSerialUSB );
#else
#include <SLIPEncodedSerial.h>
  SLIPEncodedSerial SLIPSerial(Serial1);
#endif

#include <OSCData.h>
#include <OSCBundle.h>
#include <OSCBoards.h>
#include <OSCTiming.h>
#include <OSCMessage.h>
#include <OSCMatch.h>


#include <Arduino.h>
#include <WiFi.h>

const char *WIFI_SSID = "JJandJsKewlPad";
const char *WIFI_PASSWORD = "WeL0veLettuce";
//const char *WIFI_SSID = "PILGRIMAGE_25";
//const char *WIFI_PASSWORD = "";
//const char * UDPReceiverIP = "10.0.0.164"; // ip where UDP messages are going
// const char *UDPReceiverIP = "10.0.0.174"; // main laptop // ip where UDP messages are going
const char *UDPReceiverIP = "10.0.0.162"; // mac mini // ip where UDP messages are going
//const char *UDPReceiverIP = "192.168.8.168"; // ip where UDP messages are going

const int UDPPort = 9002; // the UDP port that Max is listening on

bool wifi_connected =false;
String thisarduinomac = "";
String thisarduinoip = "";


//create UDP instance
WiFiUDP udp;
OSCErrorCode error;


void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connecting...");
    Serial.print(WiFi.status());
    delay(500);
  }
  Serial.println("Wifi connected");

  
  
}
void loop() {
  // put your main code here, to run repeatedly:
  sendOSCUDP("A", 5);



  
   OSCBundle bundleIN;
   int size;
 
   if( (size = udp.parsePacket())>0)
   {
     Serial.println("got message");
     while(size--){
      Serial.println(size);
       bundleIN.fill(udp.read());
     }
     if(!bundleIN.hasError()){
       bundleIN.route("/plantmessage", handleinput);
     }
   }
  delay(1000);

}


void handleinput(OSCMessage &msg, int num){
  Serial.println("handling input");
  
}

// sending data over OSC/UDP.
void sendOSCUDP(String letter, int number){

  configUdp();
   
 if(WiFi.status() == WL_CONNECTED){   
  //send hello world to server
  char ipbuffer[20];
  thisarduinoip.toCharArray(ipbuffer, 20);
  OSCMessage oscmsg("/CHANNELNAMEHERE/1");  
//  oscmsg.add(letter).add(number).add(ipbuffer);
  char letterPtr[6];

  letter.toCharArray(letterPtr,2); 
  oscmsg.add(letterPtr).add(number);//.add(ipbuffer);
  Serial.print("sending data ");
  Serial.print(letter);
  Serial.println(number);
  Serial.println(ipbuffer);
  Serial.println(UDPReceiverIP);
  Serial.println(UDPPort);
  udp.beginPacket(UDPReceiverIP, UDPPort);
//  udp.write(buffer, msg.length()+1);
  oscmsg.send(udp);
  udp.endPacket();
  oscmsg.empty();
 }else{
  Serial.println("not sending udp, not connected");
 }

  
}

/*
 * connecting to UDP port 
 */
void configUdp(){
  if(!wifi_connected && WiFi.status() == WL_CONNECTED){
    Serial.println("HTTP server:" + WiFi.localIP().toString());
    thisarduinoip = WiFi.localIP().toString();
    Serial.println("SSID:" + WiFi.SSID());
    wifi_connected = true;
    udp.begin(UDPPort);
  }
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("wifi not connected");
    wifi_connected = false;
  }
}
