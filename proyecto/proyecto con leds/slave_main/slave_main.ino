//THIS IS THE HEADER OF THE PROJECT ALL DE DEFINITIONS WILL BE INCLUDED HERE
//ALL OF THE LIBRARIES
#include <string.h>

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
#define pinSerial 52
//ALL OF THE INPUTS
#define E1 1
#define E2 1
#define E3 1
#define E4 1
#define E5 1
#define E6 1
#define E7 1
#define E8 1

void check_state();
void abrir_puerta_int();
void requiere_averia();
void manejar_puerta(int);

void master_receive();
void mode_no_esclusa();
void mode_esclusa();
void mode_panic();

const int PINOUT_INPUTS[] = {
  E1,
  E2,
  E3,
  E4,
  E5,
  E6,
  E7,
  E8
};
const int PINOUT_OUTPUTS[] = {
  S1,
  S2,
  S3,
  S4,
  S5,
  S6,
  S7,
  S8,
  S9,
  S10,
  S11
};

bool FLAG_ESCLUSA = false;
bool FLAG_STATE = false;
bool FLAG_SENSOR = false;
bool FLAG_BT = false;
bool FLAG_ETH = false;
bool FLAG_ABIERTO = false;
bool FLAG_SLAVE = false;
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
char READ_SLAVE = '0';

//Texto
String STATE;
String OFF = "APAGADA"; //1
String AUTOMATIC = "AUTOMATICA"; //2
String ONLYLEAVE = "SOLOSALIR"; //3
String OPEN = "ABIERTA"; //4
String NIGHT = "NOCHE"; //5
String PANIC = "PANICO"; //6
String ESCLUSA = "ESCLUSA"; //7
String LAST_STATE;
String command;

/*--------------------------------- SETUP ---------------------------------*/
void setup() {
  Serial.begin(9600);
  Serial3.begin(9600);
  pinMode(E1, INPUT);
  pinMode(E2, INPUT);
  pinMode(E3, INPUT);
  pinMode(E4, INPUT);
  pinMode(E5, INPUT);
  pinMode(E6, INPUT);
  pinMode(E7, INPUT);
  pinMode(E8, INPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S4, OUTPUT);
  pinMode(S5, OUTPUT);
  pinMode(S6, OUTPUT);
  pinMode(S7, OUTPUT);
  pinMode(S8, OUTPUT);
  pinMode(S9, OUTPUT);
  pinMode(S10, OUTPUT);
  pinMode(S11, OUTPUT);
  pinMode(pinSerial, OUTPUT);
  digitalWrite(pinSerial, LOW);
}

/*--------------------------------- ESCLUSA ---------------------------------*/

void mode_esclusa() {
  //Aviso el cambio de modo por serial.
  //Destrabo la puerta mecanicamente con el siguiente pulso
  pulse(PINOUT_OUTPUTS[11], false);
  Serial.println("Estoy en modo esclusa");
  digitalWrite(S7, LOW); //Apago el DPS si estoy en modo esclusa
  Serial.println("Apago DPS");
  FLAG_ABIERTO = false;
  FLAG_PUERTA_INTERNA = false; //false esta cerrada, true esta abierta
  FLAG_FLUJO = true; //true = entrada , false = salida
  FLAG_ADENTRO = false;
  MANTENER_ABIERTA = false;
  MANTENER_CERRADA = false;
  CLIENTE = true;
  int i = 0;

  while (STATE == ESCLUSA) {
    if (Serial.available()) {
      command = Serial.readStringUntil('\n');
      i = 0;
      if (command.equals("1")) {
        FLAG_ABIERTO = true; //Acordrase que esto llega al reves.
        digitalWrite(pinSerial, HIGH);
        Serial3.write('E');
        delay(100);
        digitalWrite(pinSerial, LOW);
        Serial.println("PUERTA INTERIOR ABIERTA");
      }
      if (command.equals("1-")) {
        FLAG_ABIERTO = false; //Para avisar que mando el bajo
        digitalWrite(pinSerial, HIGH);
        Serial3.write('F');
        delay(150);
        digitalWrite(pinSerial, LOW);
        Serial.println("PUERTA INTERIOR CERRADA");
      }
      if (command.equals("2")) requiere_averia();

      if (command.equals("3")) manejar_puerta(1);
      if (command.equals("4")) MANTENER_ABIERTA = true;
      if (command.equals("4-")) MANTENER_ABIERTA = false;
      if (command.equals("5")) manejar_puerta(2);
      if (command.equals("6")) MANTENER_ABIERTA = true;
      if (command.equals("6-")) MANTENER_ABIERTA = false;
      if (command.equals("7")) MANTENER_CERRADA = true;
      if (command.equals("7-")) MANTENER_CERRADA = false;
      if (command.equals("8")) MANTENER_CERRADA = true;
      if (command.equals("8-")) MANTENER_CERRADA = false;
    }
    if (Serial3.available()) {
      READ_SLAVE = Serial3.read();
      master_receive();
    }
  }
}

