/** \file	USART1.c
 *  \brief	Contiene las maquinas de estados correspondientes a la recepción y a la transmisión \n
 *	por USART1 del uc.\n
 *	El funcionamiento de estas rutinas entan cosevidas para que el uc se comporte principalmente\n
 *	como un DTE, debido a que están orientadas para el uso con el modulo GSM.
 */

#include "USART1.h"
#include "USART0.h"
#include "macros.h"
#include "inicioUc.h"
#include "utiles.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/wdt.h>

/*****************************************************************************
* Variables comunes a ambas Máquinas de estados
*****************************************************************************/

volatile unsigned int	uContTimeoutLecturaRxUSART1;		/**< \brief contador de timeout para espera de lectura del buffer de Rx del USART */
volatile unsigned int	uContTimeoutRxUSART1;				/**< \brief contador de timeout para espera de Rx del USART, si se vence cierro la recepcion */
volatile unsigned int	uContTimeoutTxUSART1;				/**< \brief contador de timeout para espera de transmision del buffer de Tx del USART */

/*****************************************************************************
* Variables Máquina de estados de recepción
*****************************************************************************/

unsigned char	uchEstadoRecepcionUSART1;				/**< \brief variable de la maquina de estados del recepción por USART */
char			chBufferRx_USART1[USART1_BUFFER_ZISE];	/**< \brief buffer de recepción USART del uc */
unsigned char	uchEstadoBufferRx_USART1;				/**< \brief variable de estado del buffer de recepción del UASART, lleno o vacio */

unsigned char	uchContAuxRx_USART1;					/**< \brief contador auxiliar de recepción */

/*****************************************************************************
* Variables Máquina de estados de transmisión
*****************************************************************************/

unsigned char	uchEstadoTransmisionUSART1;				/**< \brief variable de la maquina de estados del transmisión por USART */
char			chBufferTx_USART1[USART1_BUFFER_ZISE];	/**< \brief buffer de transmisión USART del uc */
unsigned char	uchEstadoBufferTx_USART1;				/**< \brief variable de estado del buffer de transmisión del USART, lleno o vacio */

unsigned char	uchContAuxTx_USART1;					/**< \brief contador auxiliar de transmisión */

/************************************************************************
* Máquina de estados de recepción por USART del uc
************************************************************************/

void Recepcion_USART_1(void) {
	switch (uchEstadoRecepcionUSART1) {
		case STANDBY:
			if (((UCSR1A & (SET<<RXC1)) == 0) || (uchEstadoBufferRx_USART1 == LLENO)) {
				if (uContTimeoutLecturaRxUSART1 == 0) {
					uchEstadoBufferRx_USART1 = VACIO;
				}
			}
			else if (((UCSR1A & (SET<<RXC1)) != 0) && (uchEstadoBufferRx_USART1 == LLENO)) {
				USART1_Flush();
			}
			else {
				uchContAuxRx_USART1 = 0;
				uchEstadoRecepcionUSART1 = RECIBIR_USART1;
			}
			break;
		
		case RECIBIR_USART1:
			if (uchContAuxRx_USART1 == 0) {
				if ((UCSR1A&(SET<<FE1)) || (UCSR1A&(SET<<DOR1)) || (UCSR1A&(SET<<UPE1))) {	/* Frame Error (FE), Data OverRun (DOR) and Parity Error (UPE) */
					USART1_Flush();
					uchEstadoBufferRx_USART1 = VACIO;
					uchEstadoRecepcionUSART1 = STANDBY;
				}
				else {
					chBufferRx_USART1[uchContAuxRx_USART1] = UDR1;
					uContTimeoutRxUSART1 = TIMEOUT_RX_USART_1;
					uchContAuxRx_USART1 ++;
				}
			}
			else if ((UCSR1A & (SET<<RXC1)) == 0) {
				if (uContTimeoutRxUSART1 == 0) {								// si se vence el tiempo de espera del siguiente caracter
					chBufferRx_USART1[uchContAuxRx_USART1] = '\0';
					uchEstadoBufferRx_USART1 = LLENO;
					uContTimeoutLecturaRxUSART1 = TIMEOUT_LECTURA_RX_USART_1;
					uchEstadoRecepcionUSART1 = STANDBY;
				}
			}
			else if (uchContAuxRx_USART1 < (USART1_BUFFER_ZISE-1)) {
				if ((UCSR1A&(SET<<FE1)) || (UCSR1A&(SET<<DOR1)) || (UCSR1A&(SET<<UPE1))) {	/* Frame Error (FE), Data OverRun (DOR) and Parity Error (UPE) */
					USART1_Flush();
					uchEstadoBufferRx_USART1 = VACIO;
					uchEstadoRecepcionUSART1 = STANDBY;
				}
				else {
					chBufferRx_USART1[uchContAuxRx_USART1] = UDR1;
					uContTimeoutRxUSART1 = TIMEOUT_RX_USART_1;
					uchContAuxRx_USART1 ++;
				}
			}
			else if (uchContAuxRx_USART1 == (USART1_BUFFER_ZISE-1)) {
				chBufferRx_USART1[uchContAuxRx_USART1] = '\0';
				uchEstadoBufferRx_USART1 = LLENO;
				uContTimeoutLecturaRxUSART1 = TIMEOUT_LECTURA_RX_USART_1;
				uchEstadoRecepcionUSART1 = STANDBY;
			}
			break;
			
		default:
			uchEstadoBufferRx_USART1 = VACIO;
			uchEstadoRecepcionUSART1 = STANDBY;
			break;
	}	
}

