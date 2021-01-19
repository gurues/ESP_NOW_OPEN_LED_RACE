## -----------------------------------------------------------------------------------------------------------

# OPEN LED RACE ESPNOW COMUNICATION (4 PLAYER)

## ------------------------------------------------------------------------------------------------------------

### Cuando era pequeño siempre pedía para reyes un scalextri, pero los reyes pensaban en otros regalos mejores para mí

### Desde que vi el proyecto Open Led Race (https://openledrace.net/) sabía que lo construiría para mis hijos ... y para mí. Después ví un proyecto de Germán Martín realizando mandos inalámbricos mediante comunicación ESPNOW y ESP8266 (https://github.com/gmag11/OpenLedRace-Wireless-Remote). Investigue un poco sobre este tipo de comunicación (https://randomnerdtutorials.com/esp-now-many-to-one-esp8266-nodemcu/) y la aplique a este proyecto de una forma mucho más fácil y primitiva que Germán

### El resultado es una adaptación del código Open Led Race de 4 player (https://gitlab.com/open-led-race/olr-arduino) donde los mandos son inalámbricos mediante comunicación ESPNOW. He diseñado 2 pequeños códigos uno para los mandos con comunicación ESPNOW y otro para el Gateway ESPNOW

### El funcionamiento es muy básico, cuando se pulsa el pulsador de cualquier mando se produce una interrupción en el propio mando lo que provoca que se envíe un mensaje mediante el protocolo ESPNOW con la siguiente información:

- Número de Jugador
- Dato "0" o "1" (rota de forma cíclica -> 0, 1, 0, 1, 0 .......)
  
### El Gateway recibe todos los mensajes de los mandos enlazados al mismo, y teniendo en cuenta el Jugador y el dato asociado activa o no el puerto correspondiente al jugador en el Open Led Race

### Para que la comunicación entre los mandos y el Gateway ESPNOW sea efectiva es necesario saber la Mac Address del microcontrolador que realizará la función de Gateway, en nuestro caso un Wemos D1 mini. Para determinar la MAC puedes cargar el código que está en la carpeta "MAC_Address" en el Wemos del Gateway (código obtenido de https://randomtutorials.com/)

## MANDO ESPNOW

### El código de los mando con comunicación ESPNOW se encuentra en la carpeta "ESP_NOW_Mando_OLR"

### Para un funcionamiento correcto se debe modificar las siguientes variables:

### // Gateway_ESPNOW MAC Address

### uint8_t broadcastAddress[] = {0x??, 0x??, 0x??, 0x??, 0x??, 0x??};

- Introducir la dirección MAC del Gateway_ESPNOW para emparejar los mandos con el Gateway ESPNOW

### Indica el número del mando del jugador a configurar

### 1 -> Jugador 1   Rojo

### 2 -> Jugador 2   Verde

### 3 -> Jugador 3   Azul

### 4 -> Jugador 4   Blanco

### #define JUGADOR ?


- Introducir el número de jugador para que se configure dicho mando.

## GATEWAY ESPNOW

### El código del Gateway ESPNOW se encuentra en la carpeta "ESP_NOW_Gateway_OLR". No es necesaria la configuración de ninguna variable para su correcto funcionamiento

## OPEN LED RACE

### El código del programa Open Led Race se encuentra en la carpeta "Open_Led_Race_4_Player" y solo se ha modificado respecto al original lo siguiente:

- Aumentar la espera para la configuración de mandos y carrera
  - línea 825:   delay(3000);  // gurues -> Retraso para la configuración

- Configuración de 4 JUGADORES. Solo es necesario pulsar el último mando a configurar
  - linea 834:   controller_init( &switchs[2], DIGITAL_MODE, DIG_CONTROL_3 );  // gurues
  - linea 835:   car_init( &cars[2], &switchs[2], COLOR3 );                    // gurues
  - linea 836:   ++race.numcars;                                               // gurues

- Activación permanente del sonido
  - línea 866:   SMOTOR=1; // gurues -> Activación del sonido motor

### Si lo crees necesario, modifica los parámetros por defecto de la carrera. Se realiza en el archivo olr-param.h cambiando los siguientes parámetros:

- Número de led de la pista
  - línea 13:  #define MAXLED       300

- Número de led de los Boxes
  - línea 14:  #define BOXLEN       60

