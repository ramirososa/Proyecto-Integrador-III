//ALL OF THE OUTPUTS
#define S1 22
#define S2 2
#define S3 3
#define S4 4
#define S5 5
#define S6 6
#define S7 7
#define S8 8
#define S9 9
#define S10 10
#define S11 11
#define S12 12

//ALL OF THE INPUTS
#define E1 1
#define E2 1
#define E3 1
#define E4 1
#define E5 1
#define E6 1
#define E7 1
#define E8 1
#define E9 1
#define E10 1
#define E11 1
#define E12 1
#define P1 1 //Tiene que ser pin de interrupt.
#define P2 1 //Tiene que ser pin de interrupt

void check_state();
void abrir_puerta_int();
//int callback(char*, byte*, unsigned int);
void requiere_averia();
void manejar_puerta(int);
//void rfid_externa_e();
//void rfid_interna_e();
//void check_peripherals();
void ethernet();
void bluetooth();
//void slave_receive();
void mode_no_esclusa();
void mode_esclusa();
void mode_panic();
void slave_send(char);
void pulse(char,bool);
void mode_night(bool);

const int PINOUT_INPUTS[] = {E1,E2,E3,E4,E5,E6,E7,E8,E9,E10,E11,E12};
const int PINOUT_OUTPUTS[] = {S1,S2,S3,S4,S5,S6,S7,S8,S9,S10,S11,S12};

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
char READ_SLAVE='0';


//Texto

String OFF = "APAGADA";  //1
String AUTOMATIC = "AUTOMATICA";  //2
String ONLYLEAVE = "SOLOSALIR"; //3
String OPEN = "ABIERTA";  //4
String NIGHT = "NOCHE"; //5
String PANIC = "PANICO";  //6
String ESCLUSA = "ESCLUSA"; //7
String LAST_STATE;
String command;
String STATE = AUTOMATIC;
void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial3.begin(9600);
    pinMode(13,OUTPUT);
    digitalWrite(13,LOW);
}


/--------------------------------- ESCLUSA ---------------------------------/
  void requiere_averia(){
    Serial.println("Requiere averia");
    //reproducir un audio
  }

void abrir_puerta_int(){
  if(!FLAG_ABIERTO && FLAG_ADENTRO){
    slave_send('9');
  }
}

void rfid_externa_e(){
  CLIENTE=false;
  Serial.println("Esta por entrar un empleado");
}
void rfid_interna_e(){
  if(!FLAG_PUERTA_INTERNA && !FLAG_ABIERTO){
    if(FLAG_FLUJO){
      abrir_puerta_int();
      CLIENTE = true;
      Serial.println("Empleado abre puerta interior");
    }
    else{
      if(!MANTENER_CERRADA){
        pulse(PINOUT_OUTPUTS[11],false);
        pulse(PINOUT_OUTPUTS[10],false);
      
      }
    }
  }
}

void mode_esclusa(){
  //Aviso el cambio de modo por serial.
  //Destrabo la puerta mecanicamente con el siguiente pulso
  FLAG_ABIERTO=false;
  FLAG_PUERTA_INTERNA = false; //false esta cerrada, true esta abierta
  FLAG_FLUJO = true; //true = entrada , false = salida
  FLAG_ADENTRO = false;
  MANTENER_ABIERTA = false;
  MANTENER_CERRADA = false;
  CLIENTE = true;
  
  pulse(PINOUT_OUTPUTS[11],false);
  digitalWrite(S7,LOW);//Apago el DPS si estoy en modo esclusa
  Serial.println("Apago DPS");
  int i=0;
  Serial.println("Estoy en modo esclusa");
  while(STATE==ESCLUSA){
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
      if(command.equals("1")){
        FLAG_ABIERTO=true;//Acordrase que esto llega al reves.
        slave_send('4');
        Serial.println("PUERTA EXTERIOR ABIERTA");
      }
      if(command.equals("1-")){
        FLAG_ABIERTO=false; //Para avisar que mando el bajo
        slave_send('5');
        Serial.println("PUERTA EXTERIOR CERRADA");
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
      
      if(command.equals("9"))rfid_externa_e();
      if(command.equals("10"))rfid_interna_e();

      if(command.equals("ol"))STATE=ONLYLEAVE;
      if(command.equals("pa"))STATE=PANIC;
      if(command.equals("e"))STATE=ESCLUSA;
      if(command.equals("a"))STATE=AUTOMATIC;
      if(command.equals("11"))abrir_puerta_int();
      }
      
      if(Serial3.available())
      {
       READ_SLAVE=Serial3.read();
       if(READ_SLAVE>0 && READ_SLAVE < 'G'){
        delay(100);
        slave_receive();
        delay(200);
       }
       
    }
  }
    /*check_peripherals();
    /*if(FLAG_ETH) ethernet();
    if(FLAG_BT) bluetooth();
    if(FLAG_SLAVE) slave_receive();
    FLAG_ETH = FLAG_BT = FLAG_SLAVE = false;*/
   check_state();
}

