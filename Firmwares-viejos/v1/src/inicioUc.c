
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
	DIRECCION_PIN(DIR_TECLADO_PORT, PIN_TECLADO_1, ENTRADA);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_1, ON);
	DIRECCION_PIN(DIR_TECLADO_PORT, PIN_TECLADO_2, ENTRADA);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_2, ON);
	DIRECCION_PIN(DIR_TECLADO_PORT, PIN_TECLADO_3, ENTRADA);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_3, ON);
	DIRECCION_PIN(DIR_TECLADO_PORT, PIN_TECLADO_4, ENTRADA);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_4, ON);
	DIRECCION_PIN(DIR_TECLADO_PORT, PIN_TECLADO_5, ENTRADA);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_5, ON);
	DIRECCION_PIN(DIR_TECLADO_PORT, PIN_TECLADO_6, ENTRADA);
	PULL_UP_PIN(TECLADO_PORT, PIN_TECLADO_6, ON);


	/* Puerto B */
	DIRECCION_PIN(DIR_LCD_BACKLIGHT_PORT, LCD_BACKLIGHT, SALIDA);
	DIRECCION_PIN(DIR_PWM_420MA_PORT, PWM_2_420MA, SALIDA);
	DIRECCION_PIN(DIR_PWM_420MA_PORT, PWM_1_420MA, SALIDA);
	DIRECCION_PIN(DIR_PULSOS_COND_PORT, PULSOS_COND, SALIDA);


	/* Puerto C */
	DIRECCION_PIN(DIR_LCD_DB_PORT, LCD_DB_7, SALIDA);
	DIRECCION_PIN(DIR_LCD_DB_PORT, LCD_DB_6, SALIDA);
	DIRECCION_PIN(DIR_LCD_DB_PORT, LCD_DB_5, SALIDA);
	DIRECCION_PIN(DIR_LCD_DB_PORT, LCD_DB_4, SALIDA);


	/* Puerto D */
	DIRECCION_PIN(DIR_SERIAL_422485_PORT, RXD_422485, ENTRADA);
	DIRECCION_PIN(DIR_SERIAL_422485_PORT, TXD_422485, SALIDA);
	DIRECCION_PIN(DIR_SERIAL_422485_PORT, SELECT_422485, SALIDA);
	DIRECCION_PIN(DIR_LCD_RS_PORT, LCD_CONT_RS, SALIDA);
	DIRECCION_PIN(DIR_LCD_RW_PORT, LCD_CONT_RW, SALIDA);


	/* Puerto E */
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_RX, ENTRADA);
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_TX, SALIDA);
	DIRECCION_PIN(DIR_BUZZER_PORT, BUZZER, SALIDA);
	DIRECCION_PIN(DIR_SALIDA_DIG_PORT, SALIDA_DIG_2, SALIDA);
	DIRECCION_PIN(DIR_SALIDA_DIG_PORT, SALIDA_DIG_1, SALIDA);


	/* Puerto F */
	DIRECCION_PIN(DIR_ADC_420MA_PORT, ADC_1_420MA_P, ENTRADA);
	DIRECCION_PIN(DIR_ADC_420MA_PORT, ADC_1_420MA_N, ENTRADA);
	DIRECCION_PIN(DIR_ADC_420MA_PORT, ADC_2_420MA_P, ENTRADA);
	DIRECCION_PIN(DIR_ADC_420MA_PORT, ADC_2_420MA_N, ENTRADA);
	DIRECCION_PIN(DIR_ADC_CONDUCT_PORT, ADC_CONDUCT_1, ENTRADA);
	DIRECCION_PIN(DIR_ADC_CONDUCT_PORT, ADC_CONDUCT_2, ENTRADA);


	/* Puerto G*/
	DIRECCION_PIN(DIR_LCD_ENB_PORT, LCD_ENB, SALIDA);

}

/************************************************************************
* Inicio Timer 0
************************************************************************/
/* Inicio el Timer 0 como marcador de inetrvalos de 1ms, modo 2 CTC, con prescaler 64 (CS02:0=100) [1024 (CS02:0=111)] */

void Inicio_Timer0 (void) {
	TCCR0 = (OFF<<FOC0)|(OFF<<WGM00)|(ON<<WGM01)|(OFF<<COM01)|(OFF<<COM00)|(ON<<CS02)|(OFF<<CS01)|(OFF<<CS00);
	OCR0 = ((F_CPU/64)/FRECUECNIA_INT_TIMER_0)-1;
	SET_BIT (TIFR, OCF0);		/* aseguro que no haya ninguna interrupción por la unidad de comparación */
	SET_BIT (TIFR, TOV0);		/* aseguro que no haya ninguna interrupción por overflow */
	CLEAR_BIT (TIMSK,TOIE0);	/* deshabilito la interrupción por overflow */
	SET_BIT (TIMSK, OCIE0);		/* Habilito la interrupción para cuando OCR0 == TCNT0 */
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