/************************************************************************
* Máquina de estados de transmisión por USART del uc
************************************************************************/

void Transmision_USART_1(void) {
	switch (uchEstadoTransmisionUSART1) {
		case STANDBY:
			if ((uchEstadoBufferTx_USART1 == LLENO) && (uchEstadoRecepcionUSART1 == STANDBY) && ((UCSR1A & (SET<<RXC1)) == 0)) {
				uchContAuxTx_USART1 = 0;
				uContTimeoutTxUSART1 = TIMEOUT_TX_USART_1;
				uchEstadoTransmisionUSART1 = TRANSMITIR_USART1;
			}
			break;
		
		case TRANSMITIR_USART1:
			if (uContTimeoutTxUSART1 == 0) {
				uchEstadoBufferTx_USART1 = VACIO;
				uchEstadoTransmisionUSART1 = STANDBY;
			}
			else if ((chBufferTx_USART1[uchContAuxTx_USART1] != '\0') && (uchContAuxTx_USART1 < (USART1_BUFFER_ZISE-1))) {
				if (( UCSR1A & (SET<<UDRE1)) != 0) {	/* si el registro de envio esta vacio */
					UDR1 = chBufferTx_USART1[uchContAuxTx_USART1];
					uchContAuxTx_USART1 ++;
					uContTimeoutTxUSART1 = TIMEOUT_TX_USART_1;
				}				
			}
			else {
				uchEstadoBufferTx_USART1 = VACIO;
				uchEstadoTransmisionUSART1 = STANDBY;
			}
			break;
		
		default:
			uchEstadoBufferTx_USART1 = VACIO;
			uchEstadoTransmisionUSART1 = STANDBY;
			break;
	}		
}

/************************************************************************
* Inicio del USART
************************************************************************/
/* La velocidad de transmisión es de 38400 kbps, 8 bit de datos, sin control de paridad y 1 bit de parada. */

void Inicio_USART_1 (void) {
	/* cargo el UBRR necesario para el baudrate escogido */
	UBRR1H = (UBRR1>>8) & BYTE_MASK;
	UBRR1L = UBRR1 & BYTE_MASK;
	
	/* limpio el flag de transmisión y no multiprocessor mode */
	UCSR1A = (OFF<<TXC1)|(VALOR_U2X1<<U2X1)|(OFF<<MPCM1);
	
	/* deshabilito la interrupción por recepción, por transmisión y por buffer vacio. Habilito la transmisión y la recepicón, 8n bit de datos y pongo en 0 el noveno bit */
	UCSR1B = (OFF<<RXCIE1)|(OFF<<TXCIE1)|(OFF<<UDRIE1)|(SET<<RXEN)|(SET<<TXEN)|(OFF<<UCSZ12)|(OFF<<TXB81);
	
	/* Modo asincronico, sin bit de paridad, dos bits de stop, 8 bit de datos */
	UCSR1C = (OFF<<UMSEL1)|(OFF<<UPM11)|(OFF<<UPM10)|(ON<<USBS1)|(SET<<UCSZ11)|(SET<<UCSZ10)|(OFF<<UCPOL1);
	
	/* pongo el 485 para transmitir */
	SET_BIT(SERIAL_422485_PORT, SELECT_422485);

	uchEstadoBufferRx_USART1 = VACIO;
	uchEstadoBufferTx_USART1 = VACIO;
}

/************************************************************************
* USART vacio el buffer de recepción
************************************************************************/

void USART1_Flush(void) {
	unsigned char	uchDummy;
	unsigned int	uContFlush1 = 0;
	
	while (UCSR1A & (SET<<RXC1)) {
		uchDummy = UDR1;
		
		if (uContFlush1 == USART1_BUFFER_ZISE)
			break;

		uContFlush1 ++;
	}	
}
