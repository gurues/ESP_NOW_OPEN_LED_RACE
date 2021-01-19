// Dirección MAC placaS esp32, (ESP8266) -> WEMOS y NODEMCU

// Descomenta para ESP32
//#define ESP32  

#include <Arduino.h>

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif

void setup(){
  Serial.begin(115200);
  Serial.println();

  #ifdef ESP32
    Serial.print("esp32 Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
  #else
    Serial.print("WEMOS/nodemcu Board MAC Address:  ");
    Serial.println(WiFi.macAddress());
  #endif
}
 
void loop(){

}