
/** \file	inicioUc.h
 *  \brief	Contiene los prototipos correspondientes a las funciones de inicio de los perisfericos \n
 *	del uc. Asi como tambien a funciones comunes a diferentes maquinas de estado.
 */

#ifndef _INICIO_UC_H
#define _INICIO_UC_H

#include <avr/io.h>

/*****************************************************************************
* Prototipos
*****************************************************************************/

/** \brief	funcion que inicia los puertos, que no se inician cada maquina de estados, segun sus \n
 *	aplicaciones. 
 */
void Inicio_Puertos (void);

/** \brief	funcion que inicia el timer 0 como marcador de intervalos de 1ms generando una \n
 *	interrupcion. 
 */
void Inicio_Timer0 (void);

/** \brief	funcion que inicia el buzzer
 */
void Inicio_Buzzer (void);

/*****************************************************************************
* Definicion de los nombres de los puertos 
*****************************************************************************/

/* Puerto A */
#define TECLADO_PORT			PORTA
#define TECLADO_PIN				PINA
#define DIR_TECLADO_PORT 		DDRA
#define	PIN_TECLADO_1			PA0
#define	PIN_TECLADO_2			PA1
#define	PIN_TECLADO_3			PA2
#define	PIN_TECLADO_4			PA3
#define	PIN_TECLADO_5			PA4
#define	PIN_TECLADO_6			PA5

//	PIN_NOT_IN_USE				PA6
//	PIN_NOT_IN_USE				PA7


/* Puerto B */
//	PIN_NOT_IN_USE				PB0
//	SCK							PB1
//	PIN_NOT_IN_USE				PB2

#define LCD_BACKLIGHT_PORT		PORTB
#define DIR_LCD_BACKLIGHT_PORT	DDRB
#define LCD_BACKLIGHT			PB3

//	PIN_NOT_IN_USE				PB4

#define PWM_420MA_PORT			PORTB
#define DIR_PWM_420MA_PORT		DDRB
#define PWM_2_420MA				PB5
#define PWM_1_420MA				PB6

#define PULSOS_COND_PORT		PORTB
#define DIR_PULSOS_COND_PORT	DDRB
#define PULSOS_COND				PB7


/* Puerto C */
#define LCD_DB_PORT				PORTC
#define DIR_LCD_DB_PORT			DDRC
#define LCD_DB_7				PC0
#define LCD_DB_6				PC1
#define LCD_DB_5				PC2
#define LCD_DB_4				PC3

//	PIN_NOT_IN_USE				PC4
//	PIN_NOT_IN_USE				PC5
//	PIN_NOT_IN_USE				PC6
//	PIN_NOT_IN_USE				PC7


/* Puerto D */
//	PIN_NOT_IN_USE				PD0
//	PIN_NOT_IN_USE				PD1

#define SERIAL_422485_PORT		PORTD
#define DIR_SERIAL_422485_PORT	DDRD
#define RXD_422485				PD2
#define TXD_422485				PD3

//	PIN_NOT_IN_USE				PD4

#define SELECT_422485			PD5

#define LCD_RS_PORT				PORTD
#define DIR_LCD_RS_PORT			DDRD
#define LCD_CONT_RS				PD6

#define LCD_RW_PORT				PORTD
#define DIR_LCD_RW_PORT			DDRD
#define LCD_CONT_RW				PD7


/* Puerto E */
#define	SERIAL_PORT				PORTE
#define	DIR_SERIAL_PORT			DDRE
#define	SERIAL_RX				PE0
#define	SERIAL_TX				PE1

//	PIN_NOT_IN_USE				PE2

#define	BUZZER_PORT				PORTE
#define	DIR_BUZZER_PORT			DDRE
#define BUZZER					PE3

#define	SALIDA_DIG_PORT			PORTE
#define	DIR_SALIDA_DIG_PORT		DDRE
#define SALIDA_DIG_2			PE4
#define SALIDA_DIG_1			PE5

//	PIN_NOT_IN_USE				PE6
//	PIN_NOT_IN_USE				PE7


/* Puerto F */
#define ADC_420MA_PORT			PORTF
#define DIR_ADC_420MA_PORT		DDRF
#define ADC_1_420MA_P			PF0
#define ADC_1_420MA_N			PF1
#define ADC_2_420MA_P			PF2
#define ADC_2_420MA_N			PF3

//	PIN_NOT_IN_USE				PF4
//	PIN_NOT_IN_USE				PF5

#define ADC_CONDUCT_PORT		PORTF
#define DIR_ADC_CONDUCT_PORT	DDRF
#define	ADC_CONDUCT_1			PF6
#define ADC_CONDUCT_2			PF7


/* Puerto G*/
//	PIN_NOT_IN_USE				PG0
//	PIN_NOT_IN_USE				PG1
//	PIN_NOT_IN_USE				PG2
//	PIN_NOT_IN_USE				PG3

#define LCD_ENB_PORT			PORTG
#define DIR_LCD_ENB_PORT		DDRG
#define LCD_ENB					PG4


/*****************************************************************************
* Definiciones varias 
*****************************************************************************/

#define BUZZER_OFF				1
#define BUZZER_INTERMITENTE		2
#define BUZZER_LARGO			3
#define BUZZER_CORTO			4
#define BUZZER_TIEMPO_ON_LARGO	5000
#define BUZZER_TIEMPO_ON_CORTO	300
#define BUZZER_TIEMPO_ON		1000
#define BUZZER_TIEMPO_OFF		400

#define FRECUECNIA_INT_TIMER_0	1000UL	/**< \brief frecuecia de interrupción del timer 0 */
#define TIMER0_A_SEGUNDOS		1000UL	/**< \brief multiplicador/divisor para pasar el timer 0 a segundos */
#define TIMER0_A_MINUTOS		60000UL	/**< \brief multiplicador/divisor para pasar el timer 0 a minutos */

#endif /* _INICIO_UC_H */
