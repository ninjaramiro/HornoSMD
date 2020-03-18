/*
 * control.c
 *
 *  Created on: Feb 16, 2018
 *      Author: poroto
 */


#include "control.h"
#include "macros.h"
#include "inicio.h"
#include "motorPP.h"
#include "USART0.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <math.h>
#include <avr/wdt.h>

/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoControl;						/**< \brief variable de la maquina de estados de control */

volatile unsigned int 	uContPeriodoPID;						/**< \brief contador del periodo de actualizacion del PID */
unsigned int			uPeriodoPID;							/**< \brief periodo de actualizacion del PID */
volatile unsigned int 	uContPeriodoVedaPID;					/**< \brief periodo de veda del PID */
unsigned int 			uPeriodoVedaPID;
volatile unsigned int 	uContPeriodoVedaCaudal;					/**< \brief periodo de veda de deteccion de variacion de caudal */
unsigned int 			uPeriodoVedaCaudal;
float					flVariacionCaudalMedido;				/**< \brief valor que ha variado el caudal */
float					flVariacionCaudalSetP;					/**< \brief porcentaje para el cual si varia el caudal corrijo fuera del PID */
float 					flEscalaVariacionCaudal;				/**< \brief factor de escala para el movimiento de la valvula con el cambio brusco de caudal */
float					flCaudalMedidoAnterior;
float					flTempMedidaAnterior;
float					flPIDerror;
float					flPIDpropTerm;
float					flPIDintegTerm;
float					flPIDderivTerm;
float					flPIDkP;
float					flPIDkI;
float					flPIDkD;
float					flPIDintMax;
float 					flCaudalEscalaKsPID;					/**< \brief valor de caudal para el cual se escala los valores de k del PID */

char 					chPrintEstado;
char 					chPrintPID;

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern float 			flCaudalMedido;							/**< \brief valor medio de caudal en l/min */
extern float			flTempMedida;
extern float 			flTempSetP;								/**< \brief temperatura seleccionada en °C */
extern int 				relativePositionBySteps;				/**< \brief posicion relativa de la valvula en cantidad de steps realizados */
extern int 				stepToRotate;							/**< \brief cantidad de pasos a rotar, + derecha, - izquierda */

