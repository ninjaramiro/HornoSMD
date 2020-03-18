
/** \file	USART0.c
 *  \brief	Contiene las maquinas de estados correspondientes a la recepción y a la transmisión \n
 *	por USART0 del uc.\n
 *	El funcionamiento de estas rutinas entan cosevidas para que el uc se comporte principalmente\n
 *	como un DCE, debido a que estan orientadas para el uso de un hiperterminal desde una PC.
 */

#include "USART0.h"
#include "macros.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/wdt.h>

/*****************************************************************************
* Variables comunes a ambas Máquinas de estados
*****************************************************************************/
volatile unsigned int	uContTimeoutEsperaUSART0;			/**< \brief contador de timeout para espera del USART */
volatile unsigned int	uContTimeoutLecturaRxUSART0;		/**< \brief contador de timeout para espera de lectura del buffer de Rx del USART */
volatile unsigned int	uContTimeoutRxUSART0;				/**< \brief contador de timeout para espera de Rx del USART, si se vence cierro la recepcion */
volatile unsigned int	uContTimeoutTxUSART0;				/**< \brief contador de timeout para espera de transmision del buffer de Tx del USART */

/*****************************************************************************
* Variables Máquina de estados de recepción
*****************************************************************************/
unsigned char	uchEstadoRecepcionUSART0;				/**< \brief variable de la maquina de estados del recepción por USART */
char			chBufferRx_USART0[USART0_BUFFER_ZISE];	/**< \brief buffer de recepción USART del uc */
unsigned char	uchEstadoBufferRx_USART0;				/**< \brief variable de estado del buffer de recepción del UASART, lleno o vacio */

unsigned char	uchContAuxRx_USART0;					/**< \brief contador auxiliar de recepción */

/*****************************************************************************
* Variables Máquina de estados de transmisión
*****************************************************************************/
unsigned char	uchEstadoTransmisionUSART0;				/**< \brief variable de la maquina de estados del transmisión por USART */
char			chBufferTx_USART0[USART0_BUFFER_ZISE];	/**< \brief buffer de transmisión USART del uc */
unsigned char	uchEstadoBufferTx_USART0;				/**< \brief variable de estado del buffer de transmisión del USART, lleno o vacio */

unsigned char	uchContAuxTx_USART0;					/**< \brief contador auxiliar de transmisión */

/************************************************************************
* Máquina de estados de recepción por USART del uc
************************************************************************/

