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
#include "botones.h"

//#include "medicion_ADC.h"
//#include "alarmas.h"
//#include "USART0.h"
//#include "discretas.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>


#define DISPLAY_SIZE	16  // Longitud de cada línea del display

#define DISPLAY_BUFFER_SIZE DISPLAY_SIZE+5

char 	chLCDbuffer[DISPLAY_BUFFER_SIZE];		/**< \brief buffer de escritura del display */

/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoDisplay;					/**< \brief variable de la maquina de estados del display */
volatile unsigned int	uContDisplay;						/**< \brief contador para la medicion de tiempos en el manejo del display */
char 					chBuffer[DISPLAY_BUFFER_SIZE];		/**< \brief buffer de escritura del display */
volatile unsigned int	uContDisplayOFF;					/**< \brief contador para hacer parpadear el display */
volatile unsigned int	uContDisplayON;						/**< \brief contador para hacer parpadear el display */
volatile char			chDisplayInstalado;					/**< \brief indica di hay n display instalado, se carga cuando inicia el equipo */

/*****************************************************************************
* Variables externas*/


extern volatile uint16_t u16_intervalo_s; // Variable que define el intervalo de conteo.
extern volatile uint16_t u16_intervalo; // Variable que define el intervalo de conteo.
extern volatile uint16_t u16_tiempo_s; // Variable que define el tiempo transcurrido, cuenta regresiva
extern volatile uint16_t u16_pulsos_en_intervalo; // Acumulacion de pulsos detectados
extern volatile uint16_t u16_CPM; // Variable donde se almacena el resultado de la medicion
extern volatile uint16_t u16_CPM_temp; // Variable donde se almacena el resultado de la medicion
extern volatile uint8_t  u8_unidad;  // Unidad usada para definir el intervalo
extern volatile uint8_t flag_stop;


extern struct_botones botones;


/*****************************************************************************
* Constantes
*****************************************************************************/
const char Texto_Inicio_L1[] 			PROGMEM = "Ramiro Alonso   \0";
const char Texto_Inicio_L2[] 			PROGMEM = "Contador Geiger \0";
const char Texto_Vacio[]				PROGMEM = "                \0";

/*****************************************************************************
* Maquina de estados
*****************************************************************************/

int indexDisplay;

void Actualizar_Display (void) {

	switch (uchEstadoDisplay) {

		case DISPLAY_STANDBY:
			if (uContDisplay == 0){
				uchEstadoDisplay = DISPLAY_PRESENTACION;
				uContDisplay = TIEMPO_PRESENTACION;
			}
			break;


		case DISPLAY_PRESENTACION:
			Pantalla_Presentacion_Display();

			if (uContDisplay == 0){
				uchEstadoDisplay = DISPLAY_MEDICION_INTERVALO;
				LCDClear();
			}


			break;


		case DISPLAY_MEDICION_INTERVALO:

			sprintf(chLCDbuffer, "Int: %u",u16_intervalo);
			LCDWriteStringXY(0,0,chLCDbuffer);

			sprintf(chLCDbuffer, "%%");
			LCDWriteStringXY(15,0,chLCDbuffer);


			if (u8_unidad == SEGUNDOS){
				sprintf(chLCDbuffer, "s");
			}

			if (u8_unidad == MINUTOS){
				sprintf(chLCDbuffer, "m");
			}

			if (u8_unidad == HORAS){
				sprintf(chLCDbuffer, "h");
			}

			if (u16_intervalo < 10){
				LCDWriteStringXY(6,0,chLCDbuffer);
			}
			else if (u16_intervalo < 100){
				LCDWriteStringXY(7,0,chLCDbuffer);
			}
			else {
				LCDWriteStringXY(8,0,chLCDbuffer);
			}

			float flPorcentaje;
			flPorcentaje = 100-((float)100/u16_intervalo_s)*((float)u16_tiempo_s);

//			flPorcentaje = ((float)u16_tiempo_s);
			char valor[10];
			dtostrf(flPorcentaje, 2,0, valor);
			LCDWriteStringXY(12,0,valor);


//			sprintf(chLCDbuffer,"Bot:  %c  %c  %d ", botones.estado+48, 	botones.boton_nro+48, botones.timeout_debouncer);
//			LCDWriteStringXY(0,1,chLCDbuffer);


			sprintf(chLCDbuffer, "CPM: %u", u16_CPM);
			LCDWriteStringXY(0,1,chLCDbuffer);

			// Detengo el conteo si tengo el flag de stop
			if (flag_stop == 1){
				sprintf(chLCDbuffer, " STOP");
			}

			else{
				if (u16_pulsos_en_intervalo > 9999){
					sprintf(chLCDbuffer, "xxxxp");
				}
				else if (u16_pulsos_en_intervalo > 999){
					sprintf(chLCDbuffer, "%up", u16_pulsos_en_intervalo);
				}
				else if (u16_pulsos_en_intervalo > 99){
					sprintf(chLCDbuffer, " %up", u16_pulsos_en_intervalo);
				}
				else if (u16_pulsos_en_intervalo > 9){
					sprintf(chLCDbuffer, "  %up", u16_pulsos_en_intervalo);
				}
				else{
					sprintf(chLCDbuffer, "   %up", u16_pulsos_en_intervalo);
				}
			}
			LCDWriteStringXY(11,1,chLCDbuffer);



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
		sprintf(chLCDbuffer, "%S", (wchar_t*)Texto_Vacio);
		LCDWriteStringXY(0,0,chLCDbuffer);
		sprintf(chLCDbuffer, "%S", (wchar_t*)Texto_Vacio);
		LCDWriteStringXY(0,1,chLCDbuffer);
		uContDisplayON = TIEMPO_DISPLAY_ON;
		uContDisplayOFF = 0;

	uchEstadoDisplay = DISPLAY_STANDBY;  //Display inicializado y listo para usar
}


/*****************************************************************************
* Pantalla presentacion
*****************************************************************************/
void Pantalla_Presentacion_Display (void) {

		sprintf(chLCDbuffer, "%S", (wchar_t*)Texto_Inicio_L1);
		LCDWriteStringXY(0,0,chLCDbuffer);


		sprintf(chLCDbuffer, "%S", (wchar_t*)Texto_Inicio_L2);
		LCDWriteStringXY(0,1,chLCDbuffer);

	//	uContDisplay = TIEMPO_PRESENTACION;
		while(uContDisplay != 0);

}
