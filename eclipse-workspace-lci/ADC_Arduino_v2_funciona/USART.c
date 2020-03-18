
/** \file	USART.c
 *  \brief	Contiene las maquinas de estados correspondientes a la recepción y a la transmisión \n
 *	porUSART del uc.
 */

#include "USART.h"
#include "macros.h"
#include "inicioUc.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>


//#ifdef COMPILE_WITH_DEBUG

/*****************************************************************************
* Variables comunes a ambas Máquinas de estados
*****************************************************************************/

unsigned int	uContTimeoutUSART0;			/**< \brief contaDOR0 de timeout para esUPE0ra del USART */

/*****************************************************************************
* Variables Máquina de estados de recepción
*****************************************************************************/

unsigned char	uchEstaDOR0ecepcionUSART0;				/**< \brief variable de la maquina de estados del recepción por USART */
//char			chBUFFERx_USART0[USART0_BUFFER_ZISE];	/**< \brief BUFFER de recepción USART del uc */
unsigned char	uchEstadoBUFFERx_USART0;				/**< \brief variable de estado del BUFFER de recepción del UASART, lleno o vacio */
unsigned char	uchContAuxRx_USART0;					/**< \brief contaDOR0 auxiliar de recepción */

/*****************************************************************************
* Variables Máquina de estados de transmisión
*****************************************************************************/

unsigned char	uchEstadoTransmisionUSART0;				/**< \brief variable de la maquina de estados del transmisión por USART */
//char			chBufferTx_USART0[USART0_BUFFER_ZISE];	/**< \brief BUFFER de transmisión USART del uc */
unsigned char	uchEstadoBUFFERTx_USART0;				/**< \brief variable de estado del BUFFER de transmisión del USART, lleno o vacio */
unsigned char	uchContAuxTx_USART0;					/**< \brief contaDOR0 auxiliar de transmisión */



/*****************************************************************************
* Variables comunes a ambas Máquinas de estados
*****************************************************************************/

unsigned int	uContTimeoutEsperaUSART0;			/**< \brief contador de timeout para espera del USART */
unsigned int	uContTimeoutLecturaRxUSART0;		/**< \brief contador de timeout para espera de lectura del buffer de Rx del USART */
unsigned int	uContTimeoutRxUSART0;				/**< \brief contador de timeout para espera de Rx del USART, si se vence cierro la recepcion */
unsigned int	uContTimeoutTxUSART0;				/**< \brief contador de timeout para espera de transmision del buffer de Tx del USART */



void Inicio_USART (unsigned int baud) {

	/* Set baud rate */
//	UBRRH = (unsigned char)(baud>>8);
//	UBRRL = (unsigned char)baud;

	/*Definicion de baudrate*/


	UBRR0H = 0;
//	UBRR0L = 0;
	UBRR0L = 1; //pARA 1Mbps


//	/* Enable receiver and transmitter */
	UCSR0A = (1<<U2X0)|(0<<MPCM0);
//
//	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0)|(0<<UDRIE0)|(0<<RXCIE0)|(0<<TXCIE0)|(0<<UCSZ02);
//
//	/* Set frame format: 8data, 2stop bit */
	UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);

}

void USART_Transmit( unsigned char data ){
	/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
//	/* Put data into buffer, sends the data */
	UDR0 = data;

}


unsigned char USART_Receive( void ){
	/* Wait for data to be received */
	while ( !(UCSR0A & (1<<RXC0)) );
	/* Get and return received data from buffer */
	return UDR0;
}


