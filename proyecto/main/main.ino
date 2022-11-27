#include <EthernetENC.h>
#include "main.h"
#include <SPI.h>
//#include <UIPEthernet.h>
#include <PubSubClient.h>

//LISTA CODIGOS SLAVE
// Abrir puerta interior es 8.


const int PINOUT_INPUTS[] = {E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12};
const int PINOUT_OUTPUTS[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12};

bool FLAG_ESCLUSA = false;
bool FLAG_STATE = false;
bool FLAG_SENSOR = false;
bool FLAG_BT = false;
bool FLAG_ETH = false;
bool FLAG_ABIERTO=false;
bool FLAG_SLAVE=false;
bool FLAG_PUERTA_INTERNA = false; //false esta cerrada, true esta abierta
bool FLAG_FLUJO = true; //true = entrada , false = salida
bool FLAG_ADENTRO = false;
bool MANTENER_ABIERTA = false;
bool MANTENER_CERRADA = false;
bool CLIENTE = true;

char ESCLUSA_BT;
char NOESCLUSA_BT;
char ESCLUSA_ETH;
char NOESCLUSA_ETH;
char READ_ETH;
char READ_BT;
char READ_SLAVE;


//Texto
String STATE;
String OFF = "APAGADA";  //1
String AUTOMATIC = "AUTOMATICA";  //2
String ONLYLEAVE = "SOLOSALIR"; //3
String OPEN = "ABIERTA";  //4
String NIGHT = "NOCHE"; //5
String PANIC = "PANICO";  //6
String ESCLUSA = "ESCLUSA"; //7
String LAST_STATE;
String command;

/*--------------------------------- CONFIGS. ETHERNET ---------------------------------*/
// Update these with values suitable for your network.
byte mac[] = {0xDE,0xED,0xBA,0xFE,0xFE,0xED};
IPAddress ip(192,168,1,25);

const char* mqtt_server = "demo.thingsboard.io";    // servidor de la aplicacion IoT
const char* token = "I5O29Aj1nla0WptymuX9";         // token de la aplicacion IoT


// Connection objects
EthernetClient ethClient;
PubSubClient client(ethClient);

//buffer sizes and messages
#define MSG_BUFFER_SIZE 50
char msg[MSG_BUFFER_SIZE];


/*--------------------------------- INTERRUPCIONES ---------------------------------*/
//Me queda la duda si esto es un switch o cada vez que recibe se pone en esclusa..
void isr_esclusa(){
  FLAG_ESCLUSA = !FLAG_ESCLUSA; //Pongo el flag de esclusa prendida o apagada en el opuesto, por defecto empiezo el programa con esclusa apagada
  LAST_STATE=STATE;
  if(FLAG_ESCLUSA) STATE=ESCLUSA;
  else STATE=AUTOMATIC;
  Serial.println("Activo interrupcion esclusa");
}

void isr_panico(){
  LAST_STATE = STATE;
  STATE = PANIC;
  Serial.println("Activo interrupcion Panico");
}

void isr_abrir_interior(){
  if(!FLAG_ABIERTO)abrir_puerta_int();
  Serial.println("Activo interrupcion abrir puerta interior");
}

void isr_cierre(){
  //timer_cierre();//Empiezo timer del cierre de las puertas.cdo se cierra la puerta automatica se manda un modo noche..
  LAST_STATE = STATE;
  STATE = ONLYLEAVE;
  Serial.println("Activo interrupcion Solo Salir");
}


/*--------------------------------- SETUP ---------------------------------*/
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(E1,INPUT);
  pinMode(E2,INPUT);
  pinMode(E3,INPUT);
  pinMode(E4,INPUT);
  pinMode(E5,INPUT);
  pinMode(E6,INPUT);
  pinMode(E7,INPUT);
  pinMode(E8,INPUT);
  pinMode(E9,INPUT);
  pinMode(E10,INPUT);
  pinMode(E11,INPUT);
  pinMode(E12,INPUT);
  pinMode(P1,INPUT);
  pinMode(P2,INPUT);
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(S3,OUTPUT);
  pinMode(S4,OUTPUT);
  pinMode(S5,OUTPUT);
  pinMode(S6,OUTPUT);
  pinMode(S7,OUTPUT);
  pinMode(S8,OUTPUT);
  pinMode(S9,OUTPUT);
  pinMode(S10,OUTPUT);
  pinMode(S11,OUTPUT);
  pinMode(S12,OUTPUT);
  attachInterrupt(digitalPinToInterrupt(P1),isr_cierre, RISING);
  attachInterrupt(digitalPinToInterrupt(P2),isr_esclusa, RISING);
  attachInterrupt(digitalPinToInterrupt(E11),isr_abrir_interior, RISING);
  attachInterrupt(digitalPinToInterrupt(E12),isr_panico, RISING);
  /*------ SETUP ETH ------*/
  pinMode(53,OUTPUT);
  // La Abuela esta bastante bien!!
  Ethernet.init(10);
  Ethernet.begin(mac,ip);
  // Allow the hardware to sort itself out
  delay(1500);
  Serial.println("Ethernet paso delay");
  client.setServer(mqtt_server, 1883);
