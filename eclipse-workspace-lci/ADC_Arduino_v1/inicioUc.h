/*


BOTONERA:

ARDUINO     COLOR/CABLE     FUNCION             PUERTO AVR
---------------------------------------
            ROJO/AZUL       GND
D6          ROJO            ARRANQUE            PD6
D7          AZUL            PARADA              PD7
D8          AMARILLO        LED_FRENTE (R=330)  PB0


ADC:

ARDUINO     COLOR/CABLE     FUNCION
---------------------------------------
D2          VERDE           CLOCK_MUESTREO      PD2
D13         VIOLETA         CLK_SPI             PB5
D11         ROSA            MOSI_SPI            PB3
            NEGRO           GND
D12         VERDE/BCO       MISO_CANAL_A (SPI)  PB4
D3          VERDE/MARRON    MISO_CANAL_B        PD3

*/



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
void Inicio_Timer2 (void);

/** \brief	funcion que inicia el buzzer
 */
void Inicio_Buzzer (void);

/** \brief	funcion que inicia la interrupcion 0
 */
void Inicio_Interrupcion0 (void);

/*****************************************************************************
* Definicion de los nombres de los puertos 
*****************************************************************************/

/* Puerto B */

#define LED_PORT				PORTB
#define DIR_LED_PORT			DDRB
#define LED						PB5
#define LED_FRENTE				PB1

//#define LED_PORT				PORTD
//#define DIR_LED_PORT			DDRD
//#define LED						PD5
//#define LED_FRENTE				PD5

#define SPI_PORT				PORTB
#define DIR_SPI_PORT			DDRB
#define MOSI_PIN				PB3
#define MISO_PIN				PB4
#define SCK_PIN					PB5

#define SPI_PORT_2				PORTD
#define DIR_SPI_PORT_2			DDRD
#define MISO_PIN_2				PD3

/* Puerto C */




/* Puerto D */

#define BOT1_PORT				PORTD
#define DIR_BOT1_PORT			DDRD
#define BOT1_PIN				PD7

#define BOT2_PORT				PORTD
#define DIR_BOT2_PORT			DDRD
#define BOT2_PIN				PD6


#define INT0_PORT				PORTD
#define DIR_INT0_PORT 			DDRD
#define INT0_PIN				PD2

#define SERIAL_PORT				PORTD
#define DIR_SERIAL_PORT			DDRD
#define SERIAL_RX				PD0
#define SERIAL_TX				PD1



// NO CONECTADOS
#define NC_B_PORT				PORTB
#define DIR_NC_B_PORT			DDRB
#define NC_B0_PIN				PB0
#define NC_B1_PIN				PB1
#define NC_B2_PIN				PB2
#define NC_B6_PIN				PB6
#define NC_B7_PIN				PB7

#define NC_C_PORT				PORTC
#define DIR_NC_C_PORT			DDRC
#define NC_C0_PIN				PC0
#define NC_C1_PIN				PC1
#define NC_C2_PIN				PC2
#define NC_C3_PIN				PC3
#define NC_C4_PIN				PC4
#define NC_C5_PIN				PC5

#define NC_D_PORT				PORTD
#define DIR_NC_D_PORT			DDRD
//#define NC_D3_PIN				PD3
#define NC_D4_PIN				PD4
#define NC_D5_PIN				PD5


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
