// http://extremeelectronics.co.in/avr-tutorials/interfacing-12-bit-spi-adc-mcp3204-with-avr-micro/
//Si hay errores agregar esta opcion al avrdude -B155
/** \file	main.c 
 *  \brief	Archivo que llama el inicio de todaslas maquinas de estado y cierra un bucle \n
 *	infinito lamando a cada una.
 */

// Programacion de fuses
// avrdude	-pm32 -cusbtiny -F -u -Ulfuse:w:0x7f:m -Uhfuse:w:0xd1:m


#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>

//#include "USART.h"
#include "macros.h"
#include "inicioUc.h"
#include "display.h"
#include "lcd.h"
#include "medicion.h"
#include "botones.h"


char 		chBuffer2[20];


volatile uint16_t u16_pulsos;
volatile uint16_t u16_pulsos_en_intervalo;
volatile uint32_t u16_intervalo_s;  // Variable que define el intervalo de conteo.
volatile uint32_t u16_intervalo;  // Variable que define el intervalo de conteo.
volatile uint8_t  u8_unidad;  // Unidad usada para definir el intervalo
volatile uint32_t u16_tiempo_s; 	// Variable que almacena el tiempo transcurrido en segundos
volatile uint16_t u16_CPM;
volatile uint16_t u16_CPM_temp;


volatile uint16_t u16Cont_ms;
volatile uint8_t flag_stop;


/*****************************************************************************
* Variables externas
*****************************************************************************/
extern volatile unsigned int		uContDisplay;									/* display.c, contador para la medicion de tiempos en el manejo del display */

extern struct_botones botones;

int main (void) {

//	wdt_disable();					/* por las dudas deshabilito el watchdog timer para el inicio de todo el programa */
	cli();							/* Deshabilito las interrupciones globales */

	_delay_ms(100); // Para que se ajusten las tensiones.

	Inicio_Puertos ();
	Inicio_Timer0 ();
//  Inicio_Timer1 ();
	Inicio_Display ();
//	Inicio_USART (0);
	Inicio_Interrupcion_pulsos();  //Interrupcion que se activa al recibir un pulso

//	wdt_enable(WDTO_500MS);

//	  	LCDClear();
//		LCDWriteStringXY(0,0,"CUENTAS/SEGUNDOS");

	sei();	// Habilito las interrupciones


	/*VALORES INICIALES */
	u16_intervalo = 30; // Intervalo de medicion - 1 minuto
	u16_intervalo_s = 30; // Intervalo de medicion - 1 minuto
	u16_tiempo_s = u16_intervalo_s;
	u8_unidad = SEGUNDOS;
	init_botones ();

while(1){

	deteccion_botones();
	atender_botones ();
	Actualizar_Display();
	calculo_CPM ();
//	wdt_reset(); // Actualizar el watchdog para que no produzca un reinicio

	}

return 0;
}


// Interrupcion por deteccion de pulso
ISR(INT0_vect){
	//u16_pulsos++; // Suma 1 al conteo
	u16_pulsos_en_intervalo++;
}


ISR (TIMER0_COMP_vect){  // Interrumpe cada 1mS

	if (botones.timeout_debouncer != 0){
		botones.timeout_debouncer--;
	}


	if (uContDisplay != 0){
		uContDisplay--;
	}


// Contador de segundos, hacia abajo.
	if (u16Cont_ms != 0){
		u16Cont_ms--;  //
	}

	else
	{
		if (u16_tiempo_s != 0){
			u16_tiempo_s--;  //Disminuye el contador del periodo de muestreo

		}

		u16Cont_ms = 1000;
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

ISR(BADISR_vect) {
	LCDClear();
	LCDWriteStringXY(0,0,"Error INT");
	while(1){
	}
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