//  client.setCallback(callback);
  Serial.println("");
  Serial.println("Eth conectado");
  Serial.println("IP: ");
  Serial.println(Ethernet.localIP());
}

/*--------------------------------- ESCLUSA ---------------------------------*/

void mode_esclusa(){
  //Aviso el cambio de modo por serial.
  //Destrabo la puerta mecanicamente con el siguiente pulso
  //pulse(PINOUT_OUTPUTS[11],false);
  digitalWrite(S7,LOW);//Apago el DPS si estoy en modo esclusa
  Serial.println("Apago DPS");
  int i=0;
  
  while(STATE==ESCLUSA){
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("1")){
        FLAG_ABIERTO=true;//Acordrase que esto llega al reves.
        slave_send('5');
        Serial.println("PUERTA EXTERIOR ABIERTA");
      }
      if(command.equals("1-")){
        FLAG_ABIERTO=false; //Para avisar que mando el bajo
        slave_send('4');
        Serial.println("PUERTA EXTERIOR CERRADA");
      }
      if(command.equals("2"))requiere_averia();
      
      if(command.equals("3")) manejar_puerta(1);
      if(command.equals("4")) MANTENER_ABIERTA = true;
      if(command.equals("4-")) MANTENER_ABIERTA = false;
      if(command.equals("5")) manejar_puerta(2);
      if(command.equals("6")) MANTENER_ABIERTA = true;
      if(command.equals("6-")) MANTENER_ABIERTA = false;
      if(command.equals("7")) MANTENER_CERRADA = true;
      if(command.equals("7-")) MANTENER_CERRADA = false;
      if(command.equals("8")) MANTENER_CERRADA = true;
      if(command.equals("8-")) MANTENER_CERRADA = false;
      
      if(command.equals("9"))rfid_externa_e();
      if(command.equals("10"))rfid_interna_e();
    }
  }
    check_peripherals();
    if(FLAG_ETH) ethernet();
    if(FLAG_BT) bluetooth();
    if(FLAG_SLAVE) slave_receive();
    FLAG_ETH = FLAG_BT = FLAG_SLAVE = false;
}

void manejar_puerta(int mov){
  //Puerta interna cerrada
  if(!FLAG_PUERTA_INTERNA){
    if(FLAG_ADENTRO){
      if(!FLAG_FLUJO){
        if(!FLAG_ABIERTO){
            if(!MANTENER_CERRADA){
              if(mov==1){
                pulse(PINOUT_OUTPUTS[11],false);
                pulse(PINOUT_OUTPUTS[10],false);
                FLAG_FLUJO=false;
                Serial.println("Habia alguien en la esclusa y esta saliendo");
                //REPRODUCIR AUDIO DE ADIOS, VUELVE PRONTO!
              }
            }
          }
          else{
            if(!MANTENER_ABIERTA){
              if(mov==2){
                mode_night(true);
                FLAG_ADENTRO=false;
                Serial.println("Habia alguien en la esclusa y Cerro puerta exterior");
              }
            }
          }
      }
    }    
    else{
        //Entrada
      if(!FLAG_ABIERTO){
        if(!MANTENER_CERRADA){
          if(mov==1){
            pulse(PINOUT_OUTPUTS[11],false);
            pulse(PINOUT_OUTPUTS[10],false);
            FLAG_FLUJO=true;
            Serial.println("Alguien entra por puerta exterior");
            //REPRODUCIR AUDIO DE BIENVENIDA
          }
        }
      }
      else{
        if(!MANTENER_ABIERTA){
          if(mov==2){
            mode_night(true);
            FLAG_ADENTRO=true;
            if(CLIENTE){
            //REPRODUCIR AUDIO: DIRIGETE AL TOTEM PARA PRESENTARTE AL LOCAL...
            }
            Serial.println("Entro alguien a la esclusa pero cerramos la puerta");
          }
        }
      }
    }
  }
}