- Número de vueltas de la carrera
  - línea 15:  #define NUMLAP       5

### El Open Lace Race (4 Players) guarda las configuraciones en la EEPROM del Arduino nano, por lo que es posible que aún modificando el número de led en el programa si la EEPROM se ha actualizado no te actualice el número máximo de led. La modificación de los parámetros de configuración grabados en la EEPROM se pueden modificar mediante comandos por el puerto serie o con la aplicación desarrollada en el proyecto Open Lace Race (App -> https://gitlab.com/open-led-race/desktopapp/-/tree/master/). En la carpeta Desing-3D esta un manual para la configuración del Open Led Race mediante puerto serie en el archivo OLRN_Protocol_Serial.pdf

### La configuración de los jugadores y resto de opciones se mantienen como las del código original (https://openledrace.net/tutorials/), con la salvedad que para obtener un "0" o un "1" cuando se pulsa el mando es necesario observa el led del Wemos Gateway ESPNOW. Si el led está encendido el dato recibido es un "0" si está apagado el dato recibido es un "1". También cabe la posibilidad de añadir un led externo para verficar esta indicación conectándolo a la patilla D4 del Wemos Gateway ESPNOW, a través de una  resistencia de 220 ohmios. La modificación de los parámetros de configuración se ejecuta durante el proceso de arranque, cuando el dato recibido del mando correspondiente es un "0", igual que en el código original del Open Led Race. La única diferencia respecto a las configuraciones en el momento de arranque es que solo es necesario pulsar sobre el último mando a utilizar. Si quieres configurar para 3 Players, se pulsará el mando del Jugador 3 y si quieres configurar para 4 Players, se pulsará solo el mando del jugador 4

## INTERCONEXIÓN ENTRE OPEN LED RACE y GATEWAY ESPNOW / CONEXIONES MANDO ESPNOW OLR

### La conexión entre el Gateway ESPNOW y el Arduino Nano se realiza puerto a puerto: (WEMOS D1, D2, D3, D5) -> Arduino Nano (A2, A0, A3, A1)

### Las tiras de led pueden ser alimentadas a 5V o a 12V, por lo que en el diseño de las conexiones se incluye un interruptor para seleccionar la alimentación del proyecto, y de esta forma utilizar ambas tiras de led

### En la carpeta "Desing-3D" encontraras un esquema eléctrico de las conexiones realizadas así como su diseño PCB (El PCB no ha sido probado, por lo que no garantizo su funcionamiento). También encontraras una colección de fotos del proyecto

## CONEXIONES MANDO ESPNOW OLR

### Los mandos OLR ESPNOW se pueden realizar de tres formas diferentes:

- 1ª OPCIÓN: Mediante un porta baterias 18650 con cargador y protecciones inclidas en el "shield", botón pulsador y Wemos D1 mini.
- 2ª OPCIÓN: Mediante porta baterias 18650, modulo de carga y protecciones de baterias 18650 TP4056, microinterruptor de 2 polos, botón pulsador y Wemos D1 mini.
- 3ª OPCIÓN: Mediante botón puldador, "shield" porta baterias 18650, cargador y protecciones ademas de el WEMOS D1 mini o ESP32, todo en la misma placa. Esta opción es la más facil de montar y la más económica.

### En la carpeta "Desing-3D" encontraras fotos de las conexiones de los mandos OLR ESPNOW

## DISEÑOS 3D

### En la carpeta "Desing-3D" se encuentran los diseños 3D de los mandos ESPNOW, caja de control, soportes para los mandos y la caja de control así como soportes para la tira led

### El diseño de los mandos es de German Martín, con unas pequeñas modificaciones mias para ajustar dicho diseño a los componentes utilizados en este proyecto (Gracias Germán por compartir los diseños)

## MATERIAL NECESARIO

### Mandos y Gateway ESPNOW

