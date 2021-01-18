/*
  gurues@2021

  Gateway ESPNOW mandos Open Led Race 

  Basado en código de Rui Santos: https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/
  
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Descomentar para realizar Degug por el puerto serie
#define DEBUG_Gateway_OLR

#define P_1  D1            // Salida Jugador 1
#define P_2  D2            // Salida Jugador 2
#define P_3  D3            // Salida Jugador 3
#define P_4  D5            // Salida Jugador 4
#define led  D4            // Led control recepcion datos

// Estructura de los datos a recibir
// Debe de ser la misma que el o los emisores
typedef struct struct_message {
    int player;
    bool pulso;
} struct_message;

// Estructura del mesaje recibido
struct_message myData;

// Callback funcion que se ejecuta cuando el dato es recibido
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  
  memcpy(&myData, incomingData, sizeof(myData));

  #ifdef DEBUG_Gateway_OLR
    char macStr[18];
    Serial.print("Packet received from: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    Serial.printf("Player ID %u: %u bytes\n", myData.player, len);
    Serial.printf("pulso value: %d \n", myData.pulso);
    Serial.println();
  #endif
  
  if (myData.player == 1){
    digitalWrite (P_1, myData.pulso);
    digitalWrite (led, myData.pulso);
    
  }
  if (myData.player == 2){
    digitalWrite (P_2, myData.pulso);
    digitalWrite (led, myData.pulso);
  }
  if (myData.player == 3){
    digitalWrite (P_3, myData.pulso);
    digitalWrite (led, myData.pulso);
  }
  if (myData.player == 4){
    digitalWrite (P_4, myData.pulso);
    digitalWrite (led, myData.pulso);
  }

}
 
//**************************************************************************************************************************************

void setup() {

  #ifdef DEBUG_Gateway_OLR
    Serial.begin(115200);
  #endif

  pinMode (P_1, OUTPUT);
  digitalWrite (P_1, HIGH);
  pinMode (P_2, OUTPUT);
  digitalWrite (P_2, HIGH);
  pinMode (P_3, OUTPUT);
  digitalWrite (P_3, HIGH);
  pinMode (P_4, OUTPUT);
  digitalWrite (P_4, HIGH);
  pinMode (led, OUTPUT);
  digitalWrite (led, HIGH);

  // Configura el dispositivo como Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Inicia protocolo ESP-NOW
  if (esp_now_init() != 0) {
    #ifdef DEBUG_Gateway_OLR
      Serial.println("Error inicializando ESP-NOW");
    #endif
    return;
  }
 
  // Se configura el disòsitivo como Receptor
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  // Se registra callback de recepción de mensajes
  esp_now_register_recv_cb(OnDataRecv);

  #ifdef DEBUG_Gateway_OLR
    Serial.println(" ");
    Serial.println("INICIALIZADO GATEWAY ESPNOW OPEN LED RACE");
  #endif

}

void loop(){

}