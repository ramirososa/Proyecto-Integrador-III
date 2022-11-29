#include <SPI.h>
#include <DFPlayer_Mini_Mp3.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>
#include <string.h>
//ALL OF THE OUTPUTS
#define S1 28
#define S2 24
#define S3 32
#define S4 37
#define S5 27
#define S6 29
#define S7 36
#define S8 33
#define S9 26
#define S10 40
#define S11 41
#define S12 39

//ALL OF THE INPUTS
#define E1 22
#define E2 25
#define E3 23
#define E4 8
#define E5 4
#define E6 9
#define E7 5
#define E8 7
#define E9 13
#define E10 12
#define E11 20
#define E12 2
#define P1 3 //Tiene que ser pin de interrupt.
#define P2 21 //Tiene que ser pin de interrupt

void check_state();
void abrir_puerta_int();
int callback(char*, byte*, unsigned int);
void requiere_averia();
void manejar_puerta(int);
void rfid_externa_e();
void rfid_interna_e();
void bluetooth();
void slave_receive();
void mode_no_esclusa();
void mode_esclusa();
void mode_panic();
void slave_send(char);
void pulse(char,bool);
void mode_night(bool);

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
char READ_SLAVE='0';
// ETHERNET

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,1,25);
IPAddress myDns(192,168,1,1);
IPAddress broker(192,168,17,106);

const char* mqtt_server = "demo.thingsboard.io";    // servidor de la aplicacion IoT
const char* token = "I5O29Aj1nla0WptymuX9";         // token de la aplicacion IoT

// Connection objects
EthernetClient ethClient;
PubSubClient client(ethClient);

//buffer sizes and messages
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
char msg2[MSG_BUFFER_SIZE];
char msg3[MSG_BUFFER_SIZE];


// FIN ETH

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
String STATE = AUTOMATIC;


void bluetooth(){
    if(READ_BT=='E'){
      FLAG_ESCLUSA = true;
      LAST_STATE = STATE;
      STATE = AUTOMATIC;
    }
    else if(READ_BT=='A'){
      FLAG_ESCLUSA = false;
      LAST_STATE = STATE;
      STATE = AUTOMATIC;
    }
    
      else if(READ_BT<'9' &&  READ_BT>'0'){}
    mp3_set_volume(parseInt(READ_BT));
  }


// ------------------ FUNCIONES WIFI -------------------- //

//This method is called whenever a MQTT message arrives. We must be prepared for any type of incoming message.
//We are subscribed to RPC Calls: v1/devices/me/rpc/request/+
void callback(char* topic, byte* payload, unsigned int length) {

  //log to console
  //Serial.print("Message arrived [");
  //Serial.print(topic);
  //Serial.print("] ");
  for (int i = 0; i < length; i++) {
    //Serial.print((char)payload[i]);
  }
  //Serial.println();
  if(payload[11]=='s') // llego un modo
  {
    char numero = payload[19];    // contiene el numero el del modo   noche = 1,auto = 2 , panico = 3 , esclusa = 4
  if(numero == '1')
  {// activo modo solo salir y le aviso al esclavo
    STATE=ONLYLEAVE;
    check_state();
  
  }
  if(numero == '2')
  {// activo modo automatico y le aviso al esclavo
    STATE=AUTOMATIC;
    check_state();
  }
  if(numero == '3')
  {// activo modo panico y le aviso al esclavo
    STATE=PANIC;
    check_state();
  }
  if(numero == '4')
  {// activo modo esclusa y le aviso al esclavo
    STATE=ESCLUSA;
    check_state();
  }
  }
  else if(payload[11]=='V'){
    // se cambia el volumen
    mp3_set_volume(parseInt(payload[19])*10+parseInt(payload[20]));
  }
    
    
}
}
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    //Serial.print("Attempting MQTT connection...");
    if (client.connect("AbuEstasBien", token, token )) {
      //if (client.connect("arduinoClient")) {
      //Serial.println("connected");
      // Once connected, subscribe to rpc topic
      client.subscribe("v1/devices/me/rpc/request/+"); // suscripcion para recibir mensaje de ThngsBoard
      //Serial.println("SUB");
    } else {
      //Serial.println("failed, rc=");
      //Serial.println(client.state());
      //Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(1000);
    }
  }
}




void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial1.begin(9600);
    Serial2.begin(9600);
    Serial3.begin(9600);
    pinMode(E1,INPUT);
    pinMode(E2,INPUT);
    pinMode(E3,INPUT);
    pinMode(E,INPUT);
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

    DFPlayerSerial3.begin(9600);
   mp3_set_serial(DFPlayerSerial);
   mp3_set_volume(15);
    pinMode(13,OUTPUT); // Pin para DE y RE
    digitalWrite(13,LOW);
    pinMode(53,OUTPUT);
    Ethernet.init(10);
    Ethernet.begin(mac,ip);
  // Allow the hardware to sort itself out
    delay(5000);
    ////Serial.println("Ethernet conectado sesu");
    client.setServer(mqtt_server, 1883); //client.setServer(server,1883)
    client.setCallback(callback);
  //Serial.println("");
  //Serial.println("Eth connected");
  //Serial.println("IP address: ");
  //Serial.println(Ethernet.localIP());
}


