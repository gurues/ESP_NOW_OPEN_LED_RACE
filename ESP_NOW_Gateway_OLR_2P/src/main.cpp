/*
  gurues@2021

  Gateway ESPNOW mandos Open Led Race 

  Basado en código de Rui Santos: https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/
  
*/

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <espnow.h>
#include <Adafruit_NeoPixel.h>

// Descomentar para realizar Degug por el puerto serie la parte de codigo ESP_NOW
#define DEBUG_Gateway_OLR
// Descomentar para realizar Degug por el puerto serie la parte de codigo Open Led Race
#define DEBUG_OLR

// Estructura de los datos a recibir
// Debe de ser la misma que el o los emisores
typedef struct struct_message {
    int player;
    bool pulso;
} struct_message;

// Estructura del mesaje recibido
struct_message myData;

//version Basic for PCB Rome Edition 
// 2 Player , without Boxes Track

#define WEMOS // Parte de código añadida por gurues para adaptarlo a WEMOS al código original Open Led Race

char const softwareId[] = "A2P0";  // A2P0: "A"=OpenLEDRace Team, "2P0"=Game ID (2P=2 Players, 0=Type 0 w slope w/ box)
char const version[] = "1.0.0";

#define MAXLED          300// MAX LEDs actives on strip

#ifdef WEMOS
  int PIN_P1 = 1;     
  int PIN_P2 = 1;
  #define PIN_LED        D1   // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A  
  #define PIN_AUDIO      D2   // through CAP 2uf to speaker 8 ohms
  #define led            D4   // Led de control placa Wemos
#else
  #define PIN_P1         A2   // switch player 1 to PIN and GND
  #define PIN_P2         A0   // switch player 2 to PIN and GND 
  #define PIN_LED        2  // R 500 ohms to DI pin for WS2812 and WS2813, for WS2813 BI pin of first LED to GND  ,  CAP 1000 uF to VCC 5v/GND,power supplie 5V 2A  
  #define PIN_AUDIO      3   // through CAP 2uf to speaker 8 ohms
#endif

#define INI_RAMP 80
#define MED_RAMP 90
#define END_RAMP 100
#define HIGH_RAMP 16
bool ENABLE_RAMP=0;
bool VIEW_RAMP=0;

int NPIXELS=MAXLED; // leds on track
int cont_print=0;

#define COLOR1   Color(255,0,0)
#define COLOR2   Color(0,255,0)

#define COLOR1_tail   Color(i*3,0,0)
#define COLOR2_tail   Color(0,i*3,0)

// Serial Communications
#define EOL                 '\n' // End of Command char used in Protocol

#define REC_COMMAND_BUFLEN  32
char cmd[REC_COMMAND_BUFLEN];    // Stores command received by ReadSerialComand()

#define TX_COMMAND_BUFLEN  64
char txbuff[TX_COMMAND_BUFLEN];  // to prepare command strings to send 



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
    PIN_P1 = myData.pulso;
    digitalWrite (led,myData.pulso);
  }
  if (myData.player == 2){
    PIN_P2 = myData.pulso;
    digitalWrite (led, myData.pulso);
  }
 
}
 

int win_music[] = {
  2637, 2637, 0, 2637, 
  0, 2093, 2637, 0,
  3136    
};
      
byte  gravity_map[MAXLED];     

int TBEEP=0;
int FBEEP=0; 
byte SMOTOR=0;
float speed1=0;
float speed2=0;
float dist1=0;
float dist2=0;

byte loop1=0;
byte loop2=0;

byte leader=0;
byte loop_max=5; //total laps race


float ACEL=0.2;
float kf=0.015; //friction constant
float kg=0.003; //gravity constant

byte flag_sw1=0;
byte flag_sw2=0;
byte draworder=0;
 
unsigned long timestamp=0;

Adafruit_NeoPixel track = Adafruit_NeoPixel(MAXLED, PIN_LED, NEO_GRB + NEO_KHZ800);

int tdelay = 5; 

void set_ramp(byte H,byte a,byte b,byte c)
{for(int i=0;i<(b-a);i++){gravity_map[a+i]=127-i*((float)H/(b-a));};
 gravity_map[b]=127; 
 for(int i=0;i<(c-b);i++){gravity_map[b+i+1]=127+H-i*((float)H/(c-b));};
}

void set_loop(byte H,byte a,byte b,byte c)
{for(int i=0;i<(b-a);i++){gravity_map[a+i]=127-i*((float)H/(b-a));};
 gravity_map[b]=255; 
 for(int i=0;i<(c-b);i++){gravity_map[b+i+1]=127+H-i*((float)H/(c-b));};
}

// Command Send/Receive base functions 
///////////////////////////////////////

// vars used only in these functions
Stream*  _stream = &Serial;
int      _bufIdx;

