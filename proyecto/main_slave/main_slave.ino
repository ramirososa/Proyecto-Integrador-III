#include "main_slave.h"

const int PINOUT_INPUTS[] = {E1,E2,E3,E4,E5,E6,E7,E8};
const int PINOUT_OUTPUTS[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11};

bool FLAG_ESCLUSA = false;
bool FLAG_STATE = false;
bool FLAG_SENSOR = false;
bool FLAG_BT = false;
bool FLAG_ETH = false;
bool FLAG_ABIERTO=false;
bool FLAG_SLAVE=false;
bool FLAG_PUERTA_INTERNA = false; //false esta cerrada, true esta abierta
bool FLAG_FLUJO = true; //true = entrada , false = salida
bool FLAG_ADENTRO = false;
bool MANTENER_ABIERTA = false;
bool MANTENER_CERRADA = false;
bool CLIENTE = true;

char ESCLUSA_BT;
char NOESCLUSA_BT;
char ESCLUSA_ETH;
char NOESCLUSA_ETH;
char READ_ETH;
char READ_BT;
char READ_SLAVE;


//Texto
String STATE;
String OFF = "APAGADA";  //1
String AUTOMATIC = "AUTOMATICA";  //2
String ONLYLEAVE = "SOLOSALIR"; //3
String OPEN = "ABIERTA";  //4
String NIGHT = "NOCHE"; //5
String PANIC = "PANICO";  //6
String ESCLUSA = "ESCLUSA"; //7
String LAST_STATE;
String command;


/*--------------------------------- SETUP ---------------------------------*/
void setup() {
  Serial.begin(9600);
  //Serial1.begin(9600);
 // Serial2.begin(9600);
  Serial3.begin(9600);
  pinMode(E1,INPUT);
  pinMode(E2,INPUT);
  pinMode(E3,INPUT);
  pinMode(E4,INPUT);
  pinMode(E5,INPUT);
  pinMode(E6,INPUT);
  pinMode(E7,INPUT);
  pinMode(E8,INPUT);
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

}

/*--------------------------------- ESCLUSA ---------------------------------*/

void mode_esclusa(){
  //Aviso el cambio de modo por serial.
  //Destrabo la puerta mecanicamente con el siguiente pulso
  //pulse(PINOUT_OUTPUTS[11],false);
  digitalWrite(S7,LOW);//Apago el DPS si estoy en modo esclusa
  Serial.println("Apago DPS");
  int i=0;
  
  while(STATE==ESCLUSA){
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("1")){
        FLAG_ABIERTO=true;//Acordrase que esto llega al reves.
        Serial3.write('E');
        Serial.println("PUERTA INTERIOR ABIERTA");
      }
      if(command.equals("1-")){
        FLAG_ABIERTO=false; //Para avisar que mando el bajo
        Serial3.write('F');
        Serial.println("PUERTA INTERIOR CERRADA");
      }
      if(command.equals("2"))requiere_averia();
      
      if(command.equals("3")) manejar_puerta(1);
      if(command.equals("4")) MANTENER_ABIERTA = true;
      if(command.equals("4-")) MANTENER_ABIERTA = false;
      if(command.equals("5")) manejar_puerta(2);
      if(command.equals("6")) MANTENER_ABIERTA = true;
      if(command.equals("6-")) MANTENER_ABIERTA = false;
      if(command.equals("7")) MANTENER_CERRADA = true;
      if(command.equals("7-")) MANTENER_CERRADA = false;
      if(command.equals("8")) MANTENER_CERRADA = true;
      if(command.equals("8-")) MANTENER_CERRADA = false;
      master_receive();
    }
  }
}

void manejar_puerta(int mov){
  //Puerta interna cerrada
  if(!FLAG_PUERTA_INTERNA){
    if(FLAG_ADENTRO){
      if(!FLAG_FLUJO){
        if(!FLAG_ABIERTO){
            if(!MANTENER_CERRADA){
              if(mov==1){
                pulse(PINOUT_OUTPUTS[11],false);
                pulse(PINOUT_OUTPUTS[10],false);
                FLAG_FLUJO=true;
                Serial.println("Habia alguien en la esclusa y esta saliendo");
                //REPRODUCIR AUDIO DE ADIOS, VUELVE PRONTO!
              }
            }
          }
          else{
            if(!MANTENER_ABIERTA){
              if(mov==2){
                mode_night(true);
                FLAG_ADENTRO=false;
                Serial3.write('B');
                Serial.println("Habia alguien en la esclusa y Cerro puerta exterior");
              }
            }
          }
      }
    }    
    else{
        //Entrada
      if(!FLAG_ABIERTO){
        if(!MANTENER_CERRADA){
          if(mov==1){
            pulse(PINOUT_OUTPUTS[11],false);
            pulse(PINOUT_OUTPUTS[10],false);
            FLAG_FLUJO=false;
            Serial3.write('D');
            Serial.println("Alguien entra por puerta exterior");
            //REPRODUCIR AUDIO DE BIENVENIDA
          }
        }
      }
      else{
        if(!MANTENER_ABIERTA){
          if(mov==2){
            mode_night(true);
            FLAG_ADENTRO=true;
            Serial3.write('A');
            if(CLIENTE){
            //REPRODUCIR AUDIO: DIRIGETE AL TOTEM PARA PRESENTARTE AL LOCAL...
            }
            Serial.println("Entro alguien a la esclusa pero cerramos la puerta");
          }
        }
      }
    }
  }
}