void manejar_puerta(int mov) {
  //Puerta interna cerrada
  Serial.println("manejando puerta");
  Serial.println(FLAG_ADENTRO);
  Serial.println(FLAG_PUERTA_INTERNA);
  Serial.println(FLAG_FLUJO);
  Serial.println(FLAG_ABIERTO);
  Serial.println(MANTENER_ABIERTA);
  Serial.println(MANTENER_CERRADA);
  if (!FLAG_ABIERTO) {
    if (FLAG_ADENTRO) {
      if (!FLAG_FLUJO) {
        if (!FLAG_PUERTA_INTERNA) {
          if (!MANTENER_CERRADA) {
            if (mov == 1) {
              pulse(PINOUT_OUTPUTS[11], false);
              pulse(PINOUT_OUTPUTS[8], false);
              FLAG_FLUJO = true;
              Serial.println("Habia alguien en la esclusa y esta saliendo");
              //REPRODUCIR AUDIO DE ADIOS, VUELVE PRONTO!
            }
          }
        } else {
          if (!MANTENER_ABIERTA) {
            if (mov == 2) {
              mode_night(true);
              FLAG_ADENTRO = false;
              digitalWrite(pinSerial, HIGH);
              Serial3.write('D');
              delay(199);
              digitalWrite(pinSerial, LOW);
              Serial.println("Habia alguien en la esclusa y Cerro puerta exterior");
            }
          }
        }
      }
    } else {
      //Entrada NO VA
      if (!FLAG_ABIERTO) {
        if (!MANTENER_CERRADA) {
          if (mov == 2) {
            pulse(PINOUT_OUTPUTS[11], false);
            pulse(PINOUT_OUTPUTS[8], false);
            FLAG_FLUJO = false;
            digitalWrite(pinSerial, HIGH);
            delay(300);
            Serial3.write('D');
            delay(1000);
            digitalWrite(pinSerial, LOW);
            Serial.println("Alguien entra por puerta exterior");
            //REPRODUCIR AUDIO DE BIENVENIDA
          }
        }
      } else {
        if (!MANTENER_ABIERTA) {
          if (mov == 1) {
            mode_night(true);
            FLAG_ADENTRO = true;
            digitalWrite(pinSerial, HIGH);
            Serial3.write('A');
            delay(100);
            digitalWrite(pinSerial, LOW);
            if (CLIENTE) {
              //REPRODUCIR AUDIO: DIRIGETE AL TOTEM PARA PRESENTARTE AL LOCAL...
            }
            Serial.println("Entro alguien a la esclusa pero cerramos la puerta");
          }
        }
      }
    }
  } else {
    if (!MANTENER_ABIERTA && FLAG_ADENTRO) {
      if (mov == 2) {
        mode_night(true);
        FLAG_ADENTRO = false;
        digitalWrite(pinSerial, HIGH);
        Serial3.write('B');
        delay(100);
        digitalWrite(pinSerial, LOW);
        Serial.println("Habia alguien en la esclusa y Cerro puerta Interior, modo noche");
      }
    } else if (!MANTENER_ABIERTA && !FLAG_FLUJO) {
      if (mov == 1) {
        mode_night(true);
        FLAG_ADENTRO = true;
        digitalWrite(pinSerial, HIGH);
        Serial3.write('A');
        delay(100);
        digitalWrite(pinSerial, LOW);
        Serial.println("Entro alguien a la esclusa pero cerramos la puerta");
      }
    }
  }
}