/*
 *
 */
void sendSerialCommand(char* str) {
  // get command length
  int dlen=0;
  for(; dlen<TX_COMMAND_BUFLEN; dlen++ ) { 
    if(*(str+dlen) == EOL ) {
      dlen++; // send EOC 
      break;
    }
  }
  _stream->write(str, dlen);
  return;
}


/*
 * 
 */
void send_race_phase( int phase ) {
  sprintf(txbuff, "R%d%c",phase,EOL);
  sendSerialCommand(txbuff);
}


void start_race(){send_race_phase(4); // Race phase 4: Countdown 
                  for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,0));};
                  track.show();
                  delay(2000);
                  track.setPixelColor(12, track.Color(0,255,0));
                  track.setPixelColor(11, track.Color(0,255,0));
                  track.show();
                  tone(PIN_AUDIO,400);
                  delay(2000);
                  noTone(PIN_AUDIO);                  
                  track.setPixelColor(12, track.Color(0,0,0));
                  track.setPixelColor(11, track.Color(0,0,0));
                  track.setPixelColor(10, track.Color(255,255,0));
                  track.setPixelColor(9, track.Color(255,255,0));
                  track.show();
                  tone(PIN_AUDIO,600);
                  delay(2000);
                  noTone(PIN_AUDIO);                  
                  track.setPixelColor(9, track.Color(0,0,0));
                  track.setPixelColor(10, track.Color(0,0,0));
                  track.setPixelColor(8, track.Color(255,0,0));
                  track.setPixelColor(7, track.Color(255,0,0));
                  track.show();
                  tone(PIN_AUDIO,1200);
                  delay(2000);
                  noTone(PIN_AUDIO);                               
                  timestamp=0; 
                  send_race_phase(5); // Race phase 4: Race Started 
                 };

void winner_fx(byte w) {
               int msize = sizeof(win_music) / sizeof(int);
               for (int note = 0; note < msize; note++) {
               if (SMOTOR==1) {tone(PIN_AUDIO, win_music[note]/(3-w),200);} else {tone(PIN_AUDIO, win_music[note],200);};
               delay(230);
               noTone(PIN_AUDIO);}                                             
              };


int get_relative_position1( void ) {
    enum{
      MIN_RPOS = 0,
      MAX_RPOS = 99,
    };    
    int trackdist = 0;
    int pos = 0;  
    trackdist = (int)dist1 % NPIXELS;
    pos = map(trackdist, 0, NPIXELS -1, MIN_RPOS, MAX_RPOS);    
    return pos;
}

int get_relative_position2( void ) {
    enum{
      MIN_RPOS = 0,
      MAX_RPOS = 99,
    };    
    int trackdist = 0;
    int pos = 0;  
    trackdist = (int)dist2 % NPIXELS;
    pos = map(trackdist, 0, NPIXELS -1, MIN_RPOS, MAX_RPOS);    
    return pos;
}

void print_cars_position( void ) {
 int  rpos = get_relative_position1();
 sprintf( txbuff, "p%d%d%d,%d%c", 1, 1, loop1, rpos, EOL );
  sendSerialCommand(txbuff);
 
 rpos = get_relative_position2();
 sprintf( txbuff, "p%d%d%d,%d%c", 2, 1, loop2, rpos, EOL );
 sendSerialCommand(txbuff);
 
}

/*  Non blocking: call these function in main loop()
 *   
 *  If there are bytes available in Serial, READ JUST ONE char 
 *  and ADD it to the 'internal' cmd buffer 
 *    The char just read is an END OF COMMAND char ?
 *      N: Return 0 (no complete command available yet) 
 *      Y: Return buffer length (the caller will find the command in [buf]
 */
int checkSerial(char *   buf) { 
  while (_stream->available()) {
    if(_bufIdx < REC_COMMAND_BUFLEN - 2) {
      char data = _stream->read();
      if(data == EOL) {
        int cmsSize=_bufIdx;
        buf[_bufIdx++] = '\0';
        _bufIdx=0;
        return(cmsSize);
      } else {
        buf[_bufIdx++] = data;
      }
    } else {
      // buffer full 
      // reset and retunn error
      buf[_bufIdx++] = '\0';
      _bufIdx=0;
      return(-2); 
    }
  }
  return(0);
}




/*
 * 
 */

