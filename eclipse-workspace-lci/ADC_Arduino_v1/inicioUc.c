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


	/* Puerto B */
	DIRECCION_PIN(DIR_LED_PORT, LED, SALIDA);
//	DIRECCION_PIN(DIR_LED_PORT, LED_FRENTE, SALIDA);


	DIRECCION_PIN(DIR_SPI_PORT, MOSI_PIN, SALIDA);
	DIRECCION_PIN(DIR_SPI_PORT, MISO_PIN, ENTRADA);
	DIRECCION_PIN(DIR_SPI_PORT, SCK_PIN, SALIDA);
	PULL_UP_PIN(SPI_PORT, MISO_PIN, OFF);

	/* Puerto C */


	/* Puerto D */

	DIRECCION_PIN(DIR_BOT1_PORT, BOT1_PIN, SALIDA);
	PULL_UP_PIN(BOT1_PORT, BOT1_PIN, ON);

	DIRECCION_PIN(DIR_BOT2_PORT, BOT2_PIN, SALIDA);
	PULL_UP_PIN(BOT2_PORT, BOT2_PIN, ON);

	DIRECCION_PIN(DIR_INT0_PORT, INT0_PIN, ENTRADA);
	PULL_UP_PIN(INT0_PORT, INT0_PIN, OFF);

	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_TX, SALIDA);
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_RX, SALIDA);
	PULL_UP_PIN(SERIAL_PORT, INT0_PIN, ON);

	DIRECCION_PIN(DIR_SPI_PORT_2, MISO_PIN_2, ENTRADA);
	PULL_UP_PIN(SPI_PORT_2, MISO_PIN_2, OFF);


	// NO CONECTADOS

//	DIRECCION_PIN(DIR_NC_B_PORT, NC_B0_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_B_PORT, NC_B1_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_B_PORT, NC_B2_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_B_PORT, NC_B6_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_B_PORT, NC_B7_PIN, ENTRADA);

//	PULL_UP_PIN(NC_B_PORT, NC_B0_PIN, ON);
//	PULL_UP_PIN(NC_B_PORT, NC_B1_PIN, ON);
	PULL_UP_PIN(NC_B_PORT, NC_B2_PIN, ON);
	PULL_UP_PIN(NC_B_PORT, NC_B6_PIN, ON);
	PULL_UP_PIN(NC_B_PORT, NC_B7_PIN, ON);


//	DIRECCION_PIN(DIR_NC_C_PORT, NC_C0_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_C_PORT, NC_C1_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_C_PORT, NC_C2_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_C_PORT, NC_C3_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_C_PORT, NC_C4_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_C_PORT, NC_C5_PIN, ENTRADA);
//
//	PULL_UP_PIN(NC_C_PORT, NC_C0_PIN, ON);
//	PULL_UP_PIN(NC_C_PORT, NC_C1_PIN, ON);
//	PULL_UP_PIN(NC_C_PORT, NC_C2_PIN, ON);
//	PULL_UP_PIN(NC_C_PORT, NC_C3_PIN, ON);
//	PULL_UP_PIN(NC_C_PORT, NC_C4_PIN, ON);
//	PULL_UP_PIN(NC_C_PORT, NC_C5_PIN, ON);

////	DIRECCION_PIN(DIR_NC_D_PORT, NC_D3_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_D_PORT, NC_D4_PIN, ENTRADA);
//	DIRECCION_PIN(DIR_NC_D_PORT, NC_D5_PIN, ENTRADA);
////	PULL_UP_PIN(NC_D_PORT, NC_D3_PIN, ON);
//	PULL_UP_PIN(NC_D_PORT, NC_D4_PIN, ON);
//	PULL_UP_PIN(NC_D_PORT, NC_D5_PIN, ON);

}


void Inicio_Interrupcion0 (void) {
//	MCUCR = (ON<<ISC01)|(OFF<<ISC00);
//	EICRA = (OFF<<ISC01)|(OFF<<ISC00); // Interrupcion por nivel bajo de INT0
	EICRA = (ON<<ISC01)|(OFF<<ISC00);  // Interrupcion flanco descendente INT0
	EIMSK = (ON<<INT0);
}

/************************************************************************
* Inicio Timer 0
************************************************************************/
/* Inicio el Timer 0 como marcador de inetrvalos de 1ms, modo 2 CTC, con prescaler 64 (CS02:0=100) [1024 (CS02:0=111)] */

void Inicio_Timer0 (void) {
//	//	TIMSK = 0;
//
//	//Timer counter control register 0
//	TCCR0 = (OFF<<FOC0)|(OFF<<WGM00)|(OFF<<WGM01)|(OFF<<COM01)|(OFF<<COM00)|(OFF<<CS02)|(ON<<CS01)|(ON<<CS00);
//
//	// Output Compare Register
//	OCR0 = ((F_CPU/64)/FRECUECNIA_INT_TIMER_0)-1;  // OCR0 = 124 creo
//
//	// Borro flags
//	CLEAR_BIT (TIFR, OCF0);		/* aseguro que no haya ninguna interrupción por la unidad de comparación */
//	CLEAR_BIT (TIFR, TOV0);		/* aseguro que no haya ninguna interrupción por overflow */
//
//	// Configuro interrupciones
//	CLEAR_BIT (TIMSK,TOIE0);	/* deshabilito la interrupción por overflow */
//	SET_BIT (TIMSK, OCIE0);		/* Habilito la interrupción para cuando OCR0 == TCNT0 */
//

}

//void Inicio_Timer0 (void) {
//  TCCR0 = 0b00000000;
//	TCCR0B = 0b00000011;	// prescaler 64
//	SET_BIT (TIMSK0,TOIE0);	// habilito la interrupción por overflow
//	CLEAR_BIT (TIFR0, TOV0);		// aseguro que no haya ninguna interrupción por overflow
//}


///************************************************************************
//* Inicio Del Buzzer
//************************************************************************/
//void Inicio_Buzzer (void) {
//	chBuzzerModo = BUZZER_CORTO;
//	uContBuzzerON = 0;
//	uContBuzzerOFF = 0;
//
//	CLEAR_BIT(BUZZER_PORT, BUZZER);
//}






