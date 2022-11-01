

void setup() {
  Serial.begin(9600);
  pinMode(2,OUTPUT);
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);
  pinMode(5,OUTPUT);
  pinMode(6,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(22,OUTPUT);
  
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
    
  }
}
