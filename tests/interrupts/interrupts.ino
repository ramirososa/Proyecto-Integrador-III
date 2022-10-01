volatile boolean prendido=false;
void Pulso(int timems)
{
 Serial.println("MandoPulso al 13");
 digitalWrite(13,HIGH);
 digitalWrite(10,HIGH);
 delay(timems);
 digitalWrite(13,LOW);
 digitalWrite(10,LOW);
}

void isr()
{
  if(!prendido){
  digitalWrite(12,HIGH);
  prendido=true;
  }
  else 
  {
  digitalWrite(12,LOW);
  prendido=false;
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(13,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(3,INPUT);
  const byte PinInterruptor = 3;
  attachInterrupt(digitalPinToInterrupt(PinInterruptor),isr, RISING); // ISR = INTERRUPT SERVICE ROUTINE (FUNCION ALA QUE VOY)
}

void loop() {
  Serial.println("Entro loop");
  Pulso(1000);
  delay(3000);
}