void rfid_externa_e(){
  CLIENTE=false;
  Serial.println("Esta por entrar un empleado");
}
void rfid_interna_e(){
  if(!FLAG_PUERTA_INTERNA && !FLAG_ABIERTO){
    if(FLAG_FLUJO){
      abrir_puerta_int();
      CLIENTE = true;
      Serial.println("Empleado abre puerta interior");
    }
    else{
      if(!MANTENER_CERRADA){
        pulse(PINOUT_OUTPUTS[11],false);
        pulse(PINOUT_OUTPUTS[10],false);
      
      }
    }
  }
}


void slave_receive(){
  if(READ_SLAVE=='A'){
    FLAG_ADENTRO = true;
    Serial.println("Recibo por RS hay alguien adentro de la esclusa");
  }
  else if(READ_SLAVE=='B'){
    FLAG_ADENTRO=false;
    Serial.println("Recibo por RS no hay nadie adentro de la esclusa");
  }
  else if(READ_SLAVE=='C'){
    FLAG_FLUJO=true;
    Serial.println("Recibo por RS que hay alguien entrando al local");
  }
  else if(READ_SLAVE=='D'){
    FLAG_FLUJO=false;
    Serial.println("Recibo por RS hay alguien saliendo del local");
  }
  else if(READ_SLAVE=='E'){
    FLAG_PUERTA_INTERNA = true;
    Serial.println("Recibo por RS que la puerta interna esta abierta");
  }
  else if(READ_SLAVE=='F'){
    FLAG_PUERTA_INTERNA = false;
    Serial.println("Recibo por RS que la puerta interna esta cerrada");
  }
  else{
    Serial.print("Recibi basura por RS...");
  }
}



/*--------------------------------- MODE ---------------------------------*/
//Que hago si tengo que poner un bajo?
//PRE
//POST
void mode_no_esclusa(){
  int i=0;
  digitalWrite(S7,HIGH);//Prendo el DPS si estoy en modo no esclusa
  Serial.println("Prendo DPS");
  //Destrabo la puerta mecanicamente si vengo desde el modo esclusa.
  pulse(PINOUT_OUTPUTS[11],false);
  Serial.println("Estoy en modo automatico");
  while(STATE==AUTOMATIC){
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("2"))requiere_averia();
      String prueba[]={"3","4","5","6","7","8"};
      while(i<9){
        if(command.equals(prueba[i])){
          pulse(PINOUT_OUTPUTS[i-2],false);
        }
        i++;
      }
    }
    check_peripherals();
    if(FLAG_ETH) ethernet();
    if(FLAG_BT) bluetooth();
    FLAG_ETH = FLAG_BT = false;
  }
  check_state();
}

void mode_panic(){
  Serial.println("Estoy en modo panico");
  LAST_STATE=STATE;
  STATE=PANIC;
  digitalWrite(S10,HIGH);
  Serial.write("6");
  while(STATE==PANIC){
    if(command.equals("12")){//recibo un lowww
      STATE=LAST_STATE;
      digitalWrite(S10,LOW);
    }
  }
  check_state();
}


void mode_only_leave(){
  int i=4;
  bool flag=true;
  while(flag){
    String prueba[]={"5","6","7","8"};
    while(i<9){
      if(command.equals(prueba[i-4])){
        pulse(PINOUT_OUTPUTS[i-2],false);
      }
      i++;
    }
    if(command.equals("9")){
      mode_night(true);
      flag=false;
    }
  }
  flag=true;
  while(flag){
    if(command.equals("9")){
      flag=false;
    }
  }
  STATE = AUTOMATIC;
  FLAG_ESCLUSA = false;
  check_state();
}

void mode_night(bool externa){
  if(externa)pulse(PINOUT_OUTPUTS[8],false);
}

/*---------*/
void check_peripherals(){
  //Check si llego algo por BT por RS o por ETH...
  if(Serial3.available()>0){
    READ_SLAVE = Serial3.read();
    FLAG_SLAVE = true;
  }
  //check bt
  if(Serial2.available()>0){
    READ_BT = Serial2.read();
    FLAG_BT = true;
    Serial.println("Entre a serial 2");
  }
}

void check_state(){
  
  if(STATE==AUTOMATIC){
    slave_send('2');
    mode_no_esclusa();
  }
  else if(STATE==PANIC){
    slave_send('6');
    mode_panic();
  }
  else if(STATE==ONLYLEAVE){
    slave_send('3');
    mode_only_leave();
  }
  else if(STATE==ESCLUSA){
    slave_send('7');
    mode_esclusa();
  }
}

