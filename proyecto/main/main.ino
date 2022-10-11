/*ARCHIVO MAIN DEL PROYECTO*/
#include main.h

// Pines que bancan interrupts 2,3,21,20,19,18.
bit ins = [0,0,0,0,0,0,0,0,0,0,0,0]
boolean FLAG_ESCLUSA = false;

void isr_esclusa(){
  FLAG_ESCLUSA = true;
}
void setup() {
  // put your setup code here, to run once:
   Serial.begin(9600); 
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

void loop() {
  // put your main code here, to run repeatedly:
  
  //Pooling de entradas
  if(digitalRead(E1)==HIGH) ins[0]=1;
  if(digitalRead(E2)==HIGH) ins[1]=1;
  if(digitalRead(E3)==HIGH) ins[2]=1;
  if(digitalRead(E4)==HIGH) ins[3]=1;
  if(digitalRead(E5)==HIGH) ins[4]=1;
  if(digitalRead(E6)==HIGH) ins[5]=1;
  if(digitalRead(E7)==HIGH) ins[6]=1;
  if(digitalRead(E8)==HIGH) ins[7]=1;
  if(digitalRead(E9)==HIGH) ins[8]=1;
  if(digitalRead(E10)==HIGH) ins[9]=1;
  if(digitalRead(E11)==HIGH) ins[10]=1;
  if(digitalRead(E12)==HIGH) ins[11]=1;

  

}