void manejar_puerta(int mov){
  //Puerta interna cerrada
  if(!FLAG_PUERTA_INTERNA){
    if(FLAG_ADENTRO){
      if(!FLAG_FLUJO){
        if(!FLAG_ABIERTO){
            if(!MANTENER_CERRADA){
              if(mov==2){
                pulse(PINOUT_OUTPUTS[11],false);
                pulse(PINOUT_OUTPUTS[9],false);
                FLAG_FLUJO=false;
                Serial.println("Habia alguien en la esclusa y esta saliendo");
                //REPRODUCIR AUDIO DE ADIOS, VUELVE PRONTO!
              }
            }
          }
          else{
            if(!MANTENER_ABIERTA){
              if(mov==1){
                mode_night(true);
                FLAG_ADENTRO=false;
                slave_send('B');
                Serial.println("Habia alguien en la esclusa y Cierro puerta exterior");
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
            pulse(PINOUT_OUTPUTS[9],false);
            FLAG_FLUJO=true;
            slave_send('C');
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
            slave_send('A');
            if(CLIENTE){
            //REPRODUCIR AUDIO: DIRIGETE AL TOTEM PARA PRESENTARTE AL LOCAL...
              Serial.println("Cliente");
            }else{
              Serial.println("Empleado");
            }
            Serial.println("Entro alguien a la esclusa pero cerramos la puerta");
          }
        }
        else{
              Serial.println("EL sensor de seguridad capto una persona, o podemos cerrar.");
            }
      }
    }
  }
}

void slave_receive(){

  Serial.println(READ_SLAVE);
  
  if(READ_SLAVE=='A'){
    FLAG_ADENTRO = true;
    Serial.println("Recibo por RS hay alguien adentro de la esclusa");
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
  else if(READ_SLAVE=='E'){
    FLAG_PUERTA_INTERNA = true;
    Serial.println("Recibo por RS que la puerta interna esta abierta");
  }
  else if(READ_SLAVE=='F'){
    FLAG_PUERTA_INTERNA = false;
    Serial.println("Recibo por RS que la puerta interna esta cerrada");
  }
}


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

void mode_no_esclusa(){
  int i=0;
  digitalWrite(7,HIGH);//Prendo el DPS si estoy en modo no esclusa
  Serial.println("Prendo DPS");
  //Destrabo la puerta mecanicamente si vengo desde el modo esclusa.
  //pulse(PINOUT_OUTPUTS[11],false);
  Serial.println("Estoy en modo automatico");
  while(STATE==AUTOMATIC){
    if(Serial.available()){
      command = Serial.readStringUntil('\n');
      i=0;
    //if(command.equals("2"))requiere_averia();
      String prueba[]={"3","4","5","6","7","8"};
      while(i<9){
        if(command.equals(prueba[i])){
         pulse(PINOUT_OUTPUTS[i],false);
        }
        i++;
      }
      if(command.equals("ol"))STATE=ONLYLEAVE;
      if(command.equals("pa"))STATE=PANIC;
      if(command.equals("e"))STATE=ESCLUSA;
      if(command.equals("a"))STATE=AUTOMATIC;
      
    }
    
    /*check_peripherals();
    if(FLAG_ETH) ethernet();
    if(FLAG_BT) bluetooth();
    FLAG_ETH = FLAG_BT = false;*/
  }
  check_state();
}




void mode_only_leave(){
  Serial.println("Estoy en modo Solo Salir");
  int i=4;
  bool flag=true;
  digitalWrite(S11,HIGH);
  while(flag){
    command = Serial.readStringUntil('\n');
    if(command.equals("9")){
      mode_night(true);
      flag=false;
    }
  }
    flag=true;
    while(flag){
      command = Serial.readStringUntil('\n');
      if(command.equals("9")){
        flag=false;
      }
    }
  digitalWrite(S11,LOW);  
  STATE = AUTOMATIC;
  FLAG_ESCLUSA = false;
  check_state();
}

void mode_panic(){
  Serial.println("Estoy en modo panico");
  digitalWrite(S10,HIGH);
  //slave_send(6);
  while(STATE==PANIC){
    command = Serial.readStringUntil('\n');
    if(command.equals("12")){//recibo un lowww
      STATE=LAST_STATE;
      digitalWrite(S10,LOW);
    }
    if(command.equals("ol"))STATE=ONLYLEAVE;
    if(command.equals("pa"))STATE=PANIC;
    if(command.equals("e"))STATE=ESCLUSA;
    if(command.equals("a"))STATE=AUTOMATIC;
  }
  check_state();
}


void mode_night(bool externa){
  if(externa)pulse(PINOUT_OUTPUTS[8],false);
}




void check_state(){
  
  if(STATE==AUTOMATIC){
    slave_send('2');
    mode_no_esclusa();
  }
  else if(STATE==PANIC){
    slave_send('6');
    mode_panic();
  }
  else if(STATE==ONLYLEAVE){
    slave_send('3');
    mode_only_leave();
  }
  else if(STATE==ESCLUSA){
    slave_send('7');
    mode_esclusa();
  }
}

void slave_send(char msg){
  digitalWrite(13,HIGH);
  Serial3.write(msg);
  delay(100);
  digitalWrite(13,LOW);
  Serial.println(msg);
}


void loop() {
    STATE = AUTOMATIC;
    LAST_STATE = AUTOMATIC;
    //Chequeo que la puerta este cerrada, sino la cierro.
   /*if(PINOUT_INPUTS[0]==LOW){
      mode_night(true);
    }
    //Me aseguro que cierre la puerta
    /*while(PINOUT_INPUTS[0]==LOW){
      delay(500);
      mode_night(true);
    }*/
    delay(10000);
    while(1){
      Serial.println("void loop");
      check_state();
    }
}
