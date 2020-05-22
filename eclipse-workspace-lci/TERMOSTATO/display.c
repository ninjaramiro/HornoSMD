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
#include "control.h"

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

volatile unsigned int Disp_index_fil;
volatile unsigned int Disp_index_col;
volatile unsigned int uiContador_Blinky;


/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoDisplay;					/**< \brief variable de la maquina de estados del display */
volatile unsigned int	uContDisplay;						/**< \brief contador para la medicion de tiempos en el manejo del display */
char 					chBuffer[DISPLAY_BUFER_SIZE];		/**< \brief buffer de escritura del display */

int mode_aux=MODE_AUX_AUTO;
unsigned int t_seg=0;
/*****************************************************************************
* Variables externas*/

extern float flTempMedida;
extern int mode;
extern unsigned char uchEstadoControl;
extern int boton_up;
extern int boton_down;

extern float flTempDeseada;

/*****************************************************************************
* Constantes
*****************************************************************************/
const char Texto_Vacio[]			PROGMEM = "                \0";

/*****************************************************************************
* Maquina de estados
*****************************************************************************/

int indexDisplay;

void Actualizar_Display (void)
{
	switch(mode)
	{
		case MODE_AUTO:
			Fase_Temp_Display();
		break;

		case MODE_MENU:
			Menu_Ppal_Display();
		break;

		case MODE_MANUAL:
			Manual_Display();
		break;

		case MODE_MANUAL_CONTROL:
			Manual_Control_Display();
		break;

		default:
			Standby_Display();
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
		uContDisplay = TIEMPO_DISPLAY;

		Disp_index_fil=0;
		Disp_index_col=0;

		mode=MODE_STANDBY;
}

void Standby_Display(void)
{
	indexDisplay = sprintf(chBuffer, "Laboratorio LCI ");
	LCDWriteStringXY(0,0,chBuffer);
	indexDisplay = sprintf(chBuffer, "   Horno SMD    ");
	LCDWriteStringXY(0,1,chBuffer);
}

void Menu_Ppal_Display(void)
{
					 //0123456789012345 Hay que tener en cuenta que se empieza a partir del segundo espacio
	sprintf(chBuffer, "Automatico     ");
	LCDWriteStringXY(1,0,chBuffer);
	sprintf(chBuffer, "Manual         ");
	LCDWriteStringXY(1,1,chBuffer);

	if(boton_up==1)
	{
		sprintf(chBuffer, "*");
		LCDWriteStringXY(0,0,chBuffer);
		sprintf(chBuffer, " ");
		LCDWriteStringXY(0,1,chBuffer);
		boton_up=0;
		mode_aux=MODE_AUX_AUTO;
	}
	if(boton_down==1){
		sprintf(chBuffer, "*");
		LCDWriteStringXY(0,1,chBuffer);
		sprintf(chBuffer, " ");
		LCDWriteStringXY(0,0,chBuffer);
		boton_down=0;
		mode_aux=MODE_AUX_MANUAL;
	}
}

void Fase_Temp_Display(void)
{
	int i=0;
	if(uContDisplay==0)
	{

		// Primera linea:
		// 0123456789012345
		// Fase: PreHeat
		// Fase: Heat
		// Fase: Cooling
		// Fase: -UL-
		// Fase: RampToSoak
		// Fase: Reflow

		i = sprintf(chBuffer, "Fase: ");

		if (uchEstadoControl == ESTADO_RAMP_TO_SOAK)
		{
			sprintf(chBuffer+i, "RampToSoak");

		}else if (uchEstadoControl == ESTADO_PREHEAT)
		{
			sprintf(chBuffer+i, "PreHeat   ");

		}else if (uchEstadoControl == ESTADO_RAMP_TO_PEAK)
		{
			sprintf(chBuffer+i, "RampToPeak");

		}else if (uchEstadoControl == ESTADO_REFLOW_UP || uchEstadoControl == ESTADO_REFLOW_DOWN)
		{
			sprintf(chBuffer+i, "Reflow    ");

		}else if (uchEstadoControl == ESTADO_COOLING)
		{
			sprintf(chBuffer+i, "Cooling   ");
		}

		if (flTempMedida < 0.0)
		{
			sprintf(chBuffer+i, "-UL-      "); // Under limit
		}else if (flTempMedida > TEMP_MAX)
		{
			sprintf(chBuffer+i, "-OL-      "); // Over limit
		}

		LCDWriteStringXY(0,0,chBuffer);


		// Segunda linea:
		// 0123456789012345
		// Temp: 70.8°C
		// Temp: 150.6°C
		// Temp: -UL-

		i = sprintf(chBuffer, "Temp: ");
		if (flTempMedida < 0.0){
			sprintf(chBuffer+i, "-UL-      "); // Under limit
		}
		else if (flTempMedida < 10.0){
			dtostrf(flTempMedida, 4, 2, chBuffer+i);
			sprintf(chBuffer+i+4, "*C    ");
		}
		else if (flTempMedida < 300.0){
			dtostrf(flTempMedida, 5, 1, chBuffer+i);
			sprintf(chBuffer+i+5, "*C   "); // " °C" no se muestra bien en el display.
										  // Tambien probe con sprintf(chBuffer, "%cC", (char)248); y no funciono
		}
		else {
			sprintf(chBuffer+i, "-OL-      "); // Over limit
		}

		LCDWriteStringXY(0,1,chBuffer);

	}
}

void Manual_Display(void)
{
	int i=0; // variable auxiliar
	if(uContDisplay==0)
	{
		// Primera linea:
						// 0123456789012345
		sprintf(chBuffer, "Seleccionar Temp");
		LCDWriteStringXY(0,0,chBuffer);


		// Segunda linea:
		// 0123456789012345
		// Temp: 70.8°C
		// Temp: 150.6°C
		// Temp: -UL-

		i = sprintf(chBuffer, "Temp: ");
		if (flTempDeseada < 0.0){
			sprintf(chBuffer+i, "-UL-      "); // Under limit
		}
		else if (flTempDeseada < 10.0){
			dtostrf(flTempDeseada, 4, 2, chBuffer+i);
			sprintf(chBuffer+i+4, "*C    ");
		}
		else if (flTempDeseada < 300.0){
			dtostrf(flTempDeseada, 5, 1, chBuffer+i);
			sprintf(chBuffer+i+5, "*C   "); // " °C" no se muestra bien en el display.
										  // Tambien probe con sprintf(chBuffer, "%cC", (char)248); y no funciono
		}
		else {
			sprintf(chBuffer+i, "-OL-      "); // Over limit
		}

		LCDWriteStringXY(0,1,chBuffer);

	}
}


void Manual_Control_Display(void)
{
	int i=0; // variable auxiliar
	if(uContDisplay==0)
	{
		// Primera linea:
						// 0123456789012345
		sprintf(chBuffer, "t [seg]:         ");
		LCDWriteStringXY(0,0,chBuffer);
		dtostrf(t_seg, 7, 0, chBuffer);
		LCDWriteStringXY(9,0,chBuffer);
		t_seg++;

		// Segunda linea:
		// 0123456789012345
		// Temp: 70.8°C
		// Temp: 150.6°C
		// Temp: -UL-

		i = sprintf(chBuffer, "Temp: ");
		if (flTempMedida < 0.0){
			sprintf(chBuffer+i, "-UL-      "); // Under limit
		}
		else if (flTempMedida < 10.0){
			dtostrf(flTempMedida, 4, 2, chBuffer+i);
			sprintf(chBuffer+i+4, "*C    ");
		}
		else if (flTempMedida < 300.0){
			dtostrf(flTempMedida, 5, 1, chBuffer+i);
			sprintf(chBuffer+i+5, "*C   "); // " °C" no se muestra bien en el display.
										  // Tambien probe con sprintf(chBuffer, "%cC", (char)248); y no funciono
		}
		else {
			sprintf(chBuffer+i, "-OL-      "); // Over limit
		}

		LCDWriteStringXY(0,1,chBuffer);
		uContDisplay = TIEMPO_DISPLAY;
	}
}
