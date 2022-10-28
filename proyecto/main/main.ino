/*ARCHIVO MAIN DEL PROYECTO*/
#include main.h

const int PINOUT_INPUTS = [E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12];
const int PINOUT_OUTPUTS = [S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12];
int INPUTS_ENABLE = [1,0,0,0,0,0,0,0,0,0,0,0];

// Hacer una estructura con por un lado el numero de pines y por otro lado si el pin se prendio.

bool FLAG_ESCLUSA = false;
bool FLAG_RECEIVE = false;
bool FLAG_SENSOR = false;
bool FLAG_BT = false;
bool FLAG_ETH = false;

char READ_ETH;
char READ_BT;
char STATE;

//Texto 
const char[] OFF = "APAGADA";
const char[] AUTOMATIC = "AUTOMATICA";
const char[] ONLYLEAVE = "SOLOSALIR";
const char[] OPEN = "ABIERTA";
const char[] NIGHT = "NOCHE";
const char[] PANIC = "PANICO";
const char[] ESCLUSA = "ESCLUSA"


void isr_esclusa(){
  FLAG_ESCLUSA = !FLAG_ESCLUSA; //Pongo el flag de esclusa prendida o apagada en el opuesto, por defecto empiezo el programa con esclusa apagada
}

void isr_cierre(){
  timer_cierre();//Empiezo timer del cierre de las puertas.
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
}

//Que hago si tengo que poner un bajo? en E1 por ejemplo..
//PRE
//POST
void mode_no_esclusa(){
  int i=0;
  if(FLAG_SENSOR) sensor();
  if(FLAG_STATE || FLAG_BT || FLAG_ETH) change_state();
}

void mode_esclusa(){
  
  
}
// A que le damos prioridad si llegan las tres juntas el BT el ETH o un btn??
void ethernet(){
  bool need_out=true
  int pinout;
    switch(STATE_ETH){
      case OFF:
        STATE = OFF;
        pinout = PINOUT_OUTPUTS[x];
      break;
      case AUTOMATIC:
        STATE = AUTOMATIC;
        pinout = PINOUT_OUTPUTS[x];
      break;
      case ONLYLEAVE:
        STATE = ONLYLEAVE;
        pinout = PINOUT_OUTPUTS[10];
      break;
      case OPEN:
        STATE = OPEN;
        pinout = PINOUT_OUTPUTS[9];
      break;
      case NIGHT:
        STATE = NIGHT;
        pinout = PINOUT_OUTPUTS[8];
      break;
      case PANIC:
        STATE = PANIC;
        pinout = PINOUT_OUTPUTS[7];
      break;
      case ESCLUSA:
        STATE = ESCLUSA;
        pinout = PINOUT_OUTPUTS[x];
        need_out=false;
      break;      
  output_cs(pinout, need_out);
}

void bluetooth(){
  if(STATE_BT==ESCLUSA_BT) FLAG_ESCLUSA = true;
  else if(STATE_BT==NOESCLUSA_BT) FLAG_ESCLUSA = false;
  else if(STATE_BT<x STATE_BT>y){}
  else Serial.println("RECIBI ESTA BASURA DEL BT " + STATE_BT);
}

//Recibo el flag en true si tengo que escribir un low, se hace solo para el caso del pin....
void pulse(int pin, bool flag){
  if(!flag){
    digitalWrite(pin,HIGH);
    delay(50);
    digitalWrite(pin,LOW);
  }
  else{
    digitalWrite(pin,LOW);
    delay(50);
    digitalWrite(pin,HIGH);
  }
}

void output_cs(int pin, bool flag){
  //PUBLISH DEL ESTADO por bt y eth, transmitir a slave??
  
}

void sensor(){
  for(int i=2; i<8; i++){
    if(INPUTS_ENABLE[i]==1) pulso(PINOUT_OUTPUTS[i-2],false);
  }
}




void loop() {
 
  INPUTS_ENABLE = [1,0,0,0,0,0,0,0,0,0,0,0];
  
  while(1){
/*----------------- Analisis de flancos -----------------*/
    int i=0;
    if(digitalRead(PINOUT_INPUTS[i])==LOW){
        INPUTS_ENABLE[i]=0;
        FLAG_RECEIVE=true;
        i++;
      }
    while(i<12){
      if(digitalRead(PINOUT_INPUTS[i])==HIGH){
        INPUTS_ENABLE[i]=1;
        FLAG_RECEIVE=true;
        i++;
        if(i>1 && i<8) FLAG_SENSOR = true;//levanto flag para chequear que hago con el sensor.
      }
    }
    if(Serial2.available()>0){
      READ_BT = Serial2.read();
      FLAG_BT = true;      
    }
    if(eth){
      READ_ETH = ;
      FLAG_ETH = ;
    }
    
/*----------------- Llamar funciones auxiliares -----------------*/
    if(FLAG_SENSOR){
      if(FLAG_ESCLUSA) mode_esclusa();
      else mode_noesclusa();
    }
    if(FLAG_ETH) ethernet();
    if(FLAG_BT) bluetooth();
    if()
  }
  

}
