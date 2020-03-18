
/** \file	inicioUc.h
 *  \brief	Contiene los prototipos correspondientes a las funciones de inicio de los perisfericos \n
 *	del uc. Asi como tambien a funciones comunes a diferentes maquinas de estado.
 */

#ifndef _BOTONES_H
#define _BOTONES_H

#include <avr/io.h>
//#include <stdbool.h>

typedef struct{
    char estado;
  	char boton_nro;
  	char timeout_debouncer;
} struct_botones;



// Para la máquina de estados de los botones
#define esperando_boton 	0
#define boton_en_deteccion 	1
#define boton_en_deteccion2 2
#define boton_detectado 	3
#define boton_atendido 		4

#define timeout_botones_ms  100

#define boton_NULL		0
#define boton_DISP		1
#define boton_RESET		2
#define boton_RUN_STOP	3
#define boton_INJ		4
#define boton_PARAM		5
#define boton_DOWN		6
#define boton_UP		7


void deteccion_botones (void);
void atender_botones (void);
void init_botones (void);

#endif /* _BOTONES_H */
