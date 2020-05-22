
/** \file	main.c 
 *  \brief	Archivo que llama el inicio de todas las maquinas de estado y cierra un bucle \n
 *	infinito llamando a cada una.
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "Blinky.h"

#include "macros.h"
#include "inicioUc.h"

#include "display.h"
#include "lcd.h"


/*****************************************************************************
* Variables externas
*****************************************************************************/
extern volatile unsigned int uiContador_Blinky; // Para el tiempo de conteo del blinky

extern volatile unsigned int uContDisplay; /* display.c, contador para la medicion de tiempos en el manejo del display */

extern volatile unsigned int Disp_index_fil;
extern volatile unsigned int Disp_index_col;


int main (void) {

	// Configuraciones iniciales

	//wdt_disable();				// por las dudas deshabilito el watchdog timer para el inicio de todo el programa
	cli();						// Deshabilito las interrupciones globales
	Inicio_Puertos ();
    Inicio_Timer0 ();
	blinky_init();
	Inicio_Display (); 			// El display es de 2x16
	//wdt_enable(WDTO_1S);		// prendo el watchdog timer
	sei();						// Habilito las interrupciones


    Test_Display();

	while (1) {  // Loop infinito

		blinky();
		Actualizar_Display ();
		//SET_BIT(BUZZER_PORT, BUZZER);

	}

return 0;
}


//ISR (TIMER0_COMP_vect) { // Esto seria si se utiliza un atmega16
ISR (SPI_STC_vect) //Para atmega 32
{
	if (uiContador_Blinky != 0){
		uiContador_Blinky--;
	}
	if (uContDisplay != 0){
			uContDisplay--;
	}
}
/*
ISR (SPI_STC_vect) {
	if (uContDisplay != 0){
		uContDisplay--;
	}
}
*/

/*****************************************************************************
* Interrupcion por defecto
*****************************************************************************/
/** \brief	Vector de interrupción por defecto en caso de que no este la interrupción activada.
 *
 *	En caso de alguna falla en el codigo y se activa una interrupción no deseada, o sea \n
 *	que no tiene su vector con su rutina asociada, cae en este vector por defecto que no \n
 *	hace nada.
 */

//ISR(__vector_default) {
//	LCDWriteStringXY(0,0,"Error INT");
//}



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
