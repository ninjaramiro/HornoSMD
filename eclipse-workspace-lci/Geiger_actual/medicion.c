
/** \file	inicioUc.c
 *  \brief	Contiene las funciones de inicio de los perisfericos del uc. \n
 *	Asi como tambien funciones comunes a diferentes maquinas de estado.
 */

#include "inicioUc.h"
#include "macros.h"
#include "display.h"..

#include "lcd.h"


#include <avr/io.h>
#include <inttypes.h>
#include <avr/sleep.h>


extern volatile uint16_t u16_intervalo_s; // Variable que define el intervalo de conteo.
extern volatile uint16_t u16_intervalo; // Variable que define el intervalo de conteo.
extern volatile uint16_t u16_tiempo_s; // Variable que define el tiempo transcurrido, cuenta regresiva
extern volatile uint16_t u16_pulsos_en_intervalo; // Acumulacion de pulsos detectados
extern volatile uint16_t u16_CPM; // Variable donde se almacena el resultado de la medicion
extern volatile uint16_t u16_CPM_temp; // Variable donde se almacena el resultado de la medicion


void calculo_CPM (void) {	/* Funcion de inicio de los puertos de entrada/salida */

	float temp=10;
	unsigned int temp2;
	unsigned int temp3;
	temp3 = u16_intervalo_s;
	temp2 = u16_pulsos_en_intervalo;
	if (u16_tiempo_s == 0){

		temp = ((float)temp2/(float)temp3)*(float)60;
		u16_CPM = (unsigned int)temp;
		u16_CPM_temp = u16_pulsos_en_intervalo;
		u16_pulsos_en_intervalo = 0;
		u16_tiempo_s = u16_intervalo_s;
		LCDClear();
	}

}