- 5 Wemos D1 mini (4 mandos y el Gateway ESPNOW) (4,25 €) -> https://es.aliexpress.com/item/32529101036.html?spm=a2g0s.9042311.0.0.36f663c0WOJN3M
- 4 Pulsadores (5,69€) -> https://es.aliexpress.com/item/32858643564.html?spm=a2g0s.9042311.0.0.274263c0AGlmOz
- 4 Cargadores Batería 18650 (2,74€) -> https://es.aliexpress.com/item/32910602118.html?spm=a2g0s.9042311.0.0.274263c0Vn9AQd
- 4 WeMos D1 ESP-Wroom-02 ESP8266 Nodemcu con carga de batería 18650 (5,30 €) -> https://es.aliexpress.com/item/4000593916138.html?spm=a2g0o.detail.1000060.1.69ad161fxYqykB&gps-id=pcDetailBottomMoreThisSeller&scm=1007.13339.169870.0&scm_id=1007.13339.169870.0&scm-url=1007.13339.169870.0&pvid=7de114b2-5daf-4faa-872d-e3b3c1729299&_t=gps-id:pcDetailBottomMoreThisSeller,scm-url:1007.13339.169870.0,pvid:7de114b2-5daf-4faa-872d-e3b3c1729299,tpp_buckets:668%230%23131923%2363_668%23888%233325%236_668%232846%238113%231998_668%232717%237561%23318_668%231000022185%231000066059%230_668%233468%2315608%23151
  (sustituye a 4 Wemos D1 mini y a los 4 cargadores 18650)
- 4 Wemos ESP-Wroom-32 Bateria-Wifi-Bluetooth (7,14 €) -> https://es.aliexpress.com/item/32819459737.html?spm=a2g0o.productlist.0.0.1dee156c1Nn3L8&algo_pvid=16c3560e-f8fa-4c0e-8534-6c6f3fda32f4&algo_expid=16c3560e-f8fa-4c0e-8534-6c6f3fda32f4-4&btsid=2100bb4a16136675350262725e07ed&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_
  (sustituye a 4 Wemos D1 mini y a los 4 cargadores 18650)
- 4 Baterías 18650 (14,95€) -> https://es.aliexpress.com/item/1005001969453906.html?spm=a2g0s.9042311.0.0.274263c08zAvmg
- 1 Adaptador de corriente Booster DC-DC 2A (1,50€) -> https://es.aliexpress.com/item/32952511309.html?spm=a2g0o.productlist.0.0.71b4c0cdZYuQw6&algo_pvid=ea7db72c-008c-46b4-bfbe-0418440ae43b&algo_expid=ea7db72c-008c-46b4-bfbe-0418440ae43b-43&btsid=0b0a050116119326139557025eede9&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_
- 1 Conector de alimentación (2,01€) -> https://es.aliexpress.com/item/33026528272.html?spm=a2g0s.9042311.0.0.274263c0Vn9AQd
- 1 Interruptor de 3 posiciones (1,28€) -> https://es.aliexpress.com/item/32869900104.html?spm=a2g0o.productlist.0.0.32d52f27ki52rk&algo_pvid=067aac56-fba8-45f0-81c0-596151364015&algo_expid=067aac56-fba8-45f0-81c0-596151364015-3&btsid=2100bdd716119327060185320e1c29&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_

### Open Led Race

- Tira de led de 5V o 12V 8112B (13,74 €) -> https://es.aliexpress.com/item/32682015405.html?spm=a2g0s.9042311.0.0.36f663c0WOJN3M
- 1 Ardino Nano Every (12,10 €) -> https://www.amazon.es/Arduino-Nano-Every-Single-Board/dp/B07VX7MX27/ref=sr_1_5?__mk_es_ES=%C3%85M%C3%85%C5%BD%C3%95%C3%91&dchild=1&keywords=arduino+nano+every&qid=1611930894&sr=8-5
- 1 Resistencia de 470 ohmios (1,64€) -> https://es.aliexpress.com/item/32317115890.html?spm=a2g0o.productlist.0.0.adba330b5ZZ50L&algo_pvid=c199f646-e2cc-4421-abb2-fecbd5d7b7df&algo_expid=c199f646-e2cc-4421-abb2-fecbd5d7b7df-3&btsid=2100bdde16119322802446980e6619&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_
- 2 Condensadores electrolíticos de 1000 microfaradios (2,78€) -> https://es.aliexpress.com/item/32954075821.html?spm=a2g0o.productlist.0.0.28b75649wCzsUk&algo_pvid=a754e220-6ff0-4fac-8122-6612ac67e156&algo_expid=a754e220-6ff0-4fac-8122-6612ac67e156-11&btsid=2100bdd716119317475621434e1c5c&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_
- 1 Condensador electrolítico de 1 microfaradio (4,84€) -> https://es.aliexpress.com/item/4000020803391.html?spm=a2g0o.productlist.0.0.2b7672d6MsFmg6&algo_pvid=5cef38e8-dd6d-4775-ba0d-1403e596ebc7&algo_expid=5cef38e8-dd6d-4775-ba0d-1403e596ebc7-19&btsid=2100bdd716119318801646390e1c30&ws_ab_test=searchweb0_0,searchweb201602_,searchweb201603_
- 1 Altavoz 8R 0.25W 40 mm (1,58€) -> https://es.aliexpress.com/item/4001098796204.html?spm=a2g0s.9042311.0.0.274263c0AGlmOz
- Fuente alimentación 5V 3A (4€) -> https://es.aliexpress.com/item/32986101102.html?spm=a2g0s.9042311.0.0.274263c0AGlmOz