//--------------------------------- ESCLUSA ---------------------------------/
  void requiere_averia(){
    //Serial.println("Requiere averia");
    //reproducir un audio
    mp3_play(5);
   delay(6000);
  }

void abrir_puerta_int(){
  if(!FLAG_ABIERTO && FLAG_ADENTRO){
    slave_send('9');
  }
}

void rfid_externa_e(){
  CLIENTE=false;
  //Serial.println("Esta por entrar un empleado");
}
void rfid_interna_e(){
  if(!FLAG_PUERTA_INTERNA && !FLAG_ABIERTO){
    if(FLAG_FLUJO){
      abrir_puerta_int();
      CLIENTE = true;
      //Serial.println("Empleado abre puerta interior");
    }
    else{
      if(!MANTENER_CERRADA){
        pulse(PINOUT_OUTPUTS[11],false);
        pulse(PINOUT_OUTPUTS[10],false);
      
      }
    }
  }
}

void mode_esclusa(){
  //Aviso el cambio de modo por serial.
  //Destrabo la puerta mecanicamente con el siguiente pulso
  FLAG_ABIERTO=false;
  FLAG_PUERTA_INTERNA = false; //false esta cerrada, true esta abierta
  FLAG_FLUJO = true; //true = entrada , false = salida
  FLAG_ADENTRO = false;
  MANTENER_ABIERTA = false;
  MANTENER_CERRADA = false;
  CLIENTE = true;
  
  pulse(PINOUT_OUTPUTS[11],false);
  digitalWrite(S7,LOW);//Apago el DPS si estoy en modo esclusa
  //Serial.println("Apago DPS");
  int i=0;
  //Serial.println("Estoy en modo esclusa");
  while(STATE==ESCLUSA){
      i=0;
      if(digitalRead(PINOUT_INPUTS[0])==LOW){
        FLAG_ABIERTO=true;//Acordrase que esto llega al reves.
        slave_send('4');
        //Serial.println("PUERTA EXTERIOR ABIERTA");
      }
      if(digitalRead(PINOUT_INPUTS[0])==HIGH){
        FLAG_ABIERTO=false; //Para avisar que mando el bajo
        slave_send('5');
        //Serial.println("PUERTA EXTERIOR CERRADA");
      }
      
      if(digitalRead(PINOUT_INPUTS[1])==HIGH)requiere_averia();
      if(digitalRead(PINOUT_INPUTS[2])==HIGH)manejar_puerta(1);
      if(digitalRead(PINOUT_INPUTS[3])==HIGH || PINOUT_INPUTS[5]==HIGH) MANTENER_ABIERTA = true;
      if(digitalRead(PINOUT_INPUTS[3])==LOW || PINOUT_INPUTS[5]==LOW) MANTENER_ABIERTA = false;
      if(digitalRead(PINOUT_INPUTS[4])==HIGH) manejar_puerta(2);
      if(digitalRead(PINOUT_INPUTS[6])==HIGH || PINOUT_INPUTS[7]==HIGH) MANTENER_CERRADA = true;
      if(digitalRead(PINOUT_INPUTS[6])==LOW || PINOUT_INPUTS[7]==LOW) MANTENER_CERRADA = false;
      if(digitalRead(PINOUT_INPUTS[8])==HIGH) rfid_externa_e();
      if(digitalRead(PINOUT_INPUTS[9])==HIGH) rfid_interna_e();
      if(digitalRead(PINOUT_INPUTS[10])==HIGH) abrir_puerta_int();
      if(Serial.available())
      {
       READ_SLAVE=Serial.read();
       if(READ_SLAVE>0 && READ_SLAVE < 'G'){
        delay(100);
        slave_receive();
        delay(200);
       }
    }
    if (!client.connected()) {
           //Serial.print("no estoy conectado");
           reconnect();
    }
    client.loop();

    if(Serial2.available()>0)bluetooth();
  }
    check_state();
}

