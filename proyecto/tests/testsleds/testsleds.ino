
String command;
void setup() {
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(22,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(12,OUTPUT);
}

void loop() {
  int j[] = {22,2,3,4,5,6,7,8,9,10,11,12};
  while(1){
  int i=0;  
  while(i<13){
    digitalWrite(j[i],HIGH);
    digitalWrite(j[i-1],LOW);
    delay(500);
    if(i==12)digitalWrite(22,LOW);
    i++;  
  }
  if(Serial.available()){
        command = Serial.readStringUntil('\n');
        if(command.equals("1")){
          digitalWrite(j[0],HIGH);
          delay(500);
          digitalWrite(j[0],LOW);
        }
        if(command.equals("2")){
          digitalWrite(j[1],HIGH);
          delay(500);
          digitalWrite(j[1],LOW);
        }
        if(command.equals("3")){
          digitalWrite(j[2],HIGH);
          delay(500);
          digitalWrite(j[2],LOW);
        }
   }
  }
}