void slave_send(char msg){
  Serial3.write(msg);
  Serial.println("Mando al RS: " + msg);
  bool aux=true;
    while(aux){
      if(Serial3.available()>0){
        if(Serial3.read()==msg){
          aux=false;
        }
        else{
          Serial3.write(msg);
        }
      }
  }
}
  // A que le damos prioridad si llegan las tres juntas el BT el ETH o un btn??
  void ethernet(){
    //READ_ETH=callback();//??? arreglar esto
    if(READ_ETH==ESCLUSA_ETH) FLAG_ESCLUSA = true;
    else if(READ_ETH==NOESCLUSA_ETH) FLAG_ESCLUSA = false;
    //  else if(READ_ETH<x STATE_ETH>y){}
    else Serial.println("RECIBI ESTA BASURA DEL BT " + READ_ETH);
    Serial.println("Entre a serial eth");
  }

  void bluetooth(){
    if(READ_BT==ESCLUSA_BT){
      FLAG_ESCLUSA = true;
      LAST_STATE = STATE;
      STATE = AUTOMATIC;
    }
    else if(READ_BT==NOESCLUSA_BT){
      FLAG_ESCLUSA = false;
      LAST_STATE = STATE;
      STATE = AUTOMATIC;
    }
    //  else if(READ_BT<x STATE_BT>y){}
    else Serial.println("RECIBI ESTA BASURA DEL BT " + READ_BT);
    Serial.println("Entre a serial bt");
  }

  //Recibo el flag en true si tengo que escribir un low, se hace solo para el caso del pin....
  void pulse(int pin, bool flag){
    if(!flag){
      digitalWrite(pin,HIGH);
      delay(1000);
      digitalWrite(pin,LOW);
    }
    else{
      digitalWrite(pin,LOW);
      delay(1000);
      digitalWrite(pin,HIGH);
    }
  }

  void requiere_averia(){
    Serial.println("Requiere averia");
    //reproducir un audio
  }

  void abrir_puerta_int(){
    //Mandamos por RS232 commando a slave para abrir la puerta.
    Serial3.write("9");
  }

  /*--------------------------------- ETHERNET ---------------------------------*/

  //This method is called whenever a MQTT message arrives. We must be prepared for any type of incoming message.
  //We are subscribed to RPC Calls: v1/devices/me/rpc/request/+
 /* int callback(char* topic, byte* payload, unsigned int length) {
    int ret;
    //log to console
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
    }
    Serial.println();
    char numero = payload[25];        // en caso de que el mensaje indique dispensar pastilla, este valor se refiere al numero del dispensador
    if (payload[11] == 'd' && payload[37] == 't') { //Se dispenso una pastilla
      // mando algo
    }
    return ret;
  }

  void reconnect() {
    // Loop until we're reconnected
    while (!client.connected()) {
      Serial.print("Attempting MQTT connection...");
      if (client.connect("AbuEstasBien", token, token )) {
        //if (client.connect("arduinoClient")) {
        Serial.println("connected");
        // Once connected, subscribe to rpc topic
        client.subscribe("v1/devices/me/rpc/request/+"); // suscripcion para recibir mensaje de ThngsBoard
        Serial.println("SUB");
      }
      else {
        Serial.println("failed, rc=");
        Serial.println(client.state());
        Serial.println(" try again in 1 second");
        // Wait 5 seconds before retrying
        delay(1000);
    }
  }
}
*/
void loop() {
    STATE = AUTOMATIC;
    LAST_STATE = AUTOMATIC;
    //Chequeo que la puerta este cerrada, sino la cierro.
    if(PINOUT_INPUTS[0]==LOW){
      mode_night(true);
    }
    //Me aseguro que cierre la puerta
    while(PINOUT_INPUTS[0]==LOW){
      delay(500);
      mode_night(true);
    }
    while(1){
      check_state();
      /*  i=0;
      if(digitalRead(PINOUT_INPUTS[i])==LOW)FLAG_ABIERTO=true;
      if(digitalRead(PINOUT_INPUTS[i])==HIGH)FLAG_ABIERTO=false;
      i++;
      if(digitalRead(PINOUT_INPUTS[i])==HIGH)requiere_averia();
      i++;
      j=0;
      while(i<9){
      if(digitalRead(PINOUT_INPUTS[i])==HIGH){
      INPUTS_SENSOR[j]=1;
      FLAG_SENSOR = true;//levanto flag para chequear que hago con el sensor.
    }
    j++;
    i++;
  }
  if(digitalRead(PINOUT_INPUTS[i])==HIGH)rfid_externa();
  i++;
  if(digitalRead(PINOUT_INPUTS[i])==HIGH)rfid_interna();
  */
  }
}