void manejar_puerta(int mov){
  //Puerta interna cerrada
  if(!FLAG_PUERTA_INTERNA){
    if(FLAG_ADENTRO){
      if(!FLAG_FLUJO){
        if(!FLAG_ABIERTO){
            if(!MANTENER_CERRADA){
              if(mov==2){
                pulse(PINOUT_OUTPUTS[11],false);
                pulse(PINOUT_OUTPUTS[9],false);
                FLAG_FLUJO=false;
                //Serial.println("Habia alguien en la esclusa y esta saliendo");
                mp3_play(5);
                delay(6000);
                //REPRODUCIR AUDIO DE ADIOS, VUELVE PRONTO!
              }
            }
          }
          else{
            if(!MANTENER_ABIERTA){
              if(mov==1){
                mode_night(true);
                FLAG_ADENTRO=false;
                slave_send('B');
                //Serial.println("Habia alguien en la esclusa y Cierro puerta exterior");
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
            pulse(PINOUT_OUTPUTS[9],false);
            FLAG_FLUJO=true;
            slave_send('C');
            //Serial.println("Alguien entra por puerta exterior");
            mp3_play(1);
            delay(6000);
            //REPRODUCIR AUDIO DE BIENVENIDA
          }
        }
      }
      else{
        if(!MANTENER_ABIERTA){
          if(mov==2){
            mode_night(true);
            FLAG_ADENTRO=true;
            slave_send('A');
            if(CLIENTE){
            //REPRODUCIR AUDIO: DIRIGETE AL TOTEM PARA PRESENTARTE AL LOCAL...
            mp3_play(2);
            delay(6000);
              //Serial.println("Cliente");
            }else{
              //Serial.println("Empleado");
            }
            //Serial.println("Entro alguien a la esclusa pero cerramos la puerta");
          }
        }
        else{
              //Serial.println("EL sensor de seguridad capto una persona, o podemos cerrar.");
            }
      }
    }
  }
}

void slave_receive(){

  //Serial.println(READ_SLAVE);
  
  if(READ_SLAVE=='A'){
    FLAG_ADENTRO = true;
    //Serial.println("Recibo por RS hay alguien adentro de la esclusa");
  }
  else if(READ_SLAVE=='B'){
    FLAG_ADENTRO=false;
    //Serial.println("Recibo por RS no hay nadie adentro de la esclusa");
  }
  else if(READ_SLAVE=='C'){
    FLAG_FLUJO=true;
    //Serial.println("Recibo por RS que hay alguien entrando al local");
  }
  else if(READ_SLAVE=='D'){
    FLAG_FLUJO=false;
    //Serial.println("Recibo por RS hay alguien saliendo del local");
  }
  else if(READ_SLAVE=='E'){
    FLAG_PUERTA_INTERNA = true;
    //Serial.println("Recibo por RS que la puerta interna esta abierta");
  }
  else if(READ_SLAVE=='F'){
    FLAG_PUERTA_INTERNA = false;
    //Serial.println("Recibo por RS que la puerta interna esta cerrada");
  }
}


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

void mode_no_esclusa(){
  int i=2;
  digitalWrite(7,HIGH);//Prendo el DPS si estoy en modo no esclusa
  //Serial.println("Prendo DPS");
  //Destrabo la puerta mecanicamente si vengo desde el modo esclusa.
  pulse(PINOUT_OUTPUTS[11],false);
  //Serial.println("Estoy en modo automatico");
  while(STATE==AUTOMATIC){
      i=2;
      if(digitalRead(PINOUT_INPUTS[1])==HIGH))requiere_averia();
      while(i<9){
        if(digitalRead(PINOUT_INPUTS[i])==HIGH)){
          pulse(PINOUT_OUTPUTS[i-2],false);
        }
        i++;
      }
    if (!client.connected()) {
           //Serial.print("no estoy conectado");
           reconnect();
    }
    client.loop();

    if(Serial2.available()>0)bluetooth();
  }
  check_state();
}




void mode_only_leave(){
  //Serial.println("Estoy en modo Solo Salir");

  bool flag=true;
  digitalWrite(S11,HIGH);
  while(flag){
    if(digitalRead(PINOUT_INPUTS[8])==LOW){
      mode_night(true);
      flag=false;
    }
  }
    flag=true;
    while(flag){
      if(digitalRead(PINOUT_INPUTS[8])==LOW){
        flag=false;
        pulse(PINOUT_OUTPUTS[11], false);
        pulse(PINOUT_OUTPUTS[9], false);
      }
    }
  digitalWrite(S11,LOW);  
  STATE = AUTOMATIC;
  FLAG_ESCLUSA = false;
  check_state();
}

void mode_panic(){
  //Serial.println("Estoy en modo panico");
  digitalWrite(S8,HIGH);
  //slave_send(6);
  while(STATE==PANIC){
    command = Serial.readStringUntil('\n');
    if(digitalRead(PINOUT_INPUTS[11])==LOW){//recibo un lowww
      STATE=LAST_STATE;
      digitalWrite(S8,LOW);
    }
  }
  check_state();
}


void mode_night(bool externa){
  if(externa)pulse(PINOUT_OUTPUTS[8],false);
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
  digitalWrite(13,HIGH);
  Serial.write(msg);
  delay(100);
  digitalWrite(13,LOW);
  //Serial.println(msg);
}


void loop() {
    
    STATE = AUTOMATIC;
    LAST_STATE = AUTOMATIC;
    //Chequeo que la puerta este cerrada, sino la cierro.
    if(PINOUT_INPUTS[0]==LOW){
      mode_night(true);
    }
    delay(10000);
    while(1){
      //Serial.println("void loop");
       if (!client.connected()) {
           //Serial.print("no estoy conectado");
           reconnect();
        }
      client.loop();
      check_state();
    }
}
