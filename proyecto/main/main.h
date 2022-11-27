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
void rfid_externa_e();
void rfid_interna_e();
void check_peripherals();
void ethernet();
void bluetooth();
void slave_receive();
void mode_no_esclusa();
void mode_esclusa();
void mode_panic();


/*    switch(STATE_ETH){
      case OFF:
        STATE = OFF;
        pinout = PINOUT_OUTPUTS[x];
      break;
      case AUTOMATIC:
        STATE = AUTOMATIC;
        pinout = PINOUT_OUTPUTS[x];
      break;
      case ONLYLEAVE:
        STATE = ONLYLEAVE;
        pinout = PINOUT_OUTPUTS[10];
      break;
      case OPEN:
        STATE = OPEN;
        pinout = PINOUT_OUTPUTS[9];
      break;
      case NIGHT:
        STATE = NIGHT;
        pinout = PINOUT_OUTPUTS[8];
      break;
      case PANIC:
        STATE = PANIC;
        pinout = PINOUT_OUTPUTS[7];
      break;
      case ESCLUSA:
        STATE = ESCLUSA;
        pinout = PINOUT_OUTPUTS[x];
        need_out=false;
      break;   */
