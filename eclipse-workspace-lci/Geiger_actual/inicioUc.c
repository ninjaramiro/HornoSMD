
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
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_7, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_6, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_5, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_4, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_3, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_2, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_1, SALIDA);
	DIRECCION_PIN(LCD_DB_PORT, LCD_DB_0, SALIDA);

	/* Puerto B */
	DIRECCION_PIN(DIR_LCD_RS_PORT, LCD_CONT_RS, SALIDA);
	DIRECCION_PIN(DIR_LCD_RW_PORT, LCD_CONT_RW, SALIDA);
	DIRECCION_PIN(DIR_LCD_ENB_PORT, LCD_ENB, SALIDA);


	DIRECCION_PIN(SPI_PORT, MOSI_PIN, SALIDA);
	DIRECCION_PIN(SPI_PORT, MISO_PIN, ENTRADA);
	PULL_UP_PIN(SPI_PORT, MISO_PIN, OFF);
	DIRECCION_PIN(SPI_PORT, SCK_PIN, SALIDA);

	/* Puerto C */

	DIRECCION_PIN(DIR_BOT_PORT, BOT_DISP_PIN, ENTRADA);
	DIRECCION_PIN(DIR_BOT_PORT, BOT_UP_PIN, ENTRADA);
	DIRECCION_PIN(DIR_BOT_PORT, BOT_RESET_PIN, ENTRADA);
	DIRECCION_PIN(DIR_BOT_PORT, BOT_DOWN_PIN, ENTRADA);
	DIRECCION_PIN(DIR_BOT_PORT, BOT_PARAM_PIN, ENTRADA);
	DIRECCION_PIN(DIR_BOT_PORT, BOT_INJ_ERR_PIN, ENTRADA);
	DIRECCION_PIN(DIR_BOT_PORT, BOT_RUN_STOP_PIN, ENTRADA);

	PULL_UP_PIN(BOT_PORT, BOT_DISP_PIN, ON);
	PULL_UP_PIN(BOT_PORT, BOT_UP_PIN, ON);
	PULL_UP_PIN(BOT_PORT, BOT_RESET_PIN, ON);
	PULL_UP_PIN(BOT_PORT, BOT_DOWN_PIN, ON);
	PULL_UP_PIN(BOT_PORT, BOT_PARAM_PIN, ON);
	PULL_UP_PIN(BOT_PORT, BOT_INJ_ERR_PIN, ON);
	PULL_UP_PIN(BOT_PORT, BOT_RUN_STOP_PIN, ON);

	/* Puerto D */
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_RX, ENTRADA);
	PULL_UP_PIN(SERIAL_PORT, SERIAL_RX, OFF);
	DIRECCION_PIN(DIR_SERIAL_PORT, SERIAL_TX, SALIDA);
	DIRECCION_PIN(DIR_BUZZER_PORT, BUZZER, SALIDA);
	DIRECCION_PIN(DIR_PULSO_PORT, PULSO_PIN, ENTRADA);
	PULL_UP_PIN(PULSO_PORT, PULSO_PIN, OFF);

// NO USADOS
	DIRECCION_PIN(DIR_NC_B_PORT, NC1_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_B_PORT, NC2_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_C_PORT, NC3_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_D_PORT, NC4_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_D_PORT, NC5_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_D_PORT, NC6_PIN, ENTRADA);
	DIRECCION_PIN(DIR_NC_D_PORT, NC7_PIN, ENTRADA);
	PULL_UP_PIN(NC_B_PORT, NC1_PIN, ON);
	PULL_UP_PIN(NC_B_PORT, NC2_PIN, ON);
	PULL_UP_PIN(NC_C_PORT, NC3_PIN, ON);
	PULL_UP_PIN(NC_D_PORT, NC4_PIN, ON);
	PULL_UP_PIN(NC_D_PORT, NC5_PIN, ON);
	PULL_UP_PIN(NC_D_PORT, NC6_PIN, ON);
	PULL_UP_PIN(NC_D_PORT, NC7_PIN, ON);

//	GICR = (OFF<<INT0)|(OFF<<INT1)|(OFF<<INT2);


}


void Inicio_Interrupcion_pulsos (void) {

	MCUCR = (ON<<ISC01)|(ON<<ISC00);  //The falling edge of INT1 generates an interrupt request.
	GICR = (ON<<INT0);
}