void Recepcion_USART_0(void) {
	switch (uchEstadoRecepcionUSART0) {
		case STANDBY:
			if (((UCSR0A & (SET<<RXC0)) == 0) || (uchEstadoBufferRx_USART0 == LLENO)) {

				if (uContTimeoutLecturaRxUSART0 == 0)
					uchEstadoBufferRx_USART0 = VACIO;

				uchEstadoRecepcionUSART0 = STANDBY;
			}
			else {
				uchContAuxRx_USART0 = 0;							
				uchEstadoRecepcionUSART0 = RECIBIR_USART0;
			}
			break;
		
		case RECIBIR_USART0:
			if (uchContAuxRx_USART0 == 0) {
				if ((UCSR0A&(SET<<FE0)) || (UCSR0A&(SET<<DOR0)) || (UCSR0A&(SET<<UPE0))) {	/* Frame Error (FE), Data OverRun (DOR) and Parity Error (UPE) */
					USART0_Flush();
					uchEstadoBufferRx_USART0 = VACIO;
					uchEstadoRecepcionUSART0 = STANDBY;
				}
				else {
					chBufferRx_USART0[uchContAuxRx_USART0] = UDR0;
					uContTimeoutRxUSART0 = TIMEOUT_RX_USART_0;

					#ifdef RECIBIR_NULL_AL_FINAL
					if  (chBufferRx_USART0[uchContAuxRx_USART0] == '\0') {	/* chequeo no haber recibido un \0, sino corto la recepción */
					#else
					if  ((chBufferRx_USART0[uchContAuxRx_USART0] == '\n') || (chBufferRx_USART0[uchContAuxRx_USART0] == '\r')) {	/* chequeo no haber recibido un \n o \r, sino corto la recepción */
						chBufferRx_USART0[uchContAuxRx_USART0 + 1] = '\r';
						chBufferRx_USART0[uchContAuxRx_USART0 + 2] = '\0';

						#ifdef TRANSMITIR_ECO
						/* escribo el enter */
						Esperar_USART_O_TX ();		/* si el registro de envio esta lleno, espero */
						UDR0 = '\n';
						Esperar_USART_O_TX ();		/* si el registro de envio esta lleno, espero */
						UDR0 = '\r';
						#endif

					#endif

						uchEstadoBufferRx_USART0 = LLENO;
						uContTimeoutLecturaRxUSART0 = TIMEOUT_LECTURA_RX_USART_0;
						uchEstadoRecepcionUSART0 = STANDBY;
					}
					else {
						#ifdef TRANSMITIR_ECO
						/* escribo el caracter que llegó*/
						Esperar_USART_O_TX ();		/* si el registro de envio esta lleno, espero */
						UDR0 = chBufferRx_USART0[uchContAuxRx_USART0];
						#endif

						uchContAuxRx_USART0 ++;
						uchEstadoRecepcionUSART0 = RECIBIR_USART0;
					}
				}				
			}
			else if ((UCSR0A & (SET<<RXC0)) == 0) {			// espero recibir el siguiente caracter
				if (uContTimeoutRxUSART0 == 0) {			// si se vence el tiempo de espera del siguiente caracter
					chBufferRx_USART0[uchContAuxRx_USART0] = '\0';
					uchEstadoBufferRx_USART0 = LLENO;
					uContTimeoutLecturaRxUSART0 = TIMEOUT_LECTURA_RX_USART_0;
					uchEstadoRecepcionUSART0 = STANDBY;
				}
				else {										// si no se vence sigo esperando el siguiente caracter
					uchEstadoRecepcionUSART0 = RECIBIR_USART0;
				}
			}
			else if (uchContAuxRx_USART0 < (USART0_BUFFER_ZISE-1)) {
				if ((UCSR0A&(SET<<FE0)) || (UCSR0A&(SET<<DOR0)) || (UCSR0A&(SET<<UPE0))) {	/* Frame Error (FE), Data OverRun (DOR) and Parity Error (UPE) */
					USART0_Flush();
					uchEstadoBufferRx_USART0 = VACIO;
					uchEstadoRecepcionUSART0 = STANDBY;
				}
				else {
					chBufferRx_USART0[uchContAuxRx_USART0] = UDR0;
					uContTimeoutRxUSART0 = TIMEOUT_RX_USART_0;

					#ifdef RECIBIR_NULL_AL_FINAL
					if  (chBufferRx_USART0[uchContAuxRx_USART0] == '\0') {	/* chequeo no haber recibido un \0, sino corto la recepción */
					#else
					if  ((chBufferRx_USART0[uchContAuxRx_USART0] == '\n') || (chBufferRx_USART0[uchContAuxRx_USART0] == '\r')) {	/* chequeo no haber recibido un \n o \r, sino corto la recepción */
						chBufferRx_USART0[uchContAuxRx_USART0 + 1] = '\r';
						chBufferRx_USART0[uchContAuxRx_USART0 + 1] = '\0';

						#ifdef TRANSMITIR_ECO
						/* escribo el enter */
						Esperar_USART_O_TX ();		/* si el registro de envio esta lleno, espero */
						UDR0 = '\n';
						Esperar_USART_O_TX (); 		/* si el registro de envio esta lleno, espero */
						UDR0 = '\r';
						#endif

					#endif

						uchEstadoBufferRx_USART0 = LLENO;
						uContTimeoutLecturaRxUSART0 = TIMEOUT_LECTURA_RX_USART_0;
						uchEstadoRecepcionUSART0 = STANDBY;
					}
					else {
						#ifdef TRANSMITIR_ECO
						/* escribo el caracter que llegó*/
						Esperar_USART_O_TX (); 		/* si el registro de envio esta lleno, espero */
						if (chBufferRx_USART0[uchContAuxRx_USART0] == 8) {
							UDR0=8;
							Esperar_USART_O_TX ();
							UDR0=' ';
							Esperar_USART_O_TX ();
							UDR0=8;
							uchContAuxRx_USART0 --;
						}
						else {
							UDR0 = chBufferRx_USART0[uchContAuxRx_USART0];
							uchContAuxRx_USART0 ++;
						}
						#else
						uchContAuxRx_USART0 ++;
						#endif
						uchEstadoRecepcionUSART0 = RECIBIR_USART0;
					}
				}
			}
			else if (uchContAuxRx_USART0 == (USART0_BUFFER_ZISE-1)) {
				chBufferRx_USART0[uchContAuxRx_USART0] = '\0';
				uchEstadoBufferRx_USART0 = LLENO;
				uContTimeoutLecturaRxUSART0 = TIMEOUT_LECTURA_RX_USART_0;
				uchEstadoRecepcionUSART0 = STANDBY;
			}						
			else {
				uchEstadoRecepcionUSART0 = RECIBIR_USART0;
			}
			break;
			
		default:
			uchEstadoBufferRx_USART0 = VACIO;
			uchEstadoRecepcionUSART0 = STANDBY;
			break;
	}	
}

/************************************************************************
* Máquina de estados de transmisión por USART del uc
************************************************************************/		
void Transmision_USART_0(void) {
	switch (uchEstadoTransmisionUSART0) {
		case STANDBY:
			if (uchEstadoBufferTx_USART0 == VACIO) {
				uchEstadoTransmisionUSART0 = STANDBY;
			}
			else {
				uchContAuxTx_USART0 = 0;
				uContTimeoutTxUSART0 = TIMEOUT_TX_USART_0;
				uchEstadoTransmisionUSART0 = TRANSMITIR_USART0;
			}
			break;
		
		case TRANSMITIR_USART0:
			if (uContTimeoutTxUSART0 == 0) {
				uchEstadoBufferTx_USART0 = VACIO;
				uchEstadoTransmisionUSART0 = STANDBY;
			}
			else if ((chBufferTx_USART0[uchContAuxTx_USART0] != '\0') && (uchContAuxTx_USART0 < (USART0_BUFFER_ZISE-1))) {
				if (!( UCSR0A & (SET<<UDRE0))) {	/* si el registro de envio esta lleno, espero */
					uchEstadoTransmisionUSART0 = TRANSMITIR_USART0;
				}
				else {
					UDR0 = chBufferTx_USART0[uchContAuxTx_USART0];
					uchContAuxTx_USART0 ++;
					uContTimeoutTxUSART0 = TIMEOUT_TX_USART_0;
					uchEstadoTransmisionUSART0 = TRANSMITIR_USART0;
				}				
			}
			else {

				#ifdef TRANSMITIR_NULL_AL_FINAL
				if (!( UCSR0A & (SET<<UDRE0))) {	/* si el registro de envio esta lleno, espero */
					uchEstadoTransmisionUSART0 = TRANSMITIR_USART0;
				}
				else {
					UDR0 = '\0';

					uchEstadoBufferTx_USART0 = VACIO;
					uchEstadoTransmisionUSART0 = STANDBY;
				}
				#else
				uchEstadoBufferTx_USART0 = VACIO;
				uchEstadoTransmisionUSART0 = STANDBY;
				#endif

			}
			break;
		
		default:
			uchEstadoBufferTx_USART0 = VACIO;
			uchEstadoTransmisionUSART0 = STANDBY;
			break;
	}		
}

/************************************************************************
* Inicio del USART
************************************************************************/
/* La velocidad de transmisión es de 38,400 kbps, 8 bit de datos, sin control de paridad y 1 bit de parada. */

void Inicio_USART_0 (void) {
	/* cargo el UBRR necesario para el baudrate escogido */
	UBRR0H = (UBRR0>>8) & BYTE_MASK;
	UBRR0L = UBRR0 & BYTE_MASK;
	
	/* limpio el flag de transmisión y no multiprocessor mode */
	UCSR0A = (OFF<<TXC0)|(VALOR_U2X0<<U2X0)|(OFF<<MPCM0);
	
	/* deshabilito la interrupción por recepción, por transmisión y por buffer vacio. Habilito la transmisión y la recepicón, 8n bit de datos y pongo en 0 el noveno bit */
	UCSR0B = (OFF<<RXCIE0)|(OFF<<TXCIE0)|(OFF<<UDRIE0)|(SET<<RXEN)|(SET<<TXEN)|(OFF<<UCSZ02)|(OFF<<TXB80);
	
	/* Modo asincronico, sin bit de paridad, un bit de stop, 8 bit de datos */ 
	UCSR0C = (OFF<<UMSEL0)|(OFF<<UPM01)|(OFF<<UPM00)|(OFF<<USBS0)|(SET<<UCSZ01)|(SET<<UCSZ00)|(OFF<<UCPOL0);
	
	uchEstadoBufferRx_USART0 = VACIO;
	uchEstadoBufferTx_USART0 = VACIO;
}

/************************************************************************
* USART vacio el buffer de recepción
************************************************************************/

void USART0_Flush(void) {
	unsigned char	uchDummy;
	unsigned int	uContFlush0 = 0;
	
	while (UCSR0A & (SET<<RXC0)) {
		uchDummy = UDR0;
		
		if (uContFlush0 == USART0_BUFFER_ZISE) {
			break;
		}
		uContFlush0 ++;
	}	
}

/************************************************************************
* Transmisión buffer conpleto por USART0 (de prepo y de corrido)
************************************************************************/

void Transmitir_Buffer_USART0 (char *chBufferPtr, int maxBufferSize) {
	wdt_reset();
	int i = 0;

	while ((*(chBufferPtr + i) != '\0') && (i < (maxBufferSize-1))) {
		Esperar_USART_O_TX (); 		/* si el registro de envio esta lleno, espero */
		UDR0 = *(chBufferPtr + i);
		i ++;
	}

#ifdef TRANSMITIR_NULL_AL_FINAL
	Esperar_USART_O_TX (); 			/* si el registro de envio esta lleno, espero */
	UDR0 = '\0';
#endif

}

/************************************************************************
* Transmisión strig desde la memoria de programa (de prepo y de corrido)
************************************************************************/

void Transmitir_Buffer_USART0_P (const char *chStringPtr) {
	wdt_reset();
	int i = 0;

	while (pgm_read_byte(chStringPtr+i) != '\0') {
		Esperar_USART_O_TX (); 		/* si el registro de envio esta lleno, espero */
		UDR0 = pgm_read_byte(chStringPtr+i);
		i ++;
	}

#ifdef TRANSMITIR_NULL_AL_FINAL
	Esperar_USART_O_TX (); 			/* si el registro de envio esta lleno, espero */
	UDR0 = '\0';
#endif

}

/************************************************************************
* Transmisión de un float por USART0
************************************************************************/

void Transmitir_Float_USART0 (float flValor, char chCantDigitos, char chDecimales) {
	wdt_reset();
	char charBuf [chCantDigitos+chDecimales];
	dtostrf(flValor, chCantDigitos, chDecimales, charBuf);
	Transmitir_Buffer_USART0 (charBuf, chCantDigitos+chDecimales);
}

/************************************************************************
* Esperar USART0 para transmitir
************************************************************************/

char Esperar_USART_O_TX (void) {
	wdt_reset();
	char chTimeout;

	uContTimeoutEsperaUSART0 = TIMEOUT_ESPERA_USART_0;

	while (((UCSR0A & (SET<<UDRE0)) == 0) && (uContTimeoutEsperaUSART0 != 0));

	(uContTimeoutEsperaUSART0 != 0) ? (chTimeout = NO) : (chTimeout = SI);

	return chTimeout;
}

/************************************************************************
* Recepcion de buffer completo por USART 0 (de prepo y de corrido)
************************************************************************/

void Recibir_Buffer_USART0 (char *chBufferPtr, int maxBufferSize) {

	int i = 0;
	char chTimeout = NO;

	while ((chTimeout == NO) && (i < (maxBufferSize-1)) && (*(chBufferPtr+i-1) != '\0') && (*(chBufferPtr+i-1) != '\n') && (*(chBufferPtr+i-1) != '\r')) {

		chTimeout = Esperar_USART_0_RX();

		if (chTimeout == NO) {
			*(chBufferPtr+i) = UDR0;
			i++;
		}
	}

	*(chBufferPtr+i) = '\0';
}

/************************************************************************
* Esperar USART0 para Recibir
************************************************************************/

char Esperar_USART_0_RX (void) {
	wdt_reset();
	char chTimeout;

	uContTimeoutEsperaUSART0 = TIMEOUT_ESPERA_USART_0;

	while (((UCSR0A & (SET<<RXC0)) == 0) && (uContTimeoutEsperaUSART0 != 0));

	(uContTimeoutEsperaUSART0 != 0) ? (chTimeout = NO) : (chTimeout = SI);

	return chTimeout;
}

