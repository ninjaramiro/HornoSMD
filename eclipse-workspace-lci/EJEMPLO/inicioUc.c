
/** \file	inicioUc.c
 *  \brief	Contiene las funciones de inicio de los perisfericos del uc. \n
 *	Asi como tambien funciones comunes a diferentes maquinas de estado.
 */



#include "inicioUc.h"
#include "macros.h"

#include <avr/io.h>
#include <inttypes.h>
#include <avr/sleep.h>

/************************************************************************
* Variables
************************************************************************/
volatile char			chBuzzerModo;	/**< \brief modo de operacion del buzzer */
volatile unsigned int	uContBuzzerON;	/**< \brief contador de tiempo encendido del buzzer */
volatile unsigned int	uContBuzzerOFF;	/**< \brief contador de tiempo apagado del buzzer */

/************************************************************************
* Inicio de los puertos
************************************************************************/
void Inicio_Puertos (void) {	/* Funcion de inicio de los puertos de entrada/salida */

	/* Puerto A */
	DIRECCION_PIN(CS2_PORT, CS2_PIN, SALIDA);
	DIRECCION_PIN(TEMP_PORT, PIN_TEMP_1, ENTRADA);
	PULL_UP_PIN(TEMP_PORT, PIN_TEMP_1, OFF);
	DIRECCION_PIN(TEMP_PORT, PIN_TEMP_2, ENTRADA);
	PULL_UP_PIN(TEMP_PORT, PIN_TEMP_2, OFF);
	DIRECCION_PIN(TEMP_PORT, PIN_TEMP_3, ENTRADA);
	PULL_UP_PIN(TEMP_PORT, PIN_TEMP_3, OFF);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_7, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_6, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_5, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_4, SALIDA);


	/* Puerto B */
	DIRECCION_PIN(DIR_LCD_RS_PORT, LCD_CONT_RS, SALIDA);
	DIRECCION_PIN(DIR_LCD_RW_PORT, LCD_CONT_RW, SALIDA);
	DIRECCION_PIN(DIR_LCD_ENB_PORT, LCD_ENB, SALIDA);
	DIRECCION_PIN(DIR_LCD_BACKLIGHT_PORT, LCD_BACKLIGHT, SALIDA);
	DIRECCION_PIN(CS3_PORT, CS3_PIN, SALIDA);
	DIRECCION_PIN(SPI_PORT, MOSI_PIN, SALIDA);
	DIRECCION_PIN(SPI_PORT, MISO_PIN, ENTRADA);
	PULL_UP_PIN(SPI_PORT, MISO_PIN, OFF);
	DIRECCION_PIN(SPI_PORT, SCK_PIN, SALIDA);


	/* Puerto C */
	DIRECCION_PIN(DIR_AUX_PORT, AUX1_PIN, ENTRADA);
	DIRECCION_PIN(DIR_AUX_PORT, AUX2_PIN, ENTRADA);
	DIRECCION_PIN(DIR_AUX_PORT, AUX3_PIN, ENTRADA);
	DIRECCION_PIN(DIR_AUX_PORT, AUX4_PIN, ENTRADA);
	DIRECCION_PIN(DIR_AUX_PORT, AUX5_PIN, ENTRADA);
	DIRECCION_PIN(DIR_AUX_PORT, AUX6_PIN, ENTRADA);
	DIRECCION_PIN(DIR_AUX_PORT, AUX7_PIN, ENTRADA);
	DIRECCION_PIN(DIR_AUX_PORT, AUX8_PIN, ENTRADA);
	PULL_UP_PIN(AUX_PORT, AUX1_PIN, ON);
	PULL_UP_PIN(AUX_PORT, AUX2_PIN, ON);
	PULL_UP_PIN(AUX_PORT, AUX3_PIN, ON);
	PULL_UP_PIN(AUX_PORT, AUX4_PIN, ON);
	PULL_UP_PIN(AUX_PORT, AUX5_PIN, ON);
	PULL_UP_PIN(AUX_PORT, AUX6_PIN, ON);
	PULL_UP_PIN(AUX_PORT, AUX7_PIN, ON);
	PULL_UP_PIN(AUX_PORT, AUX8_PIN, ON);


	/* Puerto D */
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_RX, ENTRADA);
	PULL_UP_PIN(SERIAL_PORT, SERIAL_RX, OFF);
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_TX, SALIDA);
	DIRECCION_PIN(DIR_BUZZER_PORT, BUZZER, SALIDA);
	DIRECCION_PIN(DIR_SALIDA_DIG_PORT, SALIDA_DIG_2, SALIDA);
	DIRECCION_PIN(DIR_SALIDA_DIG_PORT, SALIDA_DIG_1, SALIDA);
	DIRECCION_PIN(DIR_LEDS_PORT, BUZZER, SALIDA);
	DIRECCION_PIN(DIR_LEDS_PORT, LED1, SALIDA);
	DIRECCION_PIN(DIR_LEDS_PORT, LED2, SALIDA);

}

/************************************************************************
* Inicio Timer 0
************************************************************************/
/* Inicio el Timer 0 como marcador de inetrvalos de 1ms, modo 2 CTC, con prescaler 64 (CS02:0=100) [1024 (CS02:0=111)] */

void Inicio_Timer0 (void) {
//	TIMSK = 0;

	//Timer counter control register 0
	TCCR0 = (OFF<<FOC0)|(OFF<<WGM00)|(ON<<WGM01)|(OFF<<COM01)|(OFF<<COM00)|(OFF<<CS02)|(ON<<CS01)|(ON<<CS00);

	// Output Compare Register
	OCR0 = ((F_CPU/64)/FRECUECNIA_INT_TIMER_0)-1;  // OCR0 = 124 creo
//	OCR0 = 124;

	// Borro flags
	CLEAR_BIT (TIFR, OCF0);		/* aseguro que no haya ninguna interrupción por la unidad de comparación */
	CLEAR_BIT (TIFR, TOV0);		/* aseguro que no haya ninguna interrupción por overflow */

	// Configuro interrupciones
	CLEAR_BIT (TIMSK,TOIE0);	/* deshabilito la interrupción por overflow */
	SET_BIT (TIMSK, OCIE0);		/* Habilito la interrupción para cuando OCR0 == TCNT0 */


}