/************************************************************************
* Inicio Timer 0
************************************************************************/
/* Inicio el Timer 0 como marcador de inetrvalos de 1ms, modo 2 CTC, con prescaler 64 (CS02:0=100) [1024 (CS02:0=111)] */

void Inicio_Timer0 (void) {
	//Timer counter control register 0
//	TCCR0 = (OFF<<FOC0)|(OFF<<WGM00)|(ON<<WGM01)|(OFF<<COM01)|(OFF<<COM00)|(OFF<<CS02)|(ON<<CS01)|(ON<<CS00);
//	TCCR0 = 0b00000011;

//	MODO CTC, Prescaler en 1/64
	TCCR0 = 0b00001011;

	// Output Compare Register
	//OCR0 = ((F_CPU/64)/FRECUECNIA_INT_TIMER_0)-1;  // OCR0 = 124 creo
	OCR0 = 124; // 1mS


	// Borro flags
	CLEAR_BIT (TIFR, OCF0);		/* aseguro que no haya ninguna interrupción por la unidad de comparación */
	CLEAR_BIT (TIFR, TOV0);		/* aseguro que no haya ninguna interrupción por overflow */


	// Configuro interrupciones
//	TIMSK = (ON<<TOIE0)|(OFF<OCIE0);
//	TIMSK = (OFF<<TOIE0)|(ON<OCIE0);
	TIMSK = 0b00000010;
}

/************************************************************************
* Inicio Timer 1
************************************************************************/
/* Inicio el Timer 1 como marcador de inetrvalos de 1s, modo CTC, con prescaler 1024 */

//void Inicio_Timer1 (void) {
//
//	//Configuracion del contador 1 de 16bits, modo 4 CTC comparacion con OCR1A para resetear.
//	// CTC = clear time on compare
//
////	TCCR1A = (OFF<<COM1A1)|(OFF<<COM1A0)|(OFF<<COM1B1)|(OFF<<COM1B0)|(OFF<<FOC1A)|(OFF<<FOC1B)|(OFF<<WGM11)|(OFF<<WGM10);
////	TCCR1B = (OFF<<ICNC1)|(OFF<<ICES1)|(OFF<<WGM13)|(ON<<WGM12)|(OFF<<CS12)|(OFF<<CS11)|(ON<<CS10);
//
////	TCCR1A = 0;
////	TCCR1B = 5;
//
//	TCCR1A = (OFF<<COM1A1)|(OFF<<COM1A0)|(OFF<<COM1B1)|(OFF<<COM1B0)|(OFF<<FOC1A)|(OFF<<FOC1B)|(OFF<<WGM11)|(OFF<<WGM10);
//	TCCR1B = (OFF<<ICNC1)|(OFF<<ICES1)|(OFF<<WGM13)|(ON<<WGM12)|(ON<<CS12)|(OFF<<CS11)|(ON<<CS10);
//
//
//
//// Output Compare Register A
//// Cuenta hasta el nro 7199 (pasado a binario de 16 bits)
////	OCR1AH = 0b00011100;
////	OCR1AL = 0b00011111;
//
//	OCR1A = 10000;
//
//	// Borro flags
////	CLEAR_BIT (TIFR, ICF1);		/* aseguro que no haya ninguna interrupción por captura externa*/
////	CLEAR_BIT (TIFR, OCF1A);	/* aseguro que no haya ninguna interrupción por comparacion A */
////	CLEAR_BIT (TIFR, OCF1B);	/* aseguro que no haya ninguna interrupción por comparacion B */
////	CLEAR_BIT (TIFR, TOV1);		/* aseguro que no haya ninguna interrupción por overflow */
//
//	TIFR = (OFF<<ICF1)|(OFF<<OCF1A)|(OFF<<OCF1B)|(OFF<<TOV1);
//
//	// Configuro interrupciones
////	CLEAR_BIT (TIMSK,TICIE1);	/* deshabilito la interrupción por entrada */
////	SET_BIT (TIMSK,OCIE1A);	    /* Habilito la interrupción para cuando OCR1A == TCNT1 */
////	CLEAR_BIT (TIMSK,OCIE1B);	/* deshabilito la interrupción para cuando OCR1B == TCNT1 */
////	CLEAR_BIT (TIMSK,TOIE1);	/* deshabilito la interrupción por overflow */
//
//
//	TIMSK = (OFF<<TICIE1)|(ON<<OCIE1A)|(OFF<<OCIE1B)|(OFF<<TOIE1);
//
//
//
//}

