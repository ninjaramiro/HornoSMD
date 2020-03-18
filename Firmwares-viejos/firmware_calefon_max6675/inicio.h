
/** \file	inicio.h
 *  \brief	Contiene los prototipos correspondientes a las funciones de inicio de los perisfericos \n
 *	del uc. Asi como tambien a funciones comunes a diferentes maquinas de estado.
 */

#ifndef _INICIO_UC_H
#define _INICIO_UC_H

/*****************************************************************************
* Prototipos
*****************************************************************************/

/** \brief	funcion que inicia los puertos, que no se inician cada maquina de estados, segun sus \n
 *	aplicaciones.
 */
void Inicio_Puertos (void);

/** \brief	funcion que inicia el timer 2 como marcador de intervalos generando una \n
 *	interrupcion.
 */
void Inicio_Timer2 (void);

/*****************************************************************************
* Definicion de los nombres de los puertos
*****************************************************************************/

/* Puerto B */
#define	CAUDALIMETRO_PORT			PORTB
#define DIR_CAUDALIMETRO_PORT		DDRB
#define CAUDALIMETRO_PIN			PB0

#define	LCD_E_PORT					PORTB
#define DIR_LCD_E_PORT				DDRB
#define LCD_E_PIN					PB1

#define	TEMP_AGUA_PORT				PORTB
#define	TEMP_AGUA_PIN_PORT			PINB
#define DIR_TEMP_AGUA_PORT			DDRB
#define TEMP_AGUA_PIN				PB2

//#define LLAMA_PORT				PORTB
//#define LLAMA_PIN_PORT			PINB
//#define DIR_LLAMA_PORT			DDRB
//#define LLAMA_PIN					PB2

#define SPI_PORT					PORTB
#define DIR_SPI_PORT				DDRB
#define MOSI_PIN					PB3
#define MISO_PIN					PB4
#define SCK_PIN						PB5

//XTAL1								PB6
//XTAL2								PB7

/* Puerto C */
#define	MOTOR_PP_PORT				PORTC
#define DIR_MOTOR_PP_PORT			DDRC
#define MOTOR_PP_PIN_1				PC0
#define MOTOR_PP_PIN_2				PC1
#define MOTOR_PP_PIN_3				PC2
#define MOTOR_PP_PIN_4				PC3

#define RELAY_GAS_PORT				PORTC
#define DIR_RELAY_GAS_PORT			DDRC
#define RELAY_GAS_PIN				PC4

#define RELAY_CARGA_PORT			PORTC
#define RELAY_CARGA_PIN_PORT		PINC
#define DIR_RELAY_CARGA_PORT		DDRC
#define RELAY_CARGA_PIN				PC5

#define TENSION_BAT_PORT			PORTC
#define DIR_TENSION_BAT_PORT		DDRC
#define TENSION_BAT_PIN				PC6	// esta pata es solo ADC, pero la llamo como si estuviera en el puerto por prolijidad

#define SET_TEMP_PORT				PORTC
#define DIR_SET_TEMP_PORT			DDRC
#define SET_TEMP_PIN				PC7 // esta pata es solo ADC, pero la llamo como si estuviera en el puerto por prolijidad

/* Puerto D */
#define SERIAL_PORT					PORTD
#define DIR_SERIAL_PORT				DDRD
#define SERIAL_RXD_PIN				PD0
#define SERIAL_TXD_PIN				PD1

#define DISPLAY_PORT				PORTD
#define DIR_DISPLAY_PORT			DDRD
#define DISPLAY_RW					PD2
#define DISPLAY_RS					PD3
#define DISPLAY_DB0					PD4
#define DISPLAY_DB1					PD5
#define DISPLAY_DB2					PD6
#define DISPLAY_DB3					PD7

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define TIMER2_A_SEGUNDOS			1000		/**< \brief multiplicador para llevar el timer 2 a segundos ( = frec. [Hz]) */

#endif /* _INICIO_UC_H */
