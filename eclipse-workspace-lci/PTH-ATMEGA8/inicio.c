#include "inicio.h"
#include "macros.h"


/************************************************************************
* Inicio de los puertos
************************************************************************/

void Inicio_Puertos (void) {	/* Funcion de inicio de los puertos de entrada/salida */

/*--------- DIRECCION -----------------------------------------------*/
  DIRECCION_PIN(USART_DDR, USART_RXD, ENTRADA);
  DIRECCION_PIN(USART_DDR, USART_TXD, SALIDA);

  DIRECCION_PIN(BOT_1_DDR, BOT_1, ENTRADA);
  DIRECCION_PIN(BOT_2_DDR, BOT_2, ENTRADA);
  DIRECCION_PIN(BOT_3_DDR, BOT_3, ENTRADA);
  DIRECCION_PIN(BOT_4_DDR, BOT_4, ENTRADA);

  DIRECCION_PIN(POTE_DDR, POTE, ENTRADA);
  DIRECCION_PIN(BUZZER_DDR, BUZZER, SALIDA);
  DIRECCION_PIN(PULSE1_DDR, PULSE1, SALIDA);
  DIRECCION_PIN(PULSE2_DDR, PULSE2, SALIDA);

  DIRECCION_PIN(SENS_CORRIENTE_DDR, SENS_CORRIENTE, ENTRADA);

  DIRECCION_PIN(PWM1_DDR, PWM1, SALIDA);
  DIRECCION_PIN(PWM2_DDR, PWM2, SALIDA);

  DIRECCION_PIN(LCD_RS_DDR, LCD_RS, SALIDA);
  DIRECCION_PIN(LCD_RW_DDR, LCD_RW, SALIDA);
  DIRECCION_PIN(LCD_E_DDR, LCD_E, SALIDA);
  DIRECCION_PIN(LCD_D4_DDR, LCD_D4, SALIDA);
  DIRECCION_PIN(LCD_D5_DDR, LCD_D5, SALIDA);
  DIRECCION_PIN(LCD_D6_DDR, LCD_D6, SALIDA);
  DIRECCION_PIN(LCD_D7_DDR, LCD_D7, SALIDA);

/*--------------------------------------------------------*/
	// PULL_UP_PIN(USART_PORT, USART_RXD, NO);
	// PULL_UP_PIN(USART_PORT, USART_TXD, NO);

  PULL_UP_PIN(BOT_1_PORT, BOT_1, SI);  /* LOS BOTONES LLEVAN PULL UP */
  PULL_UP_PIN(BOT_2_PORT, BOT_2, SI);  /* LOS BOTONES LLEVAN PULL UP */
  PULL_UP_PIN(BOT_3_PORT, BOT_3, SI);  /* LOS BOTONES LLEVAN PULL UP */
  PULL_UP_PIN(BOT_4_PORT, BOT_4, SI);  /* LOS BOTONES LLEVAN PULL UP */

  PULL_UP_PIN(POTE_PORT, POTE, NO);
  PULL_UP_PIN(BUZZER_PORT, BUZZER, NO);

  PULL_UP_PIN(SENS_CORRIENTE_PORT, SENS_CORRIENTE, NO);

  PULL_UP_PIN(USART_PORT, USART_RXD, NO);


  // Estado inicial de los puertos

  CLEAR_BIT(BUZZER_PORT, BUZZER);
  CLEAR_BIT(PULSE1_PORT, PULSE1);
  CLEAR_BIT(PULSE2_PORT, PULSE2);
  CLEAR_BIT(PWM1_PORT, PWM1);
  CLEAR_BIT(PWM2_PORT, PWM2);
  CLEAR_BIT(LCD_RS_PORT, LCD_RS);
  CLEAR_BIT(LCD_RW_PORT, LCD_RW);
  CLEAR_BIT(LCD_E_PORT, LCD_E);
  CLEAR_BIT(LCD_D4_PORT, LCD_D4);
  CLEAR_BIT(LCD_D5_PORT, LCD_D5);
  CLEAR_BIT(LCD_D6_PORT, LCD_D6);
  CLEAR_BIT(LCD_D7_PORT, LCD_D7);

}

/************************************************************************
* Inicio Timer
************************************************************************/
/* Inicio el Timer 0 */

void Inicio_Timer0 (void) {
  //TCCR0 = 0b00000000;
	TCCR0 = 0b00000011;	// prescaler 64
	SET_BIT (TIMSK,TOIE0);	// habilito la interrupción por overflow
	CLEAR_BIT (TIFR, TOV0);		// aseguro que no haya ninguna interrupción por overflow
}

/************************************************************************
* Inicio PWM - Tiimer 1
************************************************************************/
void Inicio_PWM (void) {

// Fast PWM 8 bits toggle on compare
TCCR1A = 0b01010001;
TCCR1B = 0b00001001;  // Sin Prescaler  - Fast PWM, 8-bit
//TCCR1C = 0b00000000;

//DESHABILITO INTERRUPCIONES
CLEAR_BIT (TIMSK, TICIE1);
CLEAR_BIT (TIMSK, OCIE1A);
CLEAR_BIT (TIMSK, OCIE1B);
CLEAR_BIT (TIMSK, OCIE1B);

OCR1AH = 0b00000000;
OCR1AL = 0b00000000;
OCR1BH = 0b00000000;
OCR1BL = 0b00000000;

}


/************************************************************************
* Inicio Timer
************************************************************************/
/* Inicio el Timer 2 */

void Inicio_Timer2 (void) {
  TCCR2 = 0b00000010;	// prescaler 8
//	TCCR2B = 0b00000010;
	SET_BIT (TIMSK,TOIE2);	// habilito la interrupción por overflow
	CLEAR_BIT (TIFR, TOV2);		// aseguro que no haya ninguna interrupción por overflow
}
