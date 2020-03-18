
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


// PUERTO A
#define	CS2_PORT				PORTA
#define	DIR_CS2_PORT		DDRA
#define CS2					    PA0

#define ADC_PORT			  PORTA
#define DIR_ADC_PORT		DDRA
#define ADC_1			      PA1
#define ADC_2			      PA2
#define ADC_3			      PA3

#define LCD_DB_PORT				  PORTA
#define DIR_LCD_DB_PORT			DDRA
#define LCD_DB_7				    PA7
#define LCD_DB_6				    PA6
#define LCD_DB_5				    PA5
#define LCD_DB_4				    PA4

// PUERTO B
#define LCD_RS_PORT				PORTB
#define DIR_LCD_RS_PORT		DDRB
#define LCD_CONT_RS				PB0

#define LCD_RW_PORT				PORTB
#define DIR_LCD_RW_PORT		DDRB
#define LCD_CONT_RW				PB1

#define LCD_ENB_PORT			PORTB
#define DIR_LCD_ENB_PORT	DDRB
#define LCD_ENB					  PB2

#define LCD_BACKLIGHT_PORT		PORTB
#define DIR_LCD_BACKLIGHT_PORT	DDRB
#define LCD_BACKLIGHT			PB3

#define	CS3_PORT				PORTB
#define	DIR_CS3_PORT		DDRB
#define CS3					    PB4

#define SPI_PORT			  PORTB
#define DIR_SPI_PORT		DDRB
#define MOSI			      PB5
#define MISO			      PB6
#define SCK			        PB7


// PUERTO C

#define TECLADO_PORT			PORTC
#define TECLADO_PIN				PINC
#define DIR_TECLADO_PORT 	DDRC
#define	PIN_TECLADO_1			PC0
#define	PIN_TECLADO_2			PC1
#define	PIN_TECLADO_3			PC2
#define	PIN_TECLADO_4			PC3
#define	PIN_TECLADO_5			PC4
#define	PIN_TECLADO_6			PC5
#define	PIN_TECLADO_7			PC6
#define	PIN_TECLADO_8			PC7



// PUERTO D

#define	SERIAL_PORT				PORTD
#define	DIR_SERIAL_PORT		DDRD
#define	SERIAL_RX				  PD0
#define	SERIAL_TX				  PD1

#define	CS1_PORT				PORTD
#define	DIR_CS1_PORT		DDRD
#define CS1					    PD2

#define	BUZZER_PORT				PORTD
#define	DIR_BUZZER_PORT		DDRD
#define BUZZER					  PD3

#define PWM_PORT			 PORTD
#define DIR_PWM_PORT	 DDRD
#define PWM_1				   PD5
#define PWM_2				   PD4

#define	LEDS_PORT			    PORTD
#define	DIR_LEDS_PORT		  DDRD
#define LED_1			        PD4
#define LED_2			        PD7




























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