///************************************************************************
//* USART vacio el BUFFER de recepción
//************************************************************************/
//
void USART_Flush(void) {
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
//


//
///************************************************************************
//* Máquina de estados de recepción por USART del uc
//*
//* cuando el buffer está lleno de datos, hay que avisar que se lleyó para poder volver a recibir
//* ejecutando la siguiente línea
//* uchEstadoBUFFERx_USART0 = VACIO;
//*
//************************************************************************/
//
//void Recepcion_USART_0(void) {
//	switch (uchEstaDOR0ecepcionUSART0) {
//		case STANDBY_USART0_RX:
//			if ((!(UCSR0A & (SET<<RXC0))) || (uchEstadoBUFFERx_USART0 == LLENO)) {     //Si no llegó ningun caracter o el buffer está lleno, me quedo en standby
//				uchEstaDOR0ecepcionUSART0 = STANDBY_USART0_RX; // Permanezco en Standby
//			}
//			else { //Recibo caracter con el buffer Vacio, paso al estado RECIBIR_USART0
//				uchEstaDOR0ecepcionUSART0 = RECIBIR_USART0;
//			}
//			break;
//
//		case RECIBIR_USART0:   // Recibo un caracter
//			uchEstaDOR0ecepcionUSART0 = STANDBY_USART0_RX;  // Luego de analizarlo vuelvo al estado standby
//
//			//Verifico que no tenga errores
//			if ((UCSR0A & (SET<<FE0)) || (UCSR0A & (SET<<DOR0)) || (UCSR0A & (SET<<UPE0))) {	/* Frame Error (FE0), Data OverRun (DOR0) and Parity Error (UUPE0) */
//				USART0_Flush();
//				uchEstadoBUFFERx_USART0 = VACIO;
//				uchContAuxRx_USART0 = 0;
//			}
//			else { //Si el caracter es valido
//				chBUFFERx_USART0[uchContAuxRx_USART0] = UDR0;   // Cargo el dato en el buffer
//
//				#ifdef ECHO_SERIE
//					Esperar_USART_O_TX (); 		/* si el registro de envio esta lleno, esUPE0ro */
//					UDR0 = chBUFFERx_USART0[uchContAuxRx_USART0];
//				#endif
//
//				if (uchContAuxRx_USART0 == 0){  //Si es el primer caracter
//					if (chBUFFERx_USART0[uchContAuxRx_USART0] != ';'){ // Si no es un punto y coma
//						uchContAuxRx_USART0 = 0;
//						uchEstadoBUFFERx_USART0 = VACIO;
//					}
//					else{
//						uchContAuxRx_USART0 ++;
//					}
//				}
//				else{ //Si no es el primer caracter
//
//					if ((chBUFFERx_USART0[uchContAuxRx_USART0] <= '9') && (chBUFFERx_USART0[uchContAuxRx_USART0] >= '0')){  // Si es un número
//
//						if (uchContAuxRx_USART0 > 7){ //Llegue al final de la trama
//							uchEstadoBUFFERx_USART0 = LLENO;
//							uchContAuxRx_USART0 = 0;
//
//			//				Transmitir_Buffer_USART0_P(PSTR("\n\rFinal de trama"));
//			//				Transmitir_Buffer_USART0 (chBUFFERx_USART0, 10);
//
//						}
//						else{
//							uchContAuxRx_USART0 ++;
//						}
//					}
//					else{  // Si no es un numero
//						uchContAuxRx_USART0 = 0;
//						uchEstadoBUFFERx_USART0 = VACIO;
//					}
//				}
//
//			}
//			break;
//
//		default:
//			uchEstadoBUFFERx_USART0 = VACIO;
//			uchEstaDOR0ecepcionUSART0 = STANDBY_USART0_RX;
//			break;
//	}
//}
//
///************************************************************************
//* Máquina de estados de transmisión por USART del uc
//************************************************************************/
//void Transmision_USART_0(void) {
//	switch (uchEstadoTransmisionUSART0) {
//		case STANDBY_USART0_TX:
//			if (uchEstadoBUFFERTx_USART0 == VACIO) {
//				uchEstadoTransmisionUSART0 = STANDBY_USART0_TX;
//			}
//			else {
//				uchContAuxTx_USART0 = 0;
//				uchEstadoTransmisionUSART0 = TRANSMITIR_USART0;
//			}
//			break;
//
//		case TRANSMITIR_USART0:
//			if ((chBufferTx_USART0[uchContAuxTx_USART0] != '\0') && (uchContAuxTx_USART0 < (USART0_BUFFER_ZISE-1))) {
//				if (!( UCSR0A & (SET<<UDRE0))) {	/* si el registro de envio esta lleno, esUPE0ro */
//					uchEstadoTransmisionUSART0 = TRANSMITIR_USART0;
//				}
//				else {
//					UDR0 = chBufferTx_USART0[uchContAuxTx_USART0];
//					uchContAuxTx_USART0 ++;
//					uchEstadoTransmisionUSART0 = TRANSMITIR_USART0;
//				}
//			}
//			else {
//				UDR0 = '\0';
//				uchEstadoBUFFERTx_USART0 = VACIO;
//				uchEstadoTransmisionUSART0 = STANDBY_USART0_TX;
//			}
//			break;
//
//		default:
//			uchEstadoBUFFERTx_USART0 = VACIO;
//			uchEstadoTransmisionUSART0 = STANDBY_USART0_TX;
//			break;
//	}
//}
//
///************************************************************************
//* Inicio del USART
//************************************************************************/
///* La velocidad de transmisión es de 115,200 kbps, 8 bit de datos, sin control de paridad y 1 bit de parada. */
//
//void Inicio_USART (unsigned int baud) {
//
//	/* Set baud rate */
//	UBRRH = (unsigned char)(baud>>8);
//	UBRRL = (unsigned char)baud;
//	/* Enable receiver and transmitter */
//	UCSRB = (1<<RXEN)|(1<<TXEN);
//	/* Set frame format: 8data, 2stop bit */
//	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
//
//}
//
//void USART_Transmit( unsigned char data ){
//	/* Wait for empty transmit buffer */
//	while ( !( UCSRA & (1<<UDRE)) );
//	/* Put data into buffer, sends the data */
//	UDR = data;
//}
//
////
////	/* cargo el UBRR necesario para el baUDR0ate escogido */
////	UBRR0H = (UBRR>>8) & BYTE_MASK;
////	UBRR0L = UBRR & BYTE_MASK;
////
////	/* limpio el flag de transmisión y no multiprocessor mode */
////	UCSR0A = (OFF<<TXC0)|(VALOR_U2X0<<U2X0)|(OFF<<MPCM0);
////
////	/* deshabilito la interrupción por recepción, por transmisión y por BUFFER vacio. Habilito la transmisión y la recepicón, 8n bit de datos y pongo en 0 el noveno bit */
////	UCSR0B = (OFF<<RXCIE0)|(OFF<<TXCIE0)|(OFF<<UDRIE0)|(SET<<RXEN0)|(SET<<TXEN0)|(OFF<<UCSZ02)|(OFF<<TXB80);
////
////	/* Modo asincronico, con bit de paridad (impar), un bit de stop, 8 bit de datos */
////	UCSR0C = (OFF<<UMSEL01)|(OFF<<UMSEL00)|(SET<<UPM01)|(SET<<UPM00)|(OFF<<USBS0)|(SET<<UCSZ01)|(SET<<UCSZ00)|(OFF<<UCPOL0);
//
//	uchEstadoBUFFERx_USART0 = VACIO;
//	uchEstadoBUFFERTx_USART0 = VACIO;
//}
//

///************************************************************************
//* Transmisión BUFFER conpleto por USART0 (de prepo y de corrido)
//************************************************************************/
//
//void Transmitir_Buffer_USART0 (char *chBUFFERPtr, int maxBUFFERSize) {
//
//	int i = 0;
//
//	while ((*(chBUFFERPtr + i) != '\0') && (i < (maxBUFFERSize-1))) {
//		Esperar_USART_O_TX (); 		/* si el registro de envio esta lleno, esUPE0ro */
//		UDR0 = *(chBUFFERPtr + i);
//		i ++;
//	}
//	Esperar_USART_O_TX (); 			/* si el registro de envio esta lleno, esUPE0ro */
//	UDR0 = '\0';
//}
//
///************************************************************************
//* Transmisión de un float por USART0
//************************************************************************/
//
//void Transmitir_Float_USART0 (float flValor, char chCantDigitos, char chDecimales) {
//	char charBuf [chCantDigitos+chDecimales];
//	dtostrf(flValor, chCantDigitos, chDecimales, charBuf);
//	Transmitir_Buffer_USART0 (charBuf, chCantDigitos+chDecimales);
//}
//
///************************************************************************
//* Esperar USART0 para transmitir
//************************************************************************/
//
//void Esperar_USART_O_TX (void) {
//
//	uContTimeoutUSART0 = TIMEOUT_USART_0;
//
//	while (!(UCSR0A & (SET<<UDRE0)) && (uContTimeoutUSART0 != 0));
//}
//
//
///************************************************************************
//* Transmisión string desde la memoria de programa (de prepo y de corrido)
//************************************************************************/
//
//void Transmitir_Buffer_USART0_P (const char *chStringPtr) {
//
//	int i = 0;
//
//	while (pgm_read_byte(chStringPtr+i) != '\0') {
//		Esperar_USART_O_TX (); 		/* si el registro de envio esta lleno, espero */
//		UDR0 = pgm_read_byte(chStringPtr+i);
//		i ++;
//	}
//
//#ifdef TRANSMITIR_NULL_AL_FINAL
//	Esperar_USART_O_TX (); 			/* si el registro de envio esta lleno, espero */
//	UDR0 = '\0';
//#endif
//
//}
//
//
///************************************************************************
//* Transmite un caracter, char, uint8_t
//************************************************************************/
//
//void Transmitir_Uint8_USART0(uint8_t c)
//{
//    if(c == '\n')
//        Transmitir_Uint8_USART0('\r');
//
//    /* wait until transmit buffer is empty */
//    while(!(UCSR0A & (1 << UDRE0)));
//
//    /* send next byte */
//    UDR0 = c;
//}
//
///************************************************************************
//* Transmite el valor hexadecimal en ASCII de un char, uint8_t
//************************************************************************/
//
//void Transmitir_Uint8_Hex_USART0(uint8_t b)
//{
//    /* upper nibble */
//    if((b >> 4) < 0x0a)
//        Transmitir_Uint8_USART0((b >> 4) + '0');
//    else
//        Transmitir_Uint8_USART0((b >> 4) - 0x0a + 'a');
//
//    /* lower nibble */
//    if((b & 0x0f) < 0x0a)
//        Transmitir_Uint8_USART0((b & 0x0f) + '0');
//    else
//        Transmitir_Uint8_USART0((b & 0x0f) - 0x0a + 'a');
//}
//
///************************************************************************
//* Transmite el valor hexadecimal en ASCII de un int, uint16_t
//************************************************************************/
//
//void Transmitir_Uint16_Hex_USART0(uint16_t w)
//{
//    Transmitir_Uint8_Hex_USART0((uint8_t) (w >> 8));
//    Transmitir_Uint8_Hex_USART0((uint8_t) (w & 0xff));
//}
//
///************************************************************************
//* Transmite el valor hexadecimal en ASCII de un long, uint32_t
//************************************************************************/
//
//void Transmitir_Uint32_Hex_USART0(uint32_t dw)
//{
//    Transmitir_Uint16_Hex_USART0((uint16_t) (dw >> 16));
//    Transmitir_Uint16_Hex_USART0((uint16_t) (dw & 0xffff));
//}
//
///************************************************************************
//* Transmite el valor decimal en ASCII de un int, uint16_t
//************************************************************************/
//
//void Transmitir_Uint16_Dec_USART0(uint16_t w)
//{
//    uint16_t num = 10000;
//    uint8_t started = 0;
//
//    while(num > 0)
//    {
//        uint8_t b = w / num;
//        if(b > 0 || started || num == 1)
//        {
//            Transmitir_Uint8_USART0('0' + b);
//            started = 1;
//        }
//        w -= b * num;
//
//        num /= 10;
//    }
//}
//
///************************************************************************
//* Transmite el valor decimal en ASCII de un long, uint32_t
//************************************************************************/
//
//void Transmitir_Uint32_Dec_USART0(uint32_t dw)
//{
//    uint32_t num = 1000000000;
//    uint8_t started = 0;
//
//    while(num > 0)
//    {
//        uint8_t b = dw / num;
//        if(b > 0 || started || num == 1)
//        {
//            Transmitir_Uint8_USART0('0' + b);
//            started = 1;
//        }
//        dw -= b * num;
//
//        num /= 10;
//    }
//}
//
//
//
//






















//#endif //COMPILE_WITH_DEBUG
