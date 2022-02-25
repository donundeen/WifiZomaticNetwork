#include <Arduino.h>
#include <WiFi.h>
void setup() {
  Serial.begin(115200);
  WiFi.begin("JJandJsKewlPad", "WeL0veLettuce");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi connecting...");
    Serial.print(WiFi.status());
    delay(500);
  }
  Serial.println("Wifi connected");
}
void loop() {
  // put your main code here, to run repeatedly:
}
