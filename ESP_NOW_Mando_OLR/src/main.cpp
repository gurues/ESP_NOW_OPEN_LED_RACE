/*
  gurues@2021

  Mando para Open Laed Race con comunicación ESPNOW

  Basado  en el ejemplo de Rui Santos: https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/
  
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>

// Descomentar para realizar Degug por el puerto serie
#define DEBUG_Mando_OLR

// RECEPTOR MAC Address
uint8_t broadcastAddress[] = {0x48, 0x3F, 0xDA, 0x95, 0x9D, 0x05};

#define pulsador  D1            // pulsador del mando
#define led       D4            // Led de control placa Wemos

// Indica el número del mando del jugador a configurar: 
// 1 -> Jugador 1   Rojo
// 2 -> Jugador 2   Verde
// 3 -> Jugador 3   Azul
// 4 -> Jugador 4   Blanco
#define JUGADOR 3

// Estructura de los datos a enviar
// Debe de ser la misma que el receptor
typedef struct struct_message {
    int player;                     // Nº Jugador
    bool pulso;                     // Estado del boton pulsador
} struct_message;

struct_message myData;

volatile bool Pulsador_ON = false;  // Estado del pulsador del mando (variable interrupción) 
bool enviado = false;               // false -> no enviado  true -> enviado
bool dato = false;                  // flag para que solo envia 1 dato hasta la confirmación de envio
bool envio = true;                  // dato a enviar


// Callback cuando el dato es enviado
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  enviado = true;
  #ifdef DEBUG_Mando_OLR
    Serial.print("\r\nEstado ultimo mensaje: ");
    if (sendStatus == 0){
      Serial.println("Dato enviado");
    }
    else{
      Serial.println("Fallo de envio");
    }
  #endif
}

// Gestiona la interrupción de tarjeta NFC presente
void ICACHE_RAM_ATTR IRQ_ISR(){

  Pulsador_ON = true;

}

//*************************************************************************************************************

void setup() {

  #ifdef DEBUG_Mando_OLR
    Serial.begin(115200);
  #endif

  //Configuración de Pines
  pinMode (pulsador, INPUT_PULLUP);
  pinMode (led, OUTPUT);
  digitalWrite (led, HIGH);
 
  // Configura dispositivo como Wi-Fi Station
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Inicio protocolo ESP-NOW
  if (esp_now_init() != 0) {
    #ifdef DEBUG_Mando_OLR
      Serial.println("Error iniciando ESP-NOW");
    #endif
    return;
  } 
  // Set ESP-NOW rol: Remitente -> Mando
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);

  // Registro callback de envio mensaje
  esp_now_register_send_cb(OnDataSent);
  
  // Emparejar Remitente (Mando) con Receptor
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Activo interrupciones
  attachInterrupt(digitalPinToInterrupt(pulsador), IRQ_ISR, FALLING);

  #ifdef DEBUG_Mando_OLR
    Serial.println(" ");
    Serial.print("INICIALIZADO MANDO ESPNOW OPEN LED RACE JUGADOR ");
    Serial.println(JUGADOR);
  #endif

}
 
void loop() {

  if (Pulsador_ON) {
    noInterrupts(); // desactivo interrupciones
    digitalWrite (led, LOW);
    if ((!(enviado))&&(!(dato))){
      dato = true;
      // Se configuran datos a enviar
      myData.player = JUGADOR;
      myData.pulso = envio;
      // Envio mensaje via ESP-NOW
      esp_now_send(0, (uint8_t *) &myData, sizeof(myData));
      #ifdef DEBUG_Mando_OLR
        Serial.print("Jugador " + String(JUGADOR) + " envia: "); envio ? Serial.println("ON") :  Serial.println("OFF");
      #endif
      envio = !envio;
    }

    if (enviado){
      Pulsador_ON = false;
      enviado = false;
      dato = false;
      digitalWrite (led, HIGH);
      interrupts(); // Activo interrupciones
    }

  }

}

  
