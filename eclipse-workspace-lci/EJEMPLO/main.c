
/** \file	main.c 
 *  \brief	Archivo que llama el inicio de todaslas maquinas de estado y cierra un bucle \n
 *	infinito lamando a cada una.
 */

#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "Blinky.h"
#include "Botones.h"

#include "macros.h"
#include "inicioUc.h"


/*****************************************************************************
* Variables externas
*****************************************************************************/
extern volatile unsigned int uiContador_Blinky; // Para el tiempo de conteo del blinky

int main (void) {

// Configuraciones iniciales

	cli();		// Deshabilito las interrupciones globales
	Inicio_Puertos ();
    Inicio_Timer0 ();
	blinky_init();
	botones_init();
    sei();	// Habilito las interrupciones


	while (1) {  // Loop infinito

		blinky();
		botones();

	}

return 0;
}


ISR (TIMER0_COMP_vect) {

	if (uiContador_Blinky != 0){
		uiContador_Blinky--;
	}
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
//	LCDWriteStringXY(0,0,"Error INT");
}



/*

 Vectores de interrupcion Atmega32

 ADC_vect -- ADC Conversion Complete
 ANA_COMP_vect -- Analog Comparator
 EE_RDY_vect -- EEPROM Ready
 INT0_vect -- External interrupt 0
 INT1_vect -- External interrupt 1
 INT2_vect -- External interrupt 2
 SPI_STC_vect -- Serial Transfer complete
 SPM_RDY_vect -- Store Program Memory Ready
 TIMER0_COMP_vect -- Timer/Counter0 Compare Match
 TIMER0_OVF_vect -- Timer/Counter0 Overflow
 TIMER1_CAPT_vect -- Timer/Counter Capture Event
 TIMER1_COMPA_vect -- Timer/Counter1 Compare Match A
 TIMER1_COMPB_vect -- Timer/Counter1 Compare MatchB
 TIMER1_OVF_vect -- Timer/Counter1 Overflow
 TIMER2_COMP_vect -- Timer/Counter2 Compare Match
 TIMER2_OVF_vect -- Timer/Counter2 Overflow
 TWI_vect  -- 2-wire Serial Interface
 USART_RXC_vect -- USART, Rx Complete
 USART_TXC_vect  -- USART, Tx Complete
 USART_UDRE_vect -- USART Data Register Empty
 */
