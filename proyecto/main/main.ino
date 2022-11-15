/*ARCHIVO MAIN DEL PROYECTO*/
#include "main.h"

//LISTA CODIGOS SLAVE
// Abrir puerta interior es 8.


const int PINOUT_INPUTS[] = {E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12};
const int PINOUT_OUTPUTS[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12};
int INPUTS_SENSOR[] = {0,0,0,0,0,0};

// Hacer una estructura con por un lado el numero de pines y por otro lado si el pin se prendio.

bool FLAG_ESCLUSA = false;
bool FLAG_STATE = false;
bool FLAG_SENSOR = false;
bool FLAG_BT = false;
bool FLAG_ETH = false;
bool FLAG_ABIERTO=false;


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


//Hay que hacer lista de codigos para mandar por elrs al slave con cada modo y si abrimos la puerta.
void abrir_puerta_int(){
  //Mandamos por RS232 commando a slave para abrir la puerta.
  Serial.write("8");
}

void isr_panico(){
  LAST_STATE=STATE;
  STATE=PANIC;
  digitalWrite(S10,HIGH);
  Serial.write("6");
  //Mandamos por RS a slave que mande salida S1 prendida indef. chequear como se apaga modo panico.
}

void isr_cierre(){
  //timer_cierre();//Empiezo timer del cierre de las puertas.cdo se cierra la puerta automatica se manda un modo noche..
}

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
}

//Que hago si tengo que poner un bajo? en E1 por ejemplo..
//PRE
//POST
void mode_no_esclusa(){
  int i=0;

    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("1"))FLAG_ABIERTO=true;
      if(command.equals("2"))requiere_averia();
      String prueba[]={"3","4","5","6","7","8"};
      while(i<9){
        if(command.equals(prueba[i])){
          pulse(PINOUT_OUTPUTS[i-2],false);
        }
        i++;
      }
      if(command.equals("9"))rfid_externa();
      if(command.equals("10"))rfid_interna();
      if(command.equals("11"))abrir_puerta_int();
      if(STATE==PANIC){
        if(command.equals("12")){//recibo un lowww
          STATE=LAST_STATE;
          digitalWrite(S10,LOW);
        }
      }
    }


  
  if(FLAG_SENSOR) sensor();
  if(FLAG_STATE || FLAG_BT || FLAG_ETH) change_state();
}

void change_state(){
  
}

void mode_esclusa(){
  
  
}
// A que le damos prioridad si llegan las tres juntas el BT el ETH o un btn??
void ethernet(){
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

//Recibo el flag en true si tengo que escribir un low, se hace solo para el caso del pin....
void pulse(int pin, bool flag){
  if(!flag){
    c
    delay(3000);
    digitalWrite(pin,LOW);
  }
  else{
    digitalWrite(pin,LOW);
    delay(1000);
    digitalWrite(pin,HIGH);
  }
}

void sensor(){
  for(int i=0; i<6; i++){
    if(INPUTS_SENSOR[i]==1) pulse(PINOUT_OUTPUTS[i],false);//El problema que tenemos con este array es que sillegan 2 juntos en el mismo pull se va a quedar con el ultimo en la lista y no el ultimo que registra.
  }
  FLAG_SENSOR=false;
}

void requiere_averia(){
        Serial.println("Requiere averia");
        //reproducir un audio
}

void rfid_externa(){
        Serial.println("control ecceso externo");
        if(STATE==NIGHT){
          STATE=AUTOMATIC;
          FLAG_ESCLUSA=false;//si estoy en modo noche, y leo el tag de afuera, pongo las puertas en modo automatico por defecto y entro,o en modo esclusa?
        }
}

void rfid_interna(){
        Serial.println("control acceso interno");
}


void loop() {
  int i;
  int j;
  while(1){
    for(int f=0;f<6;f++)INPUTS_SENSOR[f]=0;
    i=0;
/*----------------- Analisis de flancos -----------------*/
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
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("1"))FLAG_ABIERTO=true;
      if(command.equals("2"))requiere_averia();
      j=0;
      String prueba[]={"3","4","5","6","7","8"};
      while(i<9){
        if(command.equals(prueba[i])){
          INPUTS_SENSOR[j]=1;
          FLAG_SENSOR = true;//levanto flag para chequear que hago con el sensor.
        }
        j++;
        i++;
      }
      if(command.equals("9"))rfid_externa();
      if(command.equals("10"))rfid_interna();
      if(STATE==PANIC){
        if(command.equals("12")){//recibo un lowww
          STATE=LAST_STATE;
          digitalWrite(S10,LOW);
        }
      }
    }
/*----------------- Analisis Perifericos -----------------*/
    if(Serial2.available()>0){
      READ_BT = Serial2.read();
      FLAG_BT = true;
      Serial.println("Entre a serial 2");      
    }
    /*if(eth){
      //READ_ETH = ;
      //FLAG_ETH = ;
    }*/
    
/*----------------- Llamar funciones auxiliares -----------------*/
    if(FLAG_SENSOR){
      if(FLAG_ESCLUSA) mode_esclusa();
      else mode_no_esclusa();
    }
    if(FLAG_ETH) ethernet();
    if(FLAG_BT) bluetooth();
  }
}