void master_receive(){
  if(READ_SLAVE=='2'){
    LAST_STATE=STATE;
    STATE=AUTOMATIC;
    Serial3.write('2');
  }
  else if(READ_SLAVE=='6'){
    LAST_STATE=STATE;
    STATE=PANIC;
    Serial3.write('6');
  }
  else if(READ_SLAVE=='A'){
    LAST_STATE=STATE;
    STATE=ESCLUSA;
    Serial3.write('7');
  }
  else if(READ_SLAVE=='9'){
    abrir_puerta_int();
    Serial3.write('9');
  }
  else if(READ_SLAVE=='4'){
    FLAG_PUERTA_INTERNA = true;
    Serial.println("Puerta exterior abierta");
  }
  else if(READ_SLAVE=='5'){
    FLAG_PUERTA_INTERNA = true;
    Serial.println("Puerta exterior cerrada");
  }
  
  else if(READ_SLAVE=='B'){
    FLAG_ADENTRO=false;
    Serial.println("Recibo por RS no hay nadie adentro de la esclusa");
  }
  else if(READ_SLAVE=='C'){
    FLAG_FLUJO=true;
    Serial.println("Recibo por RS que hay alguien entrando al local");
  }
  else if(READ_SLAVE=='D'){
    FLAG_FLUJO=false;
    Serial.println("Recibo por RS hay alguien saliendo del local");
  }
  else{
    Serial.print("Recibi basura por RS...");
  }
}



/*--------------------------------- MODE ---------------------------------*/
//Que hago si tengo que poner un bajo?
//PRE
//POST
void mode_no_esclusa(){
  int i=0;
  digitalWrite(S7,HIGH);//Prendo el DPS si estoy en modo no esclusa
  Serial.println("Prendo DPS");
  //Destrabo la puerta mecanicamente si vengo desde el modo esclusa.
  pulse(PINOUT_OUTPUTS[11],false);
  Serial.println("Estoy en modo automatico");
  while(STATE==AUTOMATIC){
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("2"))requiere_averia();
      String prueba[]={"3","4","5","6","7","8"};
      while(i<9){
        if(command.equals(prueba[i])){
          pulse(PINOUT_OUTPUTS[i-2],false);
        }
        i++;
      }
    }
    master_receive();
  }
  check_state();
}

void mode_panic(){
  Serial.println("Estoy en modo panico");
  digitalWrite(S11,HIGH);
  Serial.write("6");
  while(STATE==PANIC){
    master_receive();
  }
  digitalWrite(S11,LOW);
  check_state();
}


void mode_only_leave(){
  int i=4;
  bool flag=true;
  digitalWrite(S10,HIGH);
  while(STATE==ONLYLEAVE){
    master_receive();
  }
  digitalWrite(S10,LOW);
  check_state();
}

void mode_night(bool externa){
  if(externa)pulse(PINOUT_OUTPUTS[8],false);
}

void check_state(){
  if(STATE==AUTOMATIC){
    mode_no_esclusa();
  }
  else if(STATE==PANIC){
    mode_panic();
  }
  else if(STATE==ONLYLEAVE){
    mode_only_leave();
  }
  else if(STATE==ESCLUSA){
    mode_esclusa();
  }
}

  //Recibo el flag en true si tengo que escribir un low, se hace solo para el caso del pin....
  void pulse(int pin, bool flag){
    if(!flag){
      digitalWrite(pin,HIGH);
      delay(1000);
      digitalWrite(pin,LOW);
    }
    else{
      digitalWrite(pin,LOW);
      delay(1000);
      digitalWrite(pin,HIGH);
    }
  }

  void requiere_averia(){
    Serial.println("Requiere averia");
    //reproducir un audio
  }

  void abrir_puerta_int(){
    //Mandamos por RS232 commando a slave para abrir la puerta.
    Serial3.write("9");
  }

void loop() {
    STATE = AUTOMATIC;
    LAST_STATE = AUTOMATIC;
    //Chequeo que la puerta este cerrada, sino la cierro.
    if(PINOUT_INPUTS[0]==LOW){
      mode_night(true);
    }
    //Me aseguro que cierre la puerta
    /*while(PINOUT_INPUTS[0]==LOW){
      delay(500);
      mode_night(true);
    }*/
    while(1){
      check_state();
  }
}
