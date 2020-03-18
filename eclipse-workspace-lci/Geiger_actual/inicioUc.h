
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

/** \brief Inicio el Timer para generar 1kHz para la medicion de los conductimetros
 *
 */
void Inicio_Timer1 (void);



/*****************************************************************************
* Definicion de los nombres de los puertos 
*****************************************************************************/

/* Puerto A */


#define LCD_DB_PORT				PORTA
#define DIR_LCD_DB_PORT			DDRA
#define LCD_DB_7				PA7
#define LCD_DB_6				PA6
#define LCD_DB_5				PA5
#define LCD_DB_4				PA4
#define LCD_DB_3				PA3
#define LCD_DB_2				PA2
#define LCD_DB_1				PA1
#define LCD_DB_0				PA0

/* Puerto B */

#define LCD_RS_PORT				PORTB
#define DIR_LCD_RS_PORT			DDRB
#define LCD_CONT_RS				PB2

#define LCD_RW_PORT				PORTB
#define DIR_LCD_RW_PORT			DDRB
#define LCD_CONT_RW				PB1

#define LCD_ENB_PORT			PORTB
#define DIR_LCD_ENB_PORT		DDRB
#define LCD_ENB					PB0

#define SPI_PORT				PORTB
#define DIR_SPI_PORT			DDRB
#define MOSI_PIN				PB5
#define MISO_PIN				PB6
#define SCK_PIN					PB7


/* Puerto C */
#define BOT_PORT				PORTC
#define BOT_PIN					PINC
#define DIR_BOT_PORT			DDRC
#define BOT_DISP_PIN			PC0
#define BOT_RESET_PIN			PC1
#define BOT_RUN_STOP_PIN		PC2
#define BOT_INJ_ERR_PIN			PC3
#define BOT_PARAM_PIN			PC4
#define BOT_DOWN_PIN			PC5
#define BOT_UP_PIN				PC6


/* Puerto D */
#define SERIAL_PORT				PORTD
#define DIR_SERIAL_PORT			DDRD
#define SERIAL_RX				PD0
#define SERIAL_TX				PD1

#define PULSO_PORT				PORTD
#define DIR_PULSO_PORT 			DDRD
#define PULSO_PIN				PD2

#define BUZZER_PORT				PORTD
#define DIR_BUZZER_PORT 		DDRD
#define BUZZER					PD7


// NO USADOS
#define NC_B_PORT				PORTB
#define DIR_NC_B_PORT			DDRB
#define NC1_PIN					PB3
#define NC2_PIN					PB4

#define NC_C_PORT				PORTC
#define DIR_NC_C_PORT			DDRC
#define NC3_PIN					PC7

#define NC_D_PORT				PORTD
#define DIR_NC_D_PORT			DDRD
#define NC4_PIN					PD3
#define NC5_PIN					PD4
#define NC6_PIN					PD5
#define NC7_PIN					PD6

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
