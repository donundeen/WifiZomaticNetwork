/*#include <Arduino.h>
#include <WiFi.h>
*/
#include <ArduinoOSCWiFi.h>
// #include <ArduinoOSC.h> // you can use this if your borad supports only WiFi or Ethernet

const char *WIFI_SSID = "JJandJsKewlPad";
const char *WIFI_PASSWORD = "WeL0veLettuce";

// WiFi stuff
const char* ssid = "JJandJsKewlPad";
const char* pwd = "WeL0veLettuce";
const IPAddress ip(10, 0, 0, 225);
const IPAddress gateway(10, 0, 0, 1);
const IPAddress subnet(255, 255, 255, 0);

int i; float f; String s;
int publish_port= 9002;
int bind_port = 9003;


// for ArduinoOSC
//const char* host = "10.0.0.162";
const char* host = "10.0.0.74";
const int recv_port = 9003;
const int send_port = 55555;
// send / receive varibales

void onPlantMessageReceived(const OscMessage& m) {
  Serial.println("message received");
  
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
    
}

void setup() {
    Serial.begin(115200);
    delay(2000);

    // WiFi stuff (no timeout setting for WiFi)
#ifdef ESP_PLATFORM
    WiFi.disconnect(true, true);  // disable wifi, erase ap info
    delay(1000);
    WiFi.mode(WIFI_STA);
#endif
    WiFi.begin(ssid, pwd);
    WiFi.config(ip, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.print("WiFi connected, IP = ");
    Serial.println(WiFi.localIP());

    // publish osc messages (default publish rate = 30 [Hz])

/*
    OscWiFi.publish(host, publish_port, "/publish/value", i, f, s)
        ->setFrameRate(60.f);
*/
/*
 * // this sends a message every 500 milliseconds
    OscWiFi.publish(host, publish_port, "/publish/func", &millis, &micros)
        ->setIntervalMsec(500.f);
*/

  // this listens for messages, sends results to onPlantMessageReceived function
    OscWiFi.subscribe(recv_port, "/plantmessage", onPlantMessageReceived);

    
}

int count = 0;
void loop() {
    OscWiFi.update();  // should be called to receive + send osc
    count++;
    // just send message 5 times, for testing
    if(count <= 5){
      Serial.print("sending message to " );
      Serial.println(host);
      OscWiFi.send(host, publish_port, "/singlesend/func", count, 456); // to publish osc
    }

    // or do that separately
    // OscWiFi.parse(); // to receive osc
   // send single message:
//    OscWiFi.send(host, publish_port, "/singlesend/func", 123, 456); // to publish osc
    delay(500);
}
