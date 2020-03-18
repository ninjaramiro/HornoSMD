/** \file	teclado.c
 *  \brief	Contiene la maquina de estados correspondientes a la lectura de la botonera \n
 */

#include "discretas.h"
#include "macros.h"
#include "inicioUc.h"
#include "USART0.h"

#include <avr/io.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

/*****************************************************************************
* Variables Máquina de estados de lectura de botonera
*****************************************************************************/
unsigned char			uchEstadoLeerEntradas;							/**< \brief variable de la maquina de estados de lectura de entradas discretas */
volatile unsigned int	uEstadoEntradasInstantaneas;					/**< \brief en cada bit guarda el estado instantaneo de cada entrada */
volatile unsigned int 	uValorIntegracionEntradas[CANTIDAD_ENTRADAS];	/**< \brief guarda el tiempo de integracion de cada entrada para detectar activacion */
volatile unsigned int	uTiempoIntegracionEntradas;						/**< \brief tiempo durante el cual se integra el valor de las entradas */
volatile unsigned int	uEstadoEntradas;								/**< \brief estado de cada entrada luego de la integración */
volatile unsigned int	uFlagEntradasActivadas;							/**< \brief cada bit corresponde con la activacion de una entrada, el flag debe borrarse en la lectura */
volatile unsigned int	uFlagEntradasLiberadas;							/**< \brief cada bit corresponde con la liberacion de una entrada, el flag debe borrarse en la lectura */
volatile unsigned int	uTimeoutEntradas;								/**< \brief variable para el timeout de flag no limpiados de entradas detectadas */
char					chPrintDiscretas;								/**< \brief variable que indica la impresion del estado de las entradas por puerto serie */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern char					chBufferTx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de transmisión USART del uc */

/*****************************************************************************
* Mensajes para debug en memoria de programa
*****************************************************************************/
const char uEstadoBotones_MSG[]				PROGMEM = "Estado: \0";
const char uFlagBotonesPresionados_MSG[]	PROGMEM = ";  Activadas: \0";
const char uFlagBotonesLiberados_MSG[]		PROGMEM = ";  Liberadas: \0";

/************************************************************************
* Máquina de estados de lectura de la botonera
************************************************************************/
void Leer_Entradas(void) {
	switch (uchEstadoLeerEntradas) {

		case BARRER_ENTRADAS:
			uEstadoEntradasInstantaneas = 0;

				((TECLADO_PIN & (SET<<PIN_TECLADO_1)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_1)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_1));
				((TECLADO_PIN & (SET<<PIN_TECLADO_2)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_2)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_2));
				((TECLADO_PIN & (SET<<PIN_TECLADO_3)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_3)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_3));
				((TECLADO_PIN & (SET<<PIN_TECLADO_4)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_4)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_4));
				((TECLADO_PIN & (SET<<PIN_TECLADO_5)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_5)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_5));
				((TECLADO_PIN & (SET<<PIN_TECLADO_6)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_6)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_6));

				if (uTimeoutEntradas == 0) {			/* si se vence el tiempo de lectura limpio los flag */
					uFlagEntradasActivadas = 0;
					uFlagEntradasLiberadas = 0;
				}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_1;
			break;


		case ENTRADA_TECLADO_1:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_1)) != 0){

				// boton presionado! - hacer aca -

				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_1);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_1)) != 0){

				// boton soltado! - hacer aca -

				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_1);
			}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_2;
			break;


		case ENTRADA_TECLADO_2:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_2)) != 0){

				// boton presionado! - hacer aca -

				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_2);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_2)) != 0){

				// boton soltado! - hacer aca -

				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_2);
			}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_3;
			break;


		case ENTRADA_TECLADO_3:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_3)) != 0){

				// boton presionado! - hacer aca -

				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_3);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_3)) != 0){

				// boton soltado! - hacer aca -

				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_3);
			}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_4;
			break;


		case ENTRADA_TECLADO_4:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_4)) != 0){

				// boton presionado! - hacer aca -

				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_4);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_4)) != 0){

				// boton soltado! - hacer aca -

				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_4);
			}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_5;
			break;

		case ENTRADA_TECLADO_5:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_5)) != 0){

				// boton presionado! - hacer aca -

				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_5);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_5)) != 0){

				// boton soltado! - hacer aca -

				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_5);
			}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_6;
			break;


		case ENTRADA_TECLADO_6:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_6)) != 0){

				// boton presionado! - hacer aca -

				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_6);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_6)) != 0){

				// boton soltado! - hacer aca -

				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_6);
			}

			uchEstadoLeerEntradas = IMPRIMIR_DISCRETAS;
			break;


		case IMPRIMIR_DISCRETAS:
			if (chPrintDiscretas == SI){
				int index = sprintf(chBufferTx_USART0, "\r%S", (wchar_t*)uEstadoBotones_MSG);
				for (int i=0; i<CANTIDAD_ENTRADAS; i++){
					((uEstadoEntradas & (SET<<i))!=0) ? (index+=sprintf(chBufferTx_USART0+index, "1")) : (index+=sprintf(chBufferTx_USART0+index, "0"));
				}
				index += sprintf(chBufferTx_USART0+index, "%S", (wchar_t*)uFlagBotonesPresionados_MSG);
				for (int i=0; i<CANTIDAD_ENTRADAS; i++){
					((uFlagEntradasActivadas & (SET<<i))!=0) ? (index+=sprintf(chBufferTx_USART0+index, "1")) : (index+=sprintf(chBufferTx_USART0+index, "0"));
				}
				index += sprintf(chBufferTx_USART0+index, "%S", (wchar_t*)uFlagBotonesLiberados_MSG);
				for (int i=0; i<CANTIDAD_ENTRADAS; i++){
					((uFlagEntradasLiberadas & (SET<<i))!=0) ? (index+=sprintf(chBufferTx_USART0+index, "1")) : (index+=sprintf(chBufferTx_USART0+index, "0"));
				}

				Transmitir_Buffer_USART0(chBufferTx_USART0, USART0_BUFFER_ZISE);
			}

			uchEstadoLeerEntradas = BARRER_ENTRADAS;
			break;


		default:
			uchEstadoLeerEntradas = BARRER_ENTRADAS;
			break;
	}
}

/************************************************************************
* Funcion de inicio de la maquina de estados de lectura de teclado
************************************************************************/
void Inicio_Entradas (void){
	uEstadoEntradasInstantaneas = 0;
	uEstadoEntradas = 0;
	uFlagEntradasActivadas = 0;
	uFlagEntradasLiberadas = 0;
	uTiempoIntegracionEntradas = 0;

	chPrintDiscretas = NO;

	for(int i=0; i<CANTIDAD_ENTRADAS; i++)
		uValorIntegracionEntradas[i] = 0;

	uchEstadoLeerEntradas = BARRER_ENTRADAS;
}
