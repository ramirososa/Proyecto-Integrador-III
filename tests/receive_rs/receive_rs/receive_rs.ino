void setup() 
{ 
  Serial.begin(9600);  
  pinMode(13,OUTPUT);//inicializamos el pin del Led como salida
} 
 boolean estado=false;
void loop() 
{ 
  
  if (Serial.available()) {
    int angulo = Serial.read(); //Leemos el dato recibido 
    if(angulo==1&&!estado) //verificamos que sea un valor en el rango del servo
    {
      digitalWrite(13,HIGH);
      estado=true;
    }else if(angulo==1)
  {  digitalWrite(13,LOW);
  estado=false;
  }
  }

} 
