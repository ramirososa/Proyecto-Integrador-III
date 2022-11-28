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

//ALL OF THE INPUTS
#define E1 1
#define E2 1
#define E3 1
#define E4 1
#define E5 1
#define E6 1
#define E7 1
#define E8 1

/*PINS REALES
//ALL OF THE OUTPUTS
#define S1 28
#define S2 24
#define S3 32
#define S4 37
#define S5 27
#define S6 29
#define S7 36
#define S8 33
#define S9 26
#define S10 40
#define S11 41

//ALL OF THE INPUTS
#define E1 22
#define E2 25
#define E3 23
#define E4 8
#define E5 4
#define E6 9
#define E7 5
#define E8 7
*/


void check_state();
void abrir_puerta_int();
void requiere_averia();
void manejar_puerta(int);

void master_receive();
void mode_no_esclusa();
void mode_esclusa();
void mode_panic();
