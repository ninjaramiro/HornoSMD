
/** \file	main.c 
 *  \brief	Archivo que llama el inicio de todas las maquinas de estado y cierra un bucle \n
 *	infinito llamando a cada una.
 */

/* Funcionamiento del programa:
 *
 * Botones:
 *
 * |  ON/OFF  |  RUN  |  UP  |  DOWN  |  EXIT  |
 *
 * Cuando se conecta a la alimentación, aparece el mensaje "Laboratorio LCI \n Horno LCI"
 * Al presionar ON/OFF aparece el menu principal.
 * Entonces se puede elegir entre el modo automatico y el manual con los botones UP y DOWN.
 * Para seleccionar el modo se presiona el boton RUN.
 * 		Modo automático: Sigue el perfil de temperatura seteado.
 * 						 Una vez que termina de seguir esta curva, se vuelve al estado de standby.
 * 						 Si se quiere terminar antes con el proceso, se presiona el botón EXIT.
 * 		Modo manual: Se setea una temperatura deseada y para comenzar se presiona RUN.
 * 					 El horno alcanzará esta temperatura y se mantendrá allí. Se podrá ver el tiempo
 * 					 que el horno se encuentra encendido. Para salir alestado de standby se presiona EXIT.
 *
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
#include "max6675.h"
#include "botones.h"
#include "control.h"

/*****************************************************************************
* Variables
*****************************************************************************/
char chBuffer[30];

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern volatile unsigned int uiContador_Blinky; // Para el tiempo de conteo del blinky
extern volatile unsigned int uContDisplay; /* display.c, contador para la medicion de tiempos en el manejo del display */
extern float flTempMedida;
extern struct_botones botones;
extern volatile unsigned int	uContControlAuto;
extern volatile unsigned int	uContMax6675;

int main (void) {

	// Configuraciones iniciales

	//wdt_disable();			// por las dudas deshabilito el watchdog timer para el inicio de todo el programa
	cli();						// Deshabilito las interrupciones globales
	Inicio_Puertos();
    Inicio_Timer0();
    Spi_Max6675_Inicio();
	blinky_init();
	Inicio_Display(); 			// El display es de 2x16
	//wdt_enable(WDTO_1S);		// prendo el watchdog timer
	control_init();
	sei();						// Habilito las interrupciones


    //Standby_Display();

    flTempMedida=140;
    init_botones();

	while (1) {  // Loop infinito

		deteccion_botones(); // detecta los botones, cuando el pin se conecta a GND

		atender_botones();

		Max6675_Get_Temp();

		control_temperatura();

		Actualizar_Display();


	} // while

return 0;
} // main


//ISR (TIMER0_COMP_vect) { // Esto seria si se utiliza un atmega16
ISR (SPI_STC_vect) //Para atmega 32 // Interrumpe cada 1mS
{
	if (uiContador_Blinky != 0)
	{
		uiContador_Blinky--;
	}

	if (uContDisplay != 0)
	{
		uContDisplay--;
	}

	if (botones.timeout_debouncer != 0)
	{
		botones.timeout_debouncer--;
	}

	if (uContControlAuto != 0)
	{
		uContControlAuto--;
	}

	if (uContMax6675 != 0)
	{
		uContMax6675--;
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
