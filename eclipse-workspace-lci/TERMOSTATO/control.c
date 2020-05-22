/*
 * control.c
 *
 *  Created on: 7 may. 2020
 *      Author: matu
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

#include "control.h"
#include "botones.h"
#include "inicioUc.h"
#include "macros.h"



/*****************************************************************************
* variables
*****************************************************************************/
unsigned char uchEstadoControl;
extern float flTempMedida;
volatile unsigned int	uContControlAuto;
int mode=MODE_STANDBY;
int uTiempoPreheat=0; // Contador para el tiempo de preheat.
unsigned int temp_deseada=0;
float flTempDeseada=70;

extern int boton_up;
extern int boton_down;
extern int boton_exit;
extern int mode_aux;
//****************************************************************************

void control_temperatura(void)
{
	switch(uchEstadoControl)
	{
		case ESTADO_STANDBY:
			mode=MODE_STANDBY;
			uchEstadoControl=ESTADO_STANDBY;
		break;

		case ESTADO_MENU:
			mode=MODE_MENU;
			uchEstadoControl=ESTADO_MENU;
		break;

		case ESTADO_AUTO:
			mode_aux=MODE_AUX_WAIT;
			// Incio de la curva de temperatura por defecto
			uContControlAuto=TIEMPO_RAMP_TO_SOAK; 	// Es en realidad la pendiente
			uchEstadoControl=ESTADO_RAMP_TO_SOAK;
			temp_deseada=flTempMedida+2; 			// Temperatura ambiente+2
			mode=MODE_AUTO;
		break;

		case ESTADO_RAMP_TO_SOAK:
			// Es una rampa que sube desde la temperatura ambiental
			// hasta 150°C (temperatura preheat) con una pendiente
			// de 1°C/seg a 3°C/seg.

			if (uContControlAuto == 0) // Entra cada 1 segundo a este if
			{
				// SUBIR LA TEMPERATURA UN GRADO (+1°C)
				if (flTempMedida <= temp_deseada)//TEMP_PREHEAT)
				{
					// Activo el rele para activar el horno y asi subir la temperatura
					SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);

					// Despues habra que ver como hacer para que cumpla con la pendiente.
					// Asi como esta implementado, cada 1 segundo entra en este if y
					// comprueba que la temperatura sea menor a la de preheat. Si es
					// asi deja prendido el rele
				}else{
					CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}
				temp_deseada=temp_deseada+2;
				flTempMedida++; // Esto es para probar sin sensor
				uContControlAuto=TIEMPO_RAMP_TO_SOAK;
			}

			if (flTempMedida >= TEMP_PREHEAT) 	// Temperatura maxima de la fase,
												//por lo que paso a la fase siguiente
			{
				uContControlAuto=TIEMPO_PREHEAT_DELAY;
				uchEstadoControl=ESTADO_PREHEAT;
				temp_deseada=TEMP_PREHEAT+2;
			}

		break;

		case ESTADO_PREHEAT:
			// La temperatura se mantiene constante en 150°C +/- 20°C
			// durante 60 a 120 segundos.

			if (uContControlAuto == 0) // Entro cada 1 segundo
			{
				if (flTempMedida <= TEMP_PREHEAT)
				{
					SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}else{
					CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}


				uTiempoPreheat++; 	// Aumento un contador cada 1 segundo, porque esta fase se
									// mantiene con temperatura constante durante cierto tiempo
				uContControlAuto=TIEMPO_PREHEAT_DELAY;
			}

			if (uTiempoPreheat > TIEMPO_PREHEAT)
			{
				uContControlAuto=TIEMPO_RAMP_TO_PEAK; //
				uchEstadoControl=ESTADO_RAMP_TO_PEAK;
			}

		break;

		case ESTADO_RAMP_TO_PEAK:
			// Sube desde preheat hasta 217°C con una pendiente de
			// 1 a 3°C/seg.

			if (uContControlAuto == 0)
			{
				// SUBIR LA TEMPERATURA UN GRADO (+1°C)
				if (flTempMedida <= temp_deseada)
				{
					SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}else{
					CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}
				temp_deseada=temp_deseada+2;

				flTempMedida++;
				uContControlAuto=TIEMPO_RAMP_TO_SOAK;
			}

			if (flTempMedida >= TEMP_RAMP_TO_PEAK_MAX)
			{
				uContControlAuto=TIEMPO_REFLOW_UP;
				uchEstadoControl=ESTADO_REFLOW_UP;
			}
		break;

		case ESTADO_REFLOW_UP:
			if (uContControlAuto == 0)
			{
				// SUBIR LA TEMPERATURA UN GRADO (+1°C)
				if (flTempMedida <= temp_deseada)
				{
					SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}else{
					CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}
				temp_deseada=temp_deseada+2;

				flTempMedida++;
				uContControlAuto=TIEMPO_REFLOW_UP;
			}

			if (flTempMedida >= TEMP_REFLOW_MAX)
			{
				uContControlAuto=TIEMPO_REFLOW_DOWN;
				uchEstadoControl=ESTADO_REFLOW_DOWN;
			}
		break;

		case ESTADO_REFLOW_DOWN:
			if (uContControlAuto == 0)
			{
				// BAJAR LA TEMPERATURA DOS GRADOS (-2°C)
				if (flTempMedida <= temp_deseada)
				{
					SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}else{
					CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}
				temp_deseada=temp_deseada-3;


				flTempMedida--;
				uContControlAuto=TIEMPO_REFLOW_DOWN;
			}

			if (flTempMedida <= TEMP_RAMP_TO_PEAK_MAX)
			{
				uContControlAuto=TIEMPO_COOLING;
				uchEstadoControl=ESTADO_COOLING;
			}

		break;

		case ESTADO_COOLING:
			if (uContControlAuto == 0)
			{
				// BAJAR LA TEMPERATURA DOS GRADO (-2°C)
				if (flTempMedida <= temp_deseada)
				{
					SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}else{
					CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}
				temp_deseada=temp_deseada-3;

				flTempMedida--;
				uContControlAuto=TIEMPO_COOLING;
			}

			if (flTempMedida <= TEMP_COOLING_MIN)
			{
				uchEstadoControl=ESTADO_STANDBY;
				mode=MODE_STANDBY;
			}
		break;

		case ESTADO_MANUAL:
			mode_aux=MODE_AUX_WAIT;
			mode=MODE_MANUAL;
			if(boton_up==1)
			{
				flTempDeseada++;
				boton_up=0;
			}
			if(boton_down==1)
			{
				flTempDeseada--;
				boton_down=0;
			}

		break;

		case ESTADO_MANUAL_CONTROL:
			mode=MODE_MANUAL_CONTROL;
			if (uContControlAuto == 0) // Entro cada 1 segundo
			{
				if (flTempMedida <= flTempDeseada)
				{
					flTempMedida++;
					SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}else{
					flTempMedida--;
					CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				}

				uContControlAuto=TIEMPO_PREGUNTA_MANUAL;
			}

		break;


		default:

		break;
	}
}

void control_init(void)
{
	uchEstadoControl=ESTADO_STANDBY;
}
