
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
	DIRECCION_PIN(CS2_PORT, CS2, SALIDA);
	DIRECCION_PIN(ADC_PORT, PA1, ENTRADA);
	DIRECCION_PIN(ADC_PORT, PA2, ENTRADA);
	DIRECCION_PIN(ADC_PORT, PA3, ENTRADA);

	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_4, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_5, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_6, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_7, SALIDA);

	/* Puerto B */
	DIRECCION_PIN(LCD_RS_PORT, LCD_CONT_RS, SALIDA);
	DIRECCION_PIN(LCD_RW_PORT, LCD_CONT_RW, SALIDA);
	DIRECCION_PIN(LCD_ENB_PORT, LCD_ENB, SALIDA);
	DIRECCION_PIN(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT, SALIDA);
	DIRECCION_PIN(CS3_PORT, CS3, SALIDA);
	DIRECCION_PIN(SPI_PORT, MOSI, SALIDA);
	DIRECCION_PIN(SPI_PORT, MISO, ENTRADA);
	DIRECCION_PIN(SPI_PORT, SCK, SALIDA);


	/* Puerto C */

	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_1, ENTRADA);
	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_2, ENTRADA);
	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_3, ENTRADA);
	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_4, ENTRADA);
	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_5, ENTRADA);
	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_6, ENTRADA);
	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_7, ENTRADA);
	DIRECCION_PIN(TECLADO_PORT, PIN_TECLADO_8, ENTRADA);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_1, ON);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_2, ON);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_3, ON);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_4, ON);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_5, ON);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_6, ON);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_7, ON);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_8, ON);

	/* Puerto D */
	DIRECCION_PIN(SERIAL_PORT, SERIAL_RX, ENTRADA);
	DIRECCION_PIN(SERIAL_PORT, SERIAL_TX, SALIDA);
	DIRECCION_PIN(CS1_PORT, CS1, SALIDA);
	DIRECCION_PIN(BUZZER_PORT, BUZZER, SALIDA);
	DIRECCION_PIN(PWM_PORT, PWM_1, SALIDA);
	DIRECCION_PIN(PWM_PORT, PWM_2, SALIDA);
	DIRECCION_PIN(LEDS_PORT, LED_1, SALIDA);
	DIRECCION_PIN(LEDS_PORT, LED_2, SALIDA);



}

/************************************************************************
* Inicio Timer 0
************************************************************************/
/* Inicio el Timer 0 como marcador de inetrvalos de 1ms, modo 2 CTC, con prescaler 64 (CS02:0=100) [1024 (CS02:0=111)] */

void Inicio_Timer0 (void) {
	// TCCR0 = (OFF<<FOC0)|(OFF<<WGM00)|(ON<<WGM01)|(OFF<<COM01)|(OFF<<COM00)|(ON<<CS02)|(OFF<<CS01)|(OFF<<CS00);
	// OCR0 = ((F_CPU/64)/FRECUECNIA_INT_TIMER_0)-1;
	// SET_BIT (TIFR, OCF0);		/* aseguro que no haya ninguna interrupción por la unidad de comparación */
	// SET_BIT (TIFR, TOV0);		/* aseguro que no haya ninguna interrupción por overflow */
	// CLEAR_BIT (TIMSK,TOIE0);	/* deshabilito la interrupción por overflow */
	// SET_BIT (TIMSK, OCIE0);		/* Habilito la interrupción para cuando OCR0 == TCNT0 */
}


/************************************************************************
* Inicio Del Buzzer
************************************************************************/
void Inicio_Buzzer (void) {
	chBuzzerModo = BUZZER_CORTO;
	uContBuzzerON = 0;
	uContBuzzerOFF = 0;

	CLEAR_BIT(BUZZER_PORT, BUZZER);
}