extern char				chBufferTx_USART0[USART0_BUFFER_ZISE];	/**< \brief buffer de transmisión USART del uc */

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Control_Temp (void) {
	switch (uchEstadoControl) {

		case CONTROL_STANDBY:
			// detecto caudal arranque
			if (flCaudalMedido > CAUDAL_ARRANQUE){
				// inicio el PID
				flPIDintegTerm=0;
				flTempMedidaAnterior = flTempMedida;
				// inicio la deteccion de variacion brusca de caudal
				flCaudalMedidoAnterior=flCaudalMedido;
				// inicio los tiempos de veda de operacion de cada control
				uContPeriodoVedaPID=0;
				uContPeriodoVedaCaudal=uPeriodoVedaCaudal;
				// deteccion de condiciones inseguras
				if ((flTempMedida > MAX_TEMPERATURA_SEGURIDAD) || (flTempMedida < MIN_TEMPERATURA_SEGURIDAD) || (flCaudalMedido > MAX_CAUDAL_SEGURIDAD) || (flCaudalMedido < MIN_CAUDAL_SEGURIDAD)){
					//cierro el gas
					CLEAR_BIT(RELAY_GAS_PORT, RELAY_GAS_PIN);
				}
				else{
					// abro el gas
					SET_BIT(RELAY_GAS_PORT, RELAY_GAS_PIN);
					uchEstadoControl = CONTROL_PID_TEMP;
				}
			}
			else{ // poco caudal cierro el gas
				CLEAR_BIT(RELAY_GAS_PORT, RELAY_GAS_PIN);
			}

			// impresion de variables
			if (chPrintEstado == SI){
				int index = sprintf(chBufferTx_USART0, "\rCaud= ");
				dtostrf(flCaudalMedido, 4, 1, chBufferTx_USART0+index);
				index += 4;
				index += sprintf(chBufferTx_USART0+index, " l/min; SetTemp= ");
				dtostrf(flTempSetP, 4, 1, chBufferTx_USART0+index);
				index += 4;
				index += sprintf(chBufferTx_USART0+index, " C; AcTemp= ");
				dtostrf(flTempMedida, 4, 1, chBufferTx_USART0+index);
				index += 4;
				if (stepToRotate<0){
					index += sprintf(chBufferTx_USART0+index, " C; ValvPos= %03d%04d;", relativePositionBySteps, stepToRotate);
				}
				else{
					index += sprintf(chBufferTx_USART0+index, " C; ValvPos= %03d+%03d;", relativePositionBySteps, stepToRotate);
				}
				Transmitir_Buffer_USART0(chBufferTx_USART0, USART0_BUFFER_ZISE);
			}
			break;


		case CONTROL_PID_TEMP:
			// deteccion de condiciones inseguras
			if ((flTempMedida > MAX_TEMPERATURA_SEGURIDAD) || (flTempMedida < MIN_TEMPERATURA_SEGURIDAD) || (flCaudalMedido > MAX_CAUDAL_SEGURIDAD) || (flCaudalMedido < MIN_CAUDAL_SEGURIDAD)){
				//cierro el gas
				CLEAR_BIT(RELAY_GAS_PORT, RELAY_GAS_PIN);
				uchEstadoControl = CONTROL_STANDBY;
			}

			// lazo de control
			if (uContPeriodoPID == 0){
				// deteccion de variacion brusca de caudal
				flVariacionCaudalMedido = (flCaudalMedido/flCaudalMedidoAnterior)-1.0;
				if (abs(flVariacionCaudalMedido) > (flVariacionCaudalSetP/100.0)){
					// deteccion de condicion insegura
					if (abs(flVariacionCaudalMedido) > MAX_VARIACION_CAUDAL_SEGURIDAD){
						// cierro el gas
						CLEAR_BIT(RELAY_GAS_PORT, RELAY_GAS_PIN);
						// reinicio el sistema
						UC_RESET();
					}
					// accion en caso de deteccion de variacion brusca de caudal
					if ((uContPeriodoVedaCaudal==0) && (flEscalaVariacionCaudal != 0)){
						// muevo el motor pp segun variacion de caudal
						stepToRotate = relativePositionBySteps*(int)(flVariacionCaudalMedido*flEscalaVariacionCaudal);
						// reinicio el PID
						flPIDintegTerm=0;
						flTempMedidaAnterior = flTempMedida;
						// inicio el tiempos de veda de PID
						uContPeriodoVedaPID=uPeriodoVedaPID;
					}
				}
				flCaudalMedidoAnterior=flCaudalMedido;

				// control PID
				if (uContPeriodoVedaPID == 0) {
					// escalo el valor de Kp, Ki, Kd segun caudal, si corresponde
					float flEscala = 1.0;
					if(flCaudalEscalaKsPID != 0){
						flEscala=flCaudalMedido/flCaudalEscalaKsPID;
					}
					// calculo error
					flPIDerror = flTempSetP-flTempMedida;
					// calculo proporcional
					flPIDpropTerm = flPIDkP * flEscala * flPIDerror;
					// calculo integral
					flPIDintegTerm += flPIDkI * flEscala * flPIDerror * uPeriodoPID / TIMER2_A_SEGUNDOS;
					if (flPIDintegTerm > flPIDintMax){
						flPIDintegTerm = flPIDintMax;
					}
					else if (flPIDintegTerm < -flPIDintMax){
						flPIDintegTerm = -flPIDintMax;
					}
					// calculo diferencial
					flPIDderivTerm = flPIDkD * flEscala * (flTempMedidaAnterior-flTempMedida) * TIMER2_A_SEGUNDOS / uPeriodoPID;
					flTempMedidaAnterior = flTempMedida;
					// muevo el motor pp
					stepToRotate = (int)(flPIDpropTerm+flPIDintegTerm+flPIDderivTerm);
				}
				else {
					// inicio el PID
					flPIDintegTerm=0;
					flTempMedidaAnterior = flTempMedida;
				}

				// impresion de variables que observa el PID
				if (chPrintPID == SI){
					dtostrf(flCaudalMedido, 4, 1, chBufferTx_USART0);
					int index = 4;
					index += sprintf(chBufferTx_USART0+index, ";");
					dtostrf(flTempSetP, 4, 1, chBufferTx_USART0+index);
					index += 4;
					index += sprintf(chBufferTx_USART0+index, ";");
					dtostrf(flTempMedida, 4, 1, chBufferTx_USART0+index);
					index += 4;
					if (stepToRotate<0){
						index += sprintf(chBufferTx_USART0+index, ";%03d;%04d\n\r", relativePositionBySteps, stepToRotate);
					}
					else{
						index += sprintf(chBufferTx_USART0+index, ";%03d;+%03d\n\r", relativePositionBySteps, stepToRotate);
					}
					Transmitir_Buffer_USART0(chBufferTx_USART0, USART0_BUFFER_ZISE);
				}

				uContPeriodoPID = uPeriodoPID;
			}

			// impresion de variables
			if (chPrintEstado == SI){
				int index = sprintf(chBufferTx_USART0, "\rCaud= ");
				dtostrf(flCaudalMedido, 4, 1, chBufferTx_USART0+index);
				index += 4;
				index += sprintf(chBufferTx_USART0+index, " l/min; SetTemp= ");
				dtostrf(flTempSetP, 4, 1, chBufferTx_USART0+index);
				index += 4;
				index += sprintf(chBufferTx_USART0+index, " C; AcTemp= ");
				dtostrf(flTempMedida, 4, 1, chBufferTx_USART0+index);
				index += 4;
				if (stepToRotate<0){
					index += sprintf(chBufferTx_USART0+index, " C; ValvPos= %03d%04d;", relativePositionBySteps, stepToRotate);
				}
				else{
					index += sprintf(chBufferTx_USART0+index, " C; ValvPos= %03d+%03d;", relativePositionBySteps, stepToRotate);
				}
				Transmitir_Buffer_USART0(chBufferTx_USART0, USART0_BUFFER_ZISE);
			}

			// detecto caudal parada
			if (flCaudalMedido < CAUDAL_PARADA){
				// cierro el gas
				CLEAR_BIT(RELAY_GAS_PORT, RELAY_GAS_PIN);
				uchEstadoControl = CONTROL_STANDBY;
			}
			break;

		default:
			Inicio_Control_Temp();
			break;
	}
}

/*****************************************************************************
* Inicio de Maquina de estados
*****************************************************************************/

void Inicio_Control_Temp(void){
	chPrintEstado=NO;
	chPrintPID=NO;
	uchEstadoControl = CONTROL_STANDBY;
}
