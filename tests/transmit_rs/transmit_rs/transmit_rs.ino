void setup(){ 
  Serial.begin(9600);
} 
 
void loop(){
  byte angulo=1;    
  Serial.write(angulo); //enviamos el ángulo correspondiente
  delay(500);                           
} 
