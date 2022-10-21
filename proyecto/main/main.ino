/*ARCHIVO MAIN DEL PROYECTO*/
#include main.h

const int PINOUT_INPUTS = [E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12];
const int PINOUT_OUTPUTS = [S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12];
int INPUTS_ENABLE = [];
// Hacer una estructura con por un lado el numero de pines y por otro lado si el pin se prendio.

boolean FLAG_ESCLUSA = false;
boolean FLAG_RECEIVE = false;
boolean FLAG_BT = false;
char STATE_BT;

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
  while(i<(sizeof(INPUTS_ENABLE)/sizeof(int)){
    digitalWrite(OUTPUTS_ENABLE[i],HIGH)    //WARNING! revisar esto, no me importa saber que pines se prendiero, quiero saber cuales voy a prenderrr
  }
}

void mode_esclusa(){
  
  
}

void loop() {
  
  INPUTS_ENABLE = [];
  
  while(1){
/*----------------- Analisis de flancos -----------------*/
    int j=0;
    if(digitalRead(PINOUT_INPUTS[0])==LOW){
        ins[j]=PINOUT_INPUTS[0];
        FLAG_RECEIVE=true;
        j++;
      }
    int i=1;
    while(i<12){
      if(digitalRead(PINOUT_INPUTS[i])==HIGH){
        ins[j]=PINOUT_INPUTS[i];
        FLAG_RECEIVE=true;
        j++;
      }
    }
    if(Serial1.available()>0){
      STATE_BT = Serial1.read();        
    }
/*----------------- Llamar funciones auxiliares -----------------*/
    if(!FLAG_ESCLUSA && FLAG_RECEIVE) mode_no_esclusa();
    else if(FLAG_ESCLUSA && FLAG_RECEIVE) mode_esclusa();
    if(STATE_BT!=0){
      read_BT()
    }
  }
  

}
