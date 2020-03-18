
/** \file	main.c
 *  \brief	Archivo que llama el inicio de todaslas maquinas de estado y cierra un bucle \n
 *	infinito lamando a cada una.
 */

#include "inicioUc.h"
#include "macros.h"


#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************
* Variables externas
*****************************************************************************/


/*****************************************************************************
* Main
*****************************************************************************/
/** \brief	Función de entrada del programa
 *
 *	Se inicializan todas las máquinas de estado, y se entra en un bucle infinito \n
 *	donde se ejecutan las mismas. \n
 *	Además se inician diferentes perisféricos del uc que no se encuentran en \n
 *	diferentes máquinas de estado.
 */

int main (void) {
	wdt_disable();					/* por las dudas deshabilito el watchdog timer para el inicio de todo el programa */
	cli();							/* Deshabilito las interrupciones globales */

	Inicio_Puertos ();
	Inicio_Timer0 ();				/* en modo CTC para que genere una interupción cada 1ms */
	Inicio_Buzzer();

	sei();							/* habilito las interrupciones globales */



	wdt_enable(WDTO_2S);			/* prendo el watchdog timer */

	while (1) {						/* ejecuto máquinas de estado */


		wdt_reset();
	}
}

/*****************************************************************************
* Interrupcion Timer 0
*****************************************************************************/
/** \brief	Rutina de interrupción del Timer 0.
 *
 *	Este timer genera una interrupción cada 1ms, en el se incrementan diferentes \n
 *	contadores de cada máquina de estados y se operan las valvulas servo o se modifican \n
 *	los tiempos de las PWM.
 */

ISR (TIMER0_COMP_vect) {




}

/*****************************************************************************
* Interrupcion por defecto
*****************************************************************************/
/** \brief	Vector de interrupción por defecto en caso de que no este la interrupción activada.
 *
 *	En caso de alguna falla en el codigo y se activa una interrupción no deseada, o sea \n
 *	que no tiene su vector con su rutina asociada, cae en este vector por defecto que no \n
 *	hace nada.
 */

ISR(__vector_default) {
}
