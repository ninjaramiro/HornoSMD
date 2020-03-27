/** \file	display.c
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente a la actualizacion del display.
 *
 */

#include "macros.h"
#include "lcd.h"
#include "display.h"
#include "inicioUc.h"
//#include "medicion_ADC.h"
//#include "alarmas.h"
//#include "USART0.h"
//#include "discretas.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>


#ifdef LCD_TYPE_162
#define DISPLAY_SIZE	16
#endif
#ifdef LCD_TYPE_164
#define DISPLAY_SIZE	16
#endif
#ifdef LCD_TYPE_202
#define DISPLAY_SIZE	20
#endif
#ifdef LCD_TYPE_204
#define DISPLAY_SIZE	20
#endif

#define DISPLAY_BUFER_SIZE DISPLAY_SIZE+5



/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoDisplay;					/**< \brief variable de la maquina de estados del display */
volatile unsigned int	uContDisplay;						/**< \brief contador para la medicion de tiempos en el manejo del display */
char 					chBuffer[DISPLAY_BUFER_SIZE];		/**< \brief buffer de escritura del display */
volatile unsigned int	uContDisplayOFF;					/**< \brief contador para hacer parpadear el display */
volatile unsigned int	uContDisplayON;						/**< \brief contador para hacer parpadear el display */
volatile char			chDisplayInstalado;					/**< \brief indica di hay n display instalado, se carga cuando inicia el equipo */

/*****************************************************************************
* Variables externas*/


extern volatile uint16_t result; //Para el resultado del ADC



//extern volatile float				flCond1valoruS;				/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
//extern volatile float		 		flValorADCcond1;			/* medicion_ADC.c, valor del ADC en cuentas para los rangos segun correccion de linealidad */
//extern volatile float				flCond2valoruS;				/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
//extern volatile float		 		flValorADCcond2;			/* medicion_ADC.c, valor del ADC en cuentas para los rangos segun correccion de linealidad */
//extern unsigned int					uRangoCond1;				/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
//extern unsigned int					uRangoCond2;				/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
//extern int							OnOffAlarmas;				/* alarmas.c, cada bit corresponde al estado de activacion de una alarma */
//extern int							cond1MaxUs;
//extern int							cond1MinUs;
//extern int							cond2MaxUs;
//extern int							cond2MinUs;
//
//extern char							chNumeroSerie[];			/**< \brief numero de serie del equipo */
//
//extern const char 					Software_Version[];
//extern const char 					MSG_Serial_Number[];
//extern const char 					MSG_Software[];
//extern const char 					MSG_Llamas[];
//
//extern char							chBufferTx_USART1[];		/* USART1.c, buffer de transmisión USART del uc */
//extern unsigned char				uchEstadoBufferTx_USART1;	/* USART1.c, variable de estado del buffer de transmisión del USART, lleno o vacio */
//
//extern volatile unsigned int		uEstadoEntradas;			/* entradas.c, estado de cada entrada luego de la integración */

/*****************************************************************************
* Constantes
*****************************************************************************/
const char Texto_Vacio[]			PROGMEM = "                \0";

/*****************************************************************************
* Maquina de estados
*****************************************************************************/

int indexDisplay;

void Actualizar_Display (void) {
	uchEstadoDisplay = DISPLAY_ACTUALIZAR_L1;

	switch (uchEstadoDisplay) {

		case DISPLAY_STANDBY:
			if (uContDisplay == 0){
				uchEstadoDisplay = DISPLAY_ACTUALIZAR_L1;
				uContDisplay = PERIODO_ACTUALIZACION_DISPLAY;
			}
			break;


		case DISPLAY_ACTUALIZAR_L1:


			 indexDisplay = sprintf(chBuffer, "Hola");
//			 dtostrf(ADC1, 6, 2, chBuffer+indexDisplay);
//			 indexDisplay += 6;


			 // indexDisplay += sprintf(chBuffer+indexDisplay, "%%0S/cm");  // Para agregarle la unidad
			LCDWriteStringXY(0,0,chBuffer);

			uchEstadoDisplay = DISPLAY_ACTUALIZAR_L2;
			break;


		case DISPLAY_ACTUALIZAR_L2:

//			 indexDisplay = sprintf(chBuffer, "Chau");
//			 dtostrf(ADC1, 6, 2, chBuffer+indexDisplay);
//			 indexDisplay += 6;


			 // indexDisplay += sprintf(chBuffer+indexDisplay, "%%0S/cm");  // Para agregarle la unidad
			//LCDWriteStringXY(1,0,chBuffer);

			uchEstadoDisplay = DISPLAY_STANDBY;
			break;


		default:
			Inicio_Display();
			break;
	}
}


/*****************************************************************************
* Funcion de inicio
*****************************************************************************/
void Inicio_Display (void) {

		LCDInit(LS_NONE);
		sprintf(chBuffer, "%S", (wchar_t*)Texto_Vacio);
		LCDWriteStringXY(0,0,chBuffer);
		LCDWriteStringXY(0,1,chBuffer);
		SET_BIT(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT);
		uContDisplayON = TIEMPO_DISPLAY_ON;
		uContDisplayOFF = 0;

	uchEstadoDisplay = DISPLAY_STANDBY;  //Display inicializado y listo para usar
}


/*****************************************************************************
* Pantalla presentacion
*****************************************************************************/
//void Pantalla_Presentacion_Display (void) {
//	if (chDisplayInstalado == SI){
//		Transmitir_Buffer_USART0_P (PSTR("\n\r\0"));
//
//		sprintf(chBuffer, "%S", (wchar_t*)MSG_Llamas);
//		LCDWriteStringXY(0,0,chBuffer);
//
//		Transmitir_Buffer_USART0_P (PSTR("\n\r\0"));
//		Transmitir_Buffer_USART0 (chBuffer, DISPLAY_BUFER_SIZE);
//
//		sprintf(chBuffer, "%S%s %S%S", (wchar_t*)MSG_Serial_Number, chNumeroSerie, (wchar_t*)MSG_Software, (wchar_t*)Software_Version);
//		LCDWriteStringXY(0,1,chBuffer);
//
//		Transmitir_Buffer_USART0_P (PSTR("\n\r\0"));
//		Transmitir_Buffer_USART0 (chBuffer, DISPLAY_BUFER_SIZE);
//
//		uContDisplay = TIEMPO_PRESENTACION;
//		while(uContDisplay != 0);
//	}
//}