void checkSerialCommand(){
  
  int clen = checkSerial(cmd);
  if(clen == 0) return ;       // No commands received
  if(clen  < 0) {              // Error receiving command  
    sprintf( txbuff, "!1Error reading serial command:[%d]",clen); // Send a warning to host
    sendSerialCommand(txbuff);
    return;
  }
  // clen > 0 ---> Command with length=clen ready in  cmd[]

  switch (cmd[0]) {
    case '#':                         // Handshake -> send back 
      {
        sprintf( txbuff, "#%c", EOL );
        sendSerialCommand(txbuff);    
      }
      return;

  case  '@' :                         // Enter "Configuration Mode"
    {
      // send back @OK 
      // No real cfg mode here, but send @OK so the Desktop app (Upload, configure)
      // can send a GET SOFTWARE Type/Ver command and identify this software
        sprintf( txbuff, "@OK%c", EOL );
        sendSerialCommand(txbuff);          
    }
    return;
    
  case '?' :                          // Get Software Id
    {
      sprintf( txbuff, "%s%s%c", "?", softwareId, EOL );
      sendSerialCommand(txbuff);    
    }
    return;
  
  case '%' :                          // Get Software Version
    {
      sprintf( txbuff, "%s%s%c", "%", version, EOL );
      sendSerialCommand(txbuff);    
    }
    return;    
  } 

  // if we get here, the command it's not managed by this software -> Answer <CommandId>NOK
  sprintf(txbuff, "%cNOK%c", cmd[0], EOL );  
  sendSerialCommand(txbuff);
  
  return; 
}


void burning1(){
//to do
 }

void burning2(){
//to do
 }

void track_rain_fx(){
//to do
 }

void track_oil_fx(){
//to do
 }

void track_snow_fx(){
//to do
 }


void fuel_empty(){
//to do
 }

void fill_fuel_fx(){
//to do
 }

void in_track_boxs_fx(){
//to do
 }

void pause_track_boxs_fx(){
//to do
 }
 
void flag_boxs_stop(){
//to do
 }

void flag_boxs_ready(){
//to do
 }

void draw_safety_car(){
//to do
 }

void telemetry_rx(){
  //to do
 }
 
void telemetry_tx(){
  //to do
 }

void telemetry_lap_time_car1(){
//to do
 }

void telemetry_lap_time_car2(){
//to do
 }

void telemetry_record_lap(){
//to do
 }

void telemetry_total_time(){
//to do
 }

int read_sensor(byte player){
  return(0); //to do
}

int calibration_sensor(byte player){
  return(0); //to do
}

int display_lcd_laps(){
    return(0); //to do  
}

int display_lcd_time(){
    return(0); //to do  
}

void draw_car1(void){for(int i=0;i<=loop1;i++){track.setPixelColor(((word)dist1 % NPIXELS)+i, track.COLOR1);};                   
 }

void draw_car2(void){for(int i=0;i<=loop2;i++){track.setPixelColor(((word)dist2 % NPIXELS)+i, track.COLOR2);};            
 }


//**************************************************************************************************************************************

void setup() {

  //#ifdef DEBUG_Gateway_OLR
    Serial.begin(115200);
  //#endif

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

  for(int i=0;i<NPIXELS;i++){gravity_map[i]=127;};
  track.begin(); 
  #ifdef WEMOS
    if (PIN_P1==0) //push switch 1 on reset for activate physics
  #else
    pinMode(PIN_P1,INPUT_PULLUP); 
    pinMode(PIN_P2,INPUT_PULLUP);  
  if ((digitalRead(PIN_P1)==0)) //push switch 1 on reset for activate physics
  #endif
  {ENABLE_RAMP=1;
   set_ramp(HIGH_RAMP,INI_RAMP,MED_RAMP,END_RAMP); 
   for(int i=0;i<(MED_RAMP-INI_RAMP);i++){track.setPixelColor(INI_RAMP+i, track.Color(24+i*4,0,24+i*4) );};  
   for(int i=0;i<(END_RAMP-MED_RAMP);i++){track.setPixelColor(END_RAMP-i, track.Color(24+i*4,0,24+i*4) );};                                           
   track.show();
   delay(1000);
   tone(PIN_AUDIO,500);delay(500);noTone(PIN_AUDIO);delay(500);
  #ifdef WEMOS
    if (PIN_P1==0) //push switch 1 on reset for activate physics
  #else
    if ((digitalRead(PIN_P1)==0))
  #endif 
    {VIEW_RAMP=1;} // if retain push switch 1 set view ramp
   else {for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,0));};
         track.show();
         VIEW_RAMP=0;  
        }; 
   
  };
  #ifdef WEMOS
    if (((PIN_P2)==0)) {delay(1000); tone(PIN_AUDIO,1000);delay(500);noTone(PIN_AUDIO);delay(500);if (((PIN_P2)==1)) SMOTOR=1;} //push switch 2 until a tone beep on reset for activate magic FX  ;-)
  #else
    if ((digitalRead(PIN_P2)==0)) {delay(1000); tone(PIN_AUDIO,1000);delay(500);noTone(PIN_AUDIO);delay(500);if ((digitalRead(PIN_P2)==1)) SMOTOR=1;} //push switch 2 until a tone beep on reset for activate magic FX  ;-)
  #endif
  start_race();

}

