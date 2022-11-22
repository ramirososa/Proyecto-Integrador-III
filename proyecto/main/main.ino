/*ARCHIVO MAIN DEL PROYECTO*/
#include "main.h"
#include <SPI.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>

//LISTA CODIGOS SLAVE
// Abrir puerta interior es 8.


const int PINOUT_INPUTS[] = {E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12};
const int PINOUT_OUTPUTS[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12};
int INPUTS_SENSOR[] = {0,0,0,0,0,0};

bool FLAG_ESCLUSA = false;
bool FLAG_STATE = false;
bool FLAG_SENSOR = false;
bool FLAG_BT = false;
bool FLAG_ETH = false;
bool FLAG_ABIERTO=false;
bool FLAG_SLAVE=false;

char ESCLUSA_BT;
char NOESCLUSA_BT;
char ESCLUSA_ETH;
char NOESCLUSA_ETH;
char READ_ETH;
char READ_BT;
char STATE;

//Texto
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
#define MSG_BUFFER_SIZE(50)
char msg[MSG_BUFFER_SIZE];


/*--------------------------------- INTERRUPCIONES ---------------------------------*/

void isr_esclusa(){
  FLAG_ESCLUSA = !FLAG_ESCLUSA; //Pongo el flag de esclusa prendida o apagada en el opuesto, por defecto empiezo el programa con esclusa apagada
  if(FLAG_ESCLUSA){
    STATE=ESCLUSA;
    digitalWrite(S7,LOW);//Apago el DPS si estoy en modo esclusa
  }
  else{
    STATE=AUTOMATIC;
    digitalWrite(S7,HIGH);//Prendo el DPS si estoy en modo no esclusa
  }
}

void isr_panico(){
  mode_panic();
}

void isr_abrir_interior(){
  abrir_puerta_int();
}

void isr_cierre(){
  //timer_cierre();//Empiezo timer del cierre de las puertas.cdo se cierra la puerta automatica se manda un modo noche..
  mode_only_leave();
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
  client.setCallback(callback);
  Serial.println("");
  Serial.println("Eth conectado");
  Serial.println("IP: ");
  Serial.println(Ethernet.localIP());
}



/*--------------------------------- MODE ---------------------------------*/
//Que hago si tengo que poner un bajo? en E1 por ejemplo..
//PRE
//POST
void mode_no_esclusa(){
  int i=0;
  Serial.write('2');
  while(STATE==AUTOMATIC){
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("1"))FLAG_ABIERTO=true;//Acordrase que esto llega al reves.
      if(command.equals("2"))requiere_averia();
      String prueba[]={"3","4","5","6","7","8"};
      while(i<9){
        if(command.equals(prueba[i])){
          pulse(PINOUT_OUTPUTS[i-2],false);
        }
        i++;
      }
      if(command.equals("9"))rfid_externa();//va?
      if(command.equals("10"))rfid_interna();//va?
      if(command.equals("11"))abrir_puerta_int();
    }
    check_peripherals();
    if(FLAG_ETH) ethernet();
    if(FLAG_BT) bluetooth();
    if(FLAG_SLAVE) slave_receive();
    FLAG_ETH = FLAG_BT = FLAG_SLAVE = FALSE;
  }
  check_state();
}

void mode_esclusa(){

}

void mode_panic(){
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
  i=4;
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
      mode_night();
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

void mode_night(){
  pulse(PINOUT_OUTPUTS[8],false);
}

/*---------*/
void check_peripherals(){
  //Check si llego algo por BT por RS o por ETH...

  //check bt
  if(Serial2.available()>0){
    READ_BT = Serial2.read();
    FLAG_BT = true;
    Serial.println("Entre a serial 2");
  }
}

void check_state(){
  LAST_STATE=STATE;
  switch(STATE){
    case AUTOMATIC:
      mode_no_esclusa();
    break;
    case PANIC:
      mode_panic();
    break;
    case ONLYLEAVE:
      mode_only_leave();
    break;
    case ESCLUSA:
      mode_esclusa();
    break;
  }

  // A que le damos prioridad si llegan las tres juntas el BT el ETH o un btn??
  void ethernet(){
    READ_ETH=callback;//??? arreglar esto
    if(READ_ETH==ESCLUSA_ETH) FLAG_ESCLUSA = true;
    else if(READ_ETH==NOESCLUSA_ETH) FLAG_ESCLUSA = false;
    //  else if(READ_ETH<x STATE_ETH>y){}
    else Serial.println("RECIBI ESTA BASURA DEL BT " + READ_ETH);
    Serial.println("Entre a serial eth");
  }

  void bluetooth(){
    if(READ_BT==ESCLUSA_BT) FLAG_ESCLUSA = true;
    else if(READ_BT==NOESCLUSA_BT) FLAG_ESCLUSA = false;
    //  else if(READ_BT<x STATE_BT>y){}
    else Serial.println("RECIBI ESTA BASURA DEL BT " + READ_BT);
    Serial.println("Entre a serial bt");
  }

  void slave_receive(){

  }

  //Recibo el flag en true si tengo que escribir un low, se hace solo para el caso del pin....
  void pulse(int pin, bool flag){
    if(!flag){
      digitalWrite(pin,HIGH);
      delay(3000);
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

  void rfid_externa(){
    Serial.println("control acceso externo");
    //pongo alfo aca_\??
  }

  void rfid_interna(){
    Serial.println("control acceso interno");
    abrir_puerta_int();
  }

  void abrir_puerta_int(){
    //Mandamos por RS232 commando a slave para abrir la puerta.
    Serial.write("8");
  }

  /*--------------------------------- ETHERNET ---------------------------------*/

  //This method is called whenever a MQTT message arrives. We must be prepared for any type of incoming message.
  //We are subscribed to RPC Calls: v1/devices/me/rpc/request/+
  int callback(char* topic, byte* payload, unsigned int length) {
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
      } else {
        Serial.println("failed, rc=");
        Serial.println(client.state());
        Serial.println(" try again in 1 second");
        // Wait 5 seconds before retrying
        delay(1000);
      }
    }
  }


  void loop() {
    int i;
    int j;
    STATE = AUTOMATIC;
    LAST_STATE = AUTOMATIC;
    //Chequeo que la puerta este cerrada, sino la cierro.
    while(1){
      mode_no_esclusa();
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
