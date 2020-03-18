/** \file	teclado.c
 *  \brief	Contiene la maquina de estados correspondientes a la lectura de la botonera \n
 */

#include "discretas.h"
#include "macros.h"
#include "inicioUc.h"
#include "USART0.h"
#include "alarmas.h"

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
char					chPrintEntradas;								/**< \brief variable que indica la impresion del estado de las entradas por puerto serie */
char					chPrintSalidas;									/**< \brief variable que indica la impresion del estado de las salidas por puerto serie */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern char						chBufferTx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de transmisión USART del uc */
extern volatile unsigned long	ulContTiempoSilenciarAlarma;			/* alarmas.c, contador para posponer el sonido de la alarma por un determinado tiempo */
extern unsigned int				uTiempoSilenciarAlarma;					/* alarmas.c, tiempo en minutos por el cual se pospone el sonnido de la alarma */

/*****************************************************************************
* Mensajes para debug en memoria de programa
*****************************************************************************/
const char MSG_Estado_Entradas[]			PROGMEM = "Estado Entradas: \0";
const char uFlagBotonesPresionados_MSG[]	PROGMEM = ";  Activadas: \0";
const char uFlagBotonesLiberados_MSG[]		PROGMEM = ";  Liberadas: \0";
const char MSG_Estado_Salidas[]				PROGMEM = "Estado Salidas: \0";

/************************************************************************
* Máquina de estados de lectura de la botonera
************************************************************************/
void Leer_Entradas(void) {
	switch (uchEstadoLeerEntradas) {

		case BARRER_ENTRADAS:
			uEstadoEntradasInstantaneas = 0;

				CLEAR_BIT(TECLADO_PORT, PIN_TECLADO_COMUN);
				((TECLADO_PIN & (SET<<PIN_TECLADO_1)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_1)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_1));
				((TECLADO_PIN & (SET<<PIN_TECLADO_2)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_2)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_2));
				((TECLADO_PIN & (SET<<PIN_TECLADO_3)) == 0) ? (SET_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_3)) : (CLEAR_BIT(uEstadoEntradasInstantaneas, BOTON_TECLADO_3));

				if (uTimeoutEntradas == 0) {			/* si se vence el tiempo de lectura limpio los flag */
					uFlagEntradasActivadas = 0;
					uFlagEntradasLiberadas = 0;
				}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_1;
			break;


		case ENTRADA_TECLADO_1:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_1)) != 0){

				// boton presionado! - hacer aca -
				ulContTiempoSilenciarAlarma = (unsigned long)uTiempoSilenciarAlarma * TIMER0_A_MINUTOS;

//				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_1);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_1)) != 0){

				// boton soltado! - hacer aca -

//				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_1);
			}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_2;
			break;


		case ENTRADA_TECLADO_2:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_2)) != 0){

				// boton presionado! - hacer aca -

//				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_2);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_2)) != 0){

				// boton soltado! - hacer aca -

//				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_2);
			}

			uchEstadoLeerEntradas = ENTRADA_TECLADO_3;
			break;


		case ENTRADA_TECLADO_3:
			if ((uFlagEntradasActivadas & (SET<<BOTON_TECLADO_3)) != 0){

				// boton presionado! - hacer aca -

//				CLEAR_BIT(uFlagEntradasActivadas, BOTON_TECLADO_3);
			}

			if ((uFlagEntradasLiberadas & (SET<<BOTON_TECLADO_3)) != 0){

				// boton soltado! - hacer aca -

//				CLEAR_BIT(uFlagEntradasLiberadas, BOTON_TECLADO_3);
			}

			uchEstadoLeerEntradas = IMPRIMIR_DISCRETAS;
			break;


		case IMPRIMIR_DISCRETAS:
			if (chPrintEntradas == SI){
				int index = sprintf(chBufferTx_USART0, "\r%S", (wchar_t*)MSG_Estado_Entradas);
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
			if (chPrintSalidas == SI){
				int index = sprintf(chBufferTx_USART0, "\r%S", (wchar_t*)MSG_Estado_Salidas);
				(SALIDA_DIG_PORT & (SET<<SALIDA_DIG_1)) ? (index+=sprintf(chBufferTx_USART0+index, "1")) : (index+=sprintf(chBufferTx_USART0+index, "0"));
				(SALIDA_DIG_PORT & (SET<<SALIDA_DIG_2)) ? (index+=sprintf(chBufferTx_USART0+index, "1")) : (index+=sprintf(chBufferTx_USART0+index, "0"));

				Transmitir_Buffer_USART0(chBufferTx_USART0, USART0_BUFFER_ZISE);
			}

			uchEstadoLeerEntradas = BARRER_ENTRADAS;
			break;


		default:
			Inicio_Entradas();
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

	chPrintEntradas = NO;
	chPrintSalidas = NO;

	for(int i=0; i<CANTIDAD_ENTRADAS; i++)
		uValorIntegracionEntradas[i] = 0;

	uchEstadoLeerEntradas = BARRER_ENTRADAS;
}
