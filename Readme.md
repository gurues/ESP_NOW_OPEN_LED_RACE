# OPEN LED RACE ESPNOW COMUNICATION

## -------------------------------------------------------------------------------------------------------------

### Cuando era pequeño siempre pedia para reyes un scalextri, pero los reyes pensaban en otros regalos mejores para mi 

### Desde que vi el proyecto Open Led Race (https://openledrace.net/) sabia que lo construiria para mis hijos ... y para mí. Después vi un proyecto de Germán Martín realizando mandos inalambricos mediante comunicación ESPNOW y ESP8266, lo queme hizo investigar un poco sobre este tipo de comunicación

### El resultado es una adaptación del código Open Led Race de 4 player (https://gitlab.com/open-led-race/olr-arduino) donde programo directamente el número de jugadores y el sonido. He diseñado 2 pequeños prgramas uno para los mandos con comunicación ESPNOW y otro para el Gateway ESPNOW

### El funcionamiento es muy básico, cuando se pulsa el pulsador del mando se produce una interrupción lo que proboca que se envie un mensaje mediantel protocolo ESPNOW con la siguiente información:

- Número de Jugador
- Datos "0" o un "1" ( rota de forma aleatoria) mediante
  
### El Gateway recibe todos los mensajes de los mandos enlazados al mismo, y teniendo en cuenta el Jugador y el dato asociado activa o no el puerto correspondiente al jugador en el Open Led Race

### Para que la comunicación entre los mando y el Gateway ESPNOW sea efectiva es necesario saver la Mac Address del microcontrolador que realiza de Gateway, en nuestro caso un Wemos D1 mini. Para ello puedes cargar el programa "MAC_Address" en el Wemos del Gateway

## MANDO ESPNOW

### El programa de los mando con comunicación ESPNOW se encuentra en la carpeta "ESP_NOW_Mando_OLR"

### Para un funcionamiento correcto se debe modificar las siguientes variables:

// Gateway_ESPNOW MAC Address
    uint8_t broadcastAddress[] = {0x??, 0x??, 0x??, 0x??, 0x??, 0x??};

- Introducir la dirección MAC del Gateway_ESPNOW para emparejar los mandos con el Gateway ESPNOW.

// Indica el número del mando del jugador a configurar: 
// 1 -> Jugador 1   Rojo
// 2 -> Jugador 2   Verde
// 3 -> Jugador 3   Azul
// 4 -> Jugador 4   Blanco
    #define JUGADOR ?

- Introducir el mumero de jugador para que se configure dicho mando.

## GATEWAY ESPNOW

### El programa del Gateway ESPNOW se encuentra en la carpeta "ESP_NOW_Gateway_OLR". No es necesario la configuración de ninguna variable para su correcto funcionamiento

## INTERCONEXIÓN ENTRE OPEN LED RACE y GATEWAY ESPNOW

### La conexión entre el Gateway ESPNOW y el Arduino Nano se realiza puerto a puerto: (WEMOS D1, D2, D3, D5) -> Arduino Nano (A1, A0, A3, A4)

### Las tiras de led pueden ser alimentadas a 5V o a 12V por lo que en el diseño de las conexiones se incluye un interruptor para seleccionar la alimentación del proyecto y de esta forma utilizar ambas tiras de led

### En la carperta "Desing-3D" encontraras un esquema electrico de las conexiones realizadas

## DISEÑOS 3D

### En la carpeta "Desing-3D" se encuentran los diseños 3D de los mandos y de la caja de control

### El diseño de los mandos es de German Martín, con unas pequeñas modificaciones mias para ajustar dicho diseño a los componentes utilizados en este proyecto (Gracias Germán por compartir el archivo XXXX)

## MATERIAL NECESARIO

### Mandos y Gateway ESPNOW

- 5 Wemos D1 mini (4 mandos y el Gateway ESPNOW)
- 4 Pulsadores
- 4 Cargadores Bateria 18650
- 4 Baterias 18650
- 1 Adaptador de corriente Booster
- 1 Conector de alimentación
- 1 Interruptor de 3 posiciones

### Open Led Race

- Tira de led de 5V o 12V 8112B
- 1 Ardino Nano
- 1 Resistncia de 470 ohmios
- 2 Condensadores electroliticos de 1000 microfaradios
- 1 Condensador electrolitico de 1 microfaradio
- 1 Altavoz