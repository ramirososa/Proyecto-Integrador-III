// Este ejemplo bajado de mechatronics apaga y enciende un led si le llega por el serial del bluetooth al arduino el estado, 0 o 1.



#define ledPin 7
int state = 0;

void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  Serial.begin(9600);
  Serial1.begin(9600); 
}

void loop() {
  if(Serial1.available() > 0){ // Checks whether data is comming from the serial port
    state = Serial1.read(); // Reads the data from the serial port
 
 }
 Serial.println(state);
 delay(1000);

 if (state == '0') {
  digitalWrite(ledPin, LOW); // Turn LED OFF
  Serial.println("LED: OFF"); // Send back, to the phone, the String "LED: ON"
  state = 0;
 }
 else if (state > '0') {
  digitalWrite(ledPin, HIGH);
  Serial.println("LED: ON");;
  state = 0;
 }
}