void master_receive() {
  if (READ_SLAVE > '0' && READ_SLAVE < 'F') {
    Serial.println(READ_SLAVE);
    if (READ_SLAVE == '2') {
      LAST_STATE = STATE;
      STATE = AUTOMATIC;

    } else if (READ_SLAVE == '6') {
      LAST_STATE = STATE;
      STATE = PANIC;
    } else if (READ_SLAVE == '7') {
      LAST_STATE = STATE;
      STATE = ESCLUSA;

    } else if (READ_SLAVE == '9') {
      abrir_puerta_int();
      delay(30);
      Serial.println("llegue a 9");

    } else if (READ_SLAVE == '3') {
      STATE = ONLYLEAVE;
    } else if (READ_SLAVE == '4') {
      FLAG_PUERTA_INTERNA = true;
      Serial.println("Puerta exterior abierta");
    } else if (READ_SLAVE == '5') {
      FLAG_PUERTA_INTERNA = false;
      Serial.println("Puerta exterior cerrada");
    } else if (READ_SLAVE == 'A') {
      FLAG_ADENTRO = true;
      Serial.println("Recibo por RS adentro de la esclusa");
    } else if (READ_SLAVE == 'B') {
      FLAG_ADENTRO = false;
      Serial.println("Recibo por RS no hay nadie adentro de la esclusa");
    } else if (READ_SLAVE == 'C') {
      FLAG_FLUJO = true;
      Serial.println("Recibo por RS que hay alguien entrando al local");
    } else if (READ_SLAVE == 'D') {
      FLAG_FLUJO = false;
      Serial.println("Recibo por RS hay alguien saliendo del local");
    }
  }
}

/*--------------------------------- MODE ---------------------------------*/
//Que hago si tengo que poner un bajo?
//PRE
//POST
void mode_no_esclusa() {
  int i = 0;
  digitalWrite(S7, HIGH); //Prendo el DPS si estoy en modo no esclusa
  Serial.println("Prendo DPS");
  //Destrabo la puerta mecanicamente si vengo desde el modo esclusa.
  pulse(PINOUT_OUTPUTS[11], false);
  Serial.println("Estoy en modo automatico");
  while (STATE == AUTOMATIC) {
    if (Serial.available()) {
      command = Serial.readStringUntil('\n');
      i = 0;
      if (command.equals("2")) requiere_averia();
      String prueba[] = {
        "3",
        "4",
        "5",
        "6",
        "7",
        "8"
      };
      while (i < 9) {
        if (command.equals(prueba[i])) {
          pulse(PINOUT_OUTPUTS[i], false);
        }
        i++;
      }
    }
    if (Serial3.available()) {
      READ_SLAVE = Serial3.read();
      master_receive();
    }
  }
  check_state();
}

void mode_panic() {
  Serial.println("Estoy en modo panico");
  digitalWrite(S11, HIGH);
  while (STATE == PANIC) {
    if (Serial3.available()) {
      READ_SLAVE = Serial3.read();
      master_receive();
    }
  }
  digitalWrite(S11, LOW);
  check_state();
}

void mode_only_leave() {
  int i = 4;
  bool flag = true;
  digitalWrite(S10, HIGH);
  Serial.println("Modo solo salir");
  while (STATE == ONLYLEAVE) {
    if (Serial3.available()) {
      READ_SLAVE = Serial3.read();
      master_receive();
    }
  }
  digitalWrite(S10, LOW);
  check_state();
}

void mode_night(bool externa) {
  if (externa) pulse(PINOUT_OUTPUTS[7], false);
}

void check_state() {
  if (STATE == AUTOMATIC) {
    mode_no_esclusa();
  } else if (STATE == PANIC) {
    mode_panic();
  } else if (STATE == ONLYLEAVE) {
    mode_only_leave();
  } else if (STATE == ESCLUSA) {
    mode_esclusa();
  }
}

//Recibo el flag en true si tengo que escribir un low, se hace solo para el caso del pin....
void pulse(int pin, bool flag) {
  if (!flag) {
    digitalWrite(pin, HIGH);
    delay(1000);
    digitalWrite(pin, LOW);
  } else {
    digitalWrite(pin, LOW);
    delay(1000);
    digitalWrite(pin, HIGH);
  }
}

void requiere_averia() {
  Serial.println("Requiere averia");
  //reproducir un audio
}

void abrir_puerta_int() {
  //Mandamos por RS232 commando a slave para abrir la puerta.
  digitalWrite(pinSerial, HIGH);
  Serial3.write('9');
  delay(100);
  digitalWrite(pinSerial, LOW);
}

void loop() {
  STATE = AUTOMATIC;
  LAST_STATE = AUTOMATIC;
  //Chequeo que la puerta este cerrada, sino la cierro.
  if (PINOUT_INPUTS[0] == LOW) {
    mode_night(true);
  }
  while (1) {
    check_state();
  }
}
