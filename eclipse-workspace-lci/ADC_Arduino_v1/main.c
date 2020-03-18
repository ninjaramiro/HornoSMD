// http://extremeelectronics.co.in/avr-tutorials/interfacing-12-bit-spi-adc-mcp3204-with-avr-micro/

/** \file	main.c 
 *  \brief	Archivo que llama el inicio de todaslas maquinas de estado y cierra un bucle \n
 *	infinito lamando a cada una.
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <util/delay.h>

#include "macros.h"
#include "inicioUc.h"
#include "USART.h"
//#include "medicion_ADC.h"

#include "ADC.h"

/*****************************************************************************
* Variables externas
*****************************************************************************/

volatile uint16_t result; //Para el resultado del ADC
uint8_t ADC_L;
uint8_t ADC_H;

int main (void) {

//	wdt_disable();					/* por las dudas deshabilito el watchdog timer para el inicio de todo el programa */
	cli();							/* Deshabilito las interrupciones globales */
//	TIFR0=0; //Borro los flags de interrupcion de los timers

	Inicio_Puertos ();
	Inicio_USART (0);


	Inicio_Interrupcion0();

	InitADCEx();


	char modo=0;

	CLEAR_BIT(DIR_LED_PORT, LED); //Apago LED
//	CLEAR_BIT(DIR_LED_PORT, LED_FRENTE); //Apago LED

	cli();	// Deshabilito las interrupciones
	USART_Flush();

while(1){


	if ((PIND & (SET<<PD6)) == 0) {
		if (modo==0){
			modo = 1; // capturando
			SET_BIT(DIR_LED_PORT, LED); //Enciendo LED
//			SET_BIT(DIR_LED_PORT, LED_FRENTE); //ENCIENDO LED

	        USART_Transmit(','); // Separador
		    USART_Transmit(','); // Separador
			sei();	// Habilito las interrupciones
		}
	}

	if ((PIND & (SET<<PD7)) == 0) {
		modo = 0; // detenido
		CLEAR_BIT(DIR_LED_PORT, LED); //Apago LED
//		CLEAR_BIT(DIR_LED_PORT, LED_FRENTE); //Apago LED

		cli();	// Deshabilito las interrupciones
	}



}



return 0;
}




// Interrupcion externa //
ISR(INT0_vect){

	      result=ReadADC_7380();
	  	  //transmito los dos bytes
	      USART_Transmit((uint8_t)(result >> 8));   // Byte high
	      USART_Transmit((uint8_t)(result & 0xFF)); // Byte low
	      USART_Transmit(','); // Separador

//	      USART_Transmit('G');   // Byte high
//	      USART_Transmit('O'); // Byte low
//	      USART_Transmit(','); // Separador

//
//    result=ReadADC_7380();
//	  //transmito los dos bytes
//    USART_Transmit((uint8_t)(result >> 8));   // Byte high
//    USART_Transmit((uint8_t)(result & 0xFF)); // Byte low
//    USART_Transmit(','); // Separador
//
//
////    USART_Transmit('F'); // INICIO
////    USART_Transmit('F'); // INICIO
////    USART_Transmit(','); // Separador
//
////		LCDWriteStringXY(0,0,"INT");
//
//
}



//ISR (TIMER0_COMP_vect) {
//	TCNT0 = 0;  // Reseteo el contador
//
////reti();
//}



ISR(__vector_default) {

}


