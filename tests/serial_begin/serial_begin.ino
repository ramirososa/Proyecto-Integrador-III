//Prueba iniciar comunicacion serial entre varios dispositivos.
//En nuestro caso tendriamos la TX para BT y la TX para comm serial..
//Mega tiene varios Serial begin por esa razom.

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  // read from port 0, send to port 1:
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial1.print(inByte, DEC);
  }
  // read from port 1, send to port 0:
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.print(inByte, DEC);
  }
}
