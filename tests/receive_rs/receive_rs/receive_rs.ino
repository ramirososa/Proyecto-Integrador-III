void setup() 
{ 
  Serial.begin(9600);  
  pinMode(13,OUTPUT);//inicializamos el pin del Led como salida
  pinMode(8,OUTPUT);
  digitalWrite(8,HIGH);
} 
/*
 boolean estado=false;
void loop() 
{ 
  Serial.write(1);
  delay(500);

} */

void loop(){
  if(Serial.available()){
    if(Serial.read()=='a')digitalWrite(8,LOW);
    else if(Serial.read()=='b')digitalWrite(8,HIGH);
  }

  digitalWrite(13,HIGH);
  Serial.write('b');
  delay(500);
  Serial.write('a');
  digitalWrite(13,LOW);
  delay(500);
}
