
/** \file	inicio.c
 *  \brief	Contiene las funciones de inicio de los perisfericos del uc. \n
 *
 *	Asi como tambien funciones comunes a diferentes maquinas de estado.
 */

#include "inicio.h"
#include "macros.h"

#include <avr/io.h>

/************************************************************************
* Inicio de los puertos
************************************************************************/

void Inicio_Puertos (void) {	/* Funcion de inicio de los puertos de entrada/salida */

	/* Puerto B */
	DIRECCION_PIN(DIR_CAUDALIMETRO_PORT, CAUDALIMETRO_PIN, ENTRADA);
	DIRECCION_PIN(DIR_TEMP_AGUA_PORT, TEMP_AGUA_PIN, SALIDA);
	DIRECCION_PIN(DIR_LCD_E_PORT, LCD_E_PIN, SALIDA);
	DIRECCION_PIN(DIR_SPI_PORT, SCK_PIN, SALIDA);
	DIRECCION_PIN(DIR_SPI_PORT, MOSI_PIN, SALIDA);
	DIRECCION_PIN(DIR_SPI_PORT, MISO_PIN, ENTRADA);

	/* Puerto C */
	DIRECCION_PIN(DIR_MOTOR_PP_PORT, MOTOR_PP_PIN_1, SALIDA);
	DIRECCION_PIN(DIR_MOTOR_PP_PORT, MOTOR_PP_PIN_2, SALIDA);
	DIRECCION_PIN(DIR_MOTOR_PP_PORT, MOTOR_PP_PIN_3, SALIDA);
	DIRECCION_PIN(DIR_MOTOR_PP_PORT, MOTOR_PP_PIN_4, SALIDA);
	DIRECCION_PIN(DIR_RELAY_GAS_PORT, RELAY_GAS_PIN, SALIDA);
	DIRECCION_PIN(DIR_RELAY_CARGA_PORT, RELAY_CARGA_PIN, SALIDA);
//	DIRECCION_PIN(DIR_TENSION_BAT_PORT, TENSION_BAT_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_SET_TEMP_PORT, SET_TEMP_PIN, ENTRADA);

	/* Puerto D */
	DIRECCION_PIN(DIR_DISPLAY_PORT, DISPLAY_DB0, SALIDA);
	DIRECCION_PIN(DIR_DISPLAY_PORT, DISPLAY_DB1, SALIDA);
	DIRECCION_PIN(DIR_DISPLAY_PORT, DISPLAY_DB2, SALIDA);
	DIRECCION_PIN(DIR_DISPLAY_PORT, DISPLAY_DB3, SALIDA);
	DIRECCION_PIN(DIR_DISPLAY_PORT, DISPLAY_RS, SALIDA);
	DIRECCION_PIN(DIR_DISPLAY_PORT, DISPLAY_RW, SALIDA);
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_RXD_PIN, ENTRADA);
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_TXD_PIN, SALIDA);
}

/************************************************************************
* Inicio Timer 2
************************************************************************/
/* Inicio el Timer 2 como marcador de inetrvalos de 1ms, modo 2 CTC, con prescaler 64 (CS22:0=100) y OCR2 = fclk/64000-1 = TIMER2_TOP */

void Inicio_Timer2 (void) {
	TCCR2A = (ON<<WGM21)|(OFF<<WGM20)|(OFF<<COM2A1)|(OFF<<COM2A0)|(OFF<<COM2B1)|(OFF<<COM2B0);
	TCCR2B = (OFF<<FOC2A)|(OFF<<FOC2B)|(OFF<<WGM22)|(ON<<CS22)|(OFF<<CS21)|(OFF<<CS20);
	OCR2A = (F_CPU/64000)-1;
	SET_BIT (TIMSK2, OCIE2A);		/* Habilito la interrupción para cuando OCR2 == TCNT2 */
}