## -----------------------------------------------------------------------------------------------------------

# OPEN LED RACE ESPNOW COMUNICATION (2 PLAYER)
## -----------------------------------------------------------------------------------------------------------

### En la carpeta "ESP_NOW_Gateway_OLR_2P" se dispone del código Open Led Race, versión Arduino-Basico (https://gitlab.com/open-led-race/olr-arduino-basic), adaptado para que mediante solo el Gateway ESPNOW poder controlar la tira de led. Resumiendo, eliminando el Arduino Nano y manteniendo el resto de componentes disponenos de un Open Led Race para 2 jugadores (necesario 3 Wemos y el resto de componentes)

### Creo que la versión de 4 player también es posible realizar esta modificación pero es bastante más complicada por lo que ya veré si lo intento más adelante

## -----------------------------------------------------------------------------------------------------------

# PLATFORMIO

## -----------------------------------------------------------------------------------------------------------

### El proyecto está realizado con Platformio y para cargar el código en los respectivos microcontroladores será necesario ejecutar los siguientes instrucciones

### 1.- Abrir un terminar Platformio

### 2.- Desplázate a la carpeta del código que quieras cargar (ESP_NOW_Gateway_OLR, ESP_NOW_Mando_OLR, etc)

### 3.- Ejecuta la siguiente instrucción para cargar el código en un Wemos d1 mini (Gateway ESPNOW o Mando)

            pio run -e d1_mini -t upload 

### 4.- Ejecuta la siguiente instrucción para cargar el código en un ESP32 esp-wroom-32 (Mando)

            pio run -e wemosbat -t upload 

### 5.- Ejecuta la siguiente instrucción para cargar el código en un Arduino Nano (Open Led Race - Pistas máximo 300 led)

            pio run -e nano328 -t upload

### 6.- Ejecuta la siguiente instrucción para cargar el código en un Arduino Nano Every (Open Led Race - Pistas máximo 1500 led)

            pio run -e nano_every -t upload 

### 7.- Si quieres activar el monitor serie, ejecuta a continuación

            pio device monitor -b 115200

## -----------------------------------------------------------------------------------------------------------

# PRESUPUESTO
## -----------------------------------------------------------------------------------------------------------

### Por unos 80€ puedes disponer de un Open Led Race con Arduino Nano Every, tira de led de 5 metros (60 led/metro IP67) y 4 mandos inalámbricos ESPNOW

### Por unos 40€ puedes disponer de un Open Led Race Wemos Gateway ESPNOW, tira de led de 5 metros (60 led/metro IP67) y 2 mandos inalmbricos ESPNOW

## -----------------------------------------------------------------------------------------------------------

# AGRADECIMIENTOS
## -----------------------------------------------------------------------------------------------------------

### Quiero agradecer a los creadores del proyecto Open Led Race su gran trabajo e imaginación para darnos la posibilidad de disfrutar de este proyecto (https://openledrace.net/)

## También quiero agradecer:

- A Germán Martín su paciencia con las preguntas "nóveles" que tuvo el placer de contestarme en los inicios de este proyecto, así como su aportación "al mundo maiker" compartiendo un gran número de proyectos y librerías (https://github.com/gmag11)
- A Rui Santos por sus grandísimos tutoriales de los cuales he aprendido para poder realizar este y otros proyectos (https://randomtutorials.com/)

## gurues@2021
