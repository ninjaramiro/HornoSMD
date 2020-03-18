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
#include "medicion_ADC.h"
#include "alarmas.h"

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

/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoDisplay;					/**< \brief variable de la maquina de estados del display */
volatile unsigned int	uContDisplay;						/**< \brief contador para la medicion de tiempos en el manejo del display */
int 					indexDisplay;						/**< \brief index para la escritura del display */
char 					chBuffer[DISPLAY_SIZE+1];			/**< \brief buffer de escritura del display */
float 					flValorDisplay;						/**< \brief valor a mostrar en el display */
volatile unsigned int	uContDisplayOFF;					/**< \brief contador para hacer parpadear el display */
volatile unsigned int	uContDisplayON;						/**< \brief contador para hacer parpadear el display */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern float						flCond1valoruS;			/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern unsigned int 				uValorADCcond1;			/* medicion_ADC.c, valor del ADC en cuentas para los rangos segun correccion de linealidad */
extern float						flCond2valoruS;			/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern unsigned int 				uValorADCcond2;			/* medicion_ADC.c, valor del ADC en cuentas para los rangos segun correccion de linealidad */
extern int							OnOffAlarmas;			/* alarmas.c, cada bit corresponde al estado de activacion de una alarma */

/*****************************************************************************
* Constantes
*****************************************************************************/
const char Texto_Llamas[]			PROGMEM = "F.J. LLAMAS S.A.\0";					/**< \brief nombre de la empresa */
const char Texto_Cargando[]			PROGMEM = "Cargando\0";							/**< \brief leyenda de inicio */
const char Texto_Punto[]			PROGMEM = ".\0";								/**< \brief punto */
const char Texto_Vacio[]			PROGMEM = "                \0";

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Actualizar_Display (void) {
	switch (uchEstadoDisplay) {

		case DISPLAY_STANDBY:
			if (uContDisplay == 0){
				uchEstadoDisplay = DISPLAY_ACTUALIZAR_L1;
			}
			break;


		case DISPLAY_ACTUALIZAR_L1:
			// Actualización display
			if (((OnOffAlarmas&(SET<<ALARMA_1)) != 0) && (uContDisplayOFF != 0)){
				sprintf(chBuffer, "%S", (wchar_t*)Texto_Vacio);
			}
			else {
				if ((flCond1valoruS > MAX_COND1_US) || (uValorADCcond1 >1020)){
					indexDisplay = sprintf(chBuffer, "CH1> ");
					flValorDisplay = MAX_COND1_US;
				}
				else if (flCond1valoruS < MIN_COND1_US){
					indexDisplay = sprintf(chBuffer, "CH1= ");	// pongo = porque el minimo del canal 1 es cero!
					flValorDisplay = MIN_COND1_US;
				}
				else{
					indexDisplay = sprintf(chBuffer, "CH1= ");
					flValorDisplay = flCond1valoruS;
				}
				dtostrf(flValorDisplay, 6, 2, chBuffer+indexDisplay);
				indexDisplay += 6;
				indexDisplay += sprintf(chBuffer+indexDisplay, "%%0%%1/cm");
			}

			LCDWriteStringXY(0,0,chBuffer);

			uchEstadoDisplay = DISPLAY_ACTUALIZAR_L2;
			break;


		case DISPLAY_ACTUALIZAR_L2:
			// Actualización display
			if (((OnOffAlarmas&(SET<<ALARMA_2)) != 0) && (uContDisplayOFF != 0)){
				sprintf(chBuffer, "%S", (wchar_t*)Texto_Vacio);
			}
			else {
				if ((flCond2valoruS > MAX_COND2_US) || (uValorADCcond2 >1020)){
					indexDisplay = sprintf(chBuffer, "CH2> ");
					flValorDisplay = MAX_COND2_US;
				}
				else if (flCond2valoruS < MIN_COND2_US){
					indexDisplay = sprintf(chBuffer, "CH2< ");
					flValorDisplay = MIN_COND2_US;
				}
				else{
					indexDisplay = sprintf(chBuffer, "CH2= ");
					flValorDisplay = flCond2valoruS;
				}
				dtostrf(flValorDisplay, 6, 1, chBuffer+indexDisplay);
				indexDisplay += 6;
				indexDisplay += sprintf(chBuffer+indexDisplay, "%%0%%1/cm");
			}

			LCDWriteStringXY(0,1,chBuffer);

			uContDisplay = PERIODO_ACTUALIZACION_DISPLAY;

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
	LCDInit(LS_NONE);								// OJO HAY QUE HACER LA DETECCION DEL LCD ANTES DE INICIAR

	sprintf(chBuffer, "%S", (wchar_t*)Texto_Vacio);
	LCDWriteStringXY(0,0,chBuffer);
	LCDWriteStringXY(0,1,chBuffer);
	SET_BIT(LCD_BACKLIGHT_PORT, LCD_BACKLIGHT);

	uContDisplayON = TIEMPO_DISPLAY_ON;
	uContDisplayOFF = 0;

	uchEstadoDisplay = DISPLAY_STANDBY;
}


/*****************************************************************************
* Pantalla presentacion
*****************************************************************************/
void Pantalla_Presentacion_Display (void) {

	sprintf(chBuffer, "%S", (wchar_t*)Texto_Llamas);
	LCDWriteStringXY(0,0,chBuffer);

	uContDisplay = TIEMPO_PUNTOS_PRESENTACION;
	while(uContDisplay != 0);

	indexDisplay = sprintf(chBuffer, "%S", (wchar_t*)Texto_Cargando);
	LCDWriteStringXY(0,1,chBuffer);

	uContDisplay = TIEMPO_PUNTOS_PRESENTACION;
	while(uContDisplay != 0);

	sprintf(chBuffer, "%S", (wchar_t*)Texto_Punto);
	for (int i=0; i<DISPLAY_SIZE-indexDisplay; i++){
		LCDWriteStringXY(indexDisplay+i,1,chBuffer);

		uContDisplay = TIEMPO_PUNTOS_PRESENTACION;
		while(uContDisplay != 0);
	}
}