void loop() {   

    // look for commands received on serial 
    checkSerialCommand();

  
    for(int i=0;i<NPIXELS;i++){track.setPixelColor(i, track.Color(0,0,0));};
    if ((ENABLE_RAMP==1) && ( VIEW_RAMP==1)) {for(int i=0;i<(MED_RAMP-INI_RAMP);i++){track.setPixelColor(INI_RAMP+i, track.Color(24+i*4,0,24+i*4) );};  
                                              for(int i=0;i<(END_RAMP-MED_RAMP);i++){track.setPixelColor(END_RAMP-i, track.Color(24+i*4,0,24+i*4) );};                                           
                                             };    
    #ifdef WEMOS
      if ( (flag_sw1==1) && ((PIN_P1)==0) ) {flag_sw1=0;speed1+=ACEL;};
      if ( (flag_sw1==0) && ((PIN_P1)==1) ) {flag_sw1=1;};
    #else
      if ( (flag_sw1==1) && (digitalRead(PIN_P1)==0) ) {flag_sw1=0;speed1+=ACEL;};
      if ( (flag_sw1==0) && (digitalRead(PIN_P1)==1) ) {flag_sw1=1;};
    #endif
    if ((gravity_map[(word)dist1 % NPIXELS])<127) speed1-=kg*(127-(gravity_map[(word)dist1 % NPIXELS]));
    if ((gravity_map[(word)dist1 % NPIXELS])>127) speed1+=kg*((gravity_map[(word)dist1 % NPIXELS])-127);
        
    speed1-=speed1*kf; 
    #ifdef WEMOS
      if ( (flag_sw2==1) && ((PIN_P2)==0) ) {flag_sw2=0;speed2+=ACEL;};
      if ( (flag_sw2==0) && ((PIN_P2)==1) ) {flag_sw2=1;};
    #else
      if ( (flag_sw2==1) && (digitalRead(PIN_P2)==0) ) {flag_sw2=0;speed2+=ACEL;};
      if ( (flag_sw2==0) && (digitalRead(PIN_P2)==1) ) {flag_sw2=1;};
    #endif
    if ((gravity_map[(word)dist2 % NPIXELS])<127) speed2-=kg*(127-(gravity_map[(word)dist2 % NPIXELS]));
    if ((gravity_map[(word)dist2 % NPIXELS])>127) speed2+=kg*((gravity_map[(word)dist2 % NPIXELS])-127);
        
    speed2-=speed2*kf; 
        
    dist1+=speed1;
    dist2+=speed2;

    if (dist1>dist2) {if (leader==2) {FBEEP=440;TBEEP=10;}
                      leader=1;} 
    if (dist2>dist1) {if (leader==1) {FBEEP=440*2;TBEEP=10;}
                      leader=2;};

    
      
    if (dist1>NPIXELS*loop1) {loop1++;TBEEP=10;FBEEP=440;};
    if (dist2>NPIXELS*loop2) {loop2++;TBEEP=10;FBEEP=440*2;};

    if (loop1>loop_max) {sprintf( txbuff, "w1%c", EOL );
                         sendSerialCommand(txbuff); // Send Winner=1 command
                         for(int i=0;i<NPIXELS/10;i++){track.setPixelColor(i,track.COLOR1_tail);}; track.show();
                         winner_fx(1);loop1=0;loop2=0;dist1=0;dist2=0;speed1=0;speed2=0;timestamp=0;
                         start_race();
                        }
    if (loop2>loop_max) {sprintf( txbuff, "w2%c", EOL );
                         sendSerialCommand(txbuff); // Send Winner=2 command
                         for(int i=0;i<NPIXELS/10;i++){track.setPixelColor(i, track.COLOR2_tail);}; track.show();
                         winner_fx(2);loop1=0;loop2=0;dist1=0;dist2=0;speed1=0;speed2=0;timestamp=0;
                         start_race();
                        }
   
     if ((millis() & 512)==(512*draworder)) {if (draworder==0) {draworder=1;}
                          else {draworder=0;}
                         };
    if (abs(round(speed1*100))>abs(round(speed2*100))) {draworder=1;};
    if (abs(round(speed2*100))>abs(round(speed1*100))) {draworder=0;};
       
    if ( draworder==0 ) {
      draw_car1();
      draw_car2();
    }
    else {
      draw_car2();
      draw_car1();
    } 
                 
    track.show(); 
    if (SMOTOR==1) tone(PIN_AUDIO,FBEEP+int(speed1*440*2)+int(speed2*440*3));
    delay(tdelay);
    if (TBEEP>0) {TBEEP--;} else {FBEEP=0;};
    cont_print++; 
    if (cont_print>100) {print_cars_position();cont_print=0;}
         
}


