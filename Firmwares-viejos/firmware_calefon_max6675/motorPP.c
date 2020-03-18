/*
 * motorPP.c
 *
 *  Created on: Feb 7, 2018
 *      Author: poroto
 */


#include "motorPP.h"
#include "macros.h"
#include "inicio.h"
#include "USART0.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoMotorPP;			/**< \brief variable de la maquina de estados de control de motor PP */
unsigned char			uchEstadoRotar;
int 					stepToRotate;				/**< \brief cantidad de pasos a rotar, + derecha, - izquierda */
int 					stateMotorPP;				/**< \brief variable que indica la posición/step en que se encuentra el rotor del motor */
volatile unsigned int	uContMotorPP;				/**< \brief contador de tiempo para el control del motor, ejemplo duracion del step */
int 					relativePositionBySteps;	/**< \brief posicion relativa de la valvula en cantidad de steps realizados */

/*****************************************************************************
* Variables externas
*****************************************************************************/

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Rotar_Motor_PP (void) {
	switch (uchEstadoMotorPP) {
		case MOTOR_PP_STANDBY:
			if (uContMotorPP == 0){
				if ((stepToRotate > 0) && (relativePositionBySteps < MAX_MOTOR_PP_ACUMULADOS)){
					switch (stateMotorPP) { //1->3->2->4->1
						case STATE_MOTOR_1:
							relativePositionBySteps++;
							stepToRotate--;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_3;
							break;

						case STATE_MOTOR_2:
							relativePositionBySteps++;
							stepToRotate--;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_4;
							break;

						case STATE_MOTOR_3:
							relativePositionBySteps++;
							stepToRotate--;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_2;
							break;

						case STATE_MOTOR_4:
							relativePositionBySteps++;
							stepToRotate--;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_1;
							break;
					}
					uchEstadoMotorPP = MOTOR_PP_ROTAR;
				}
				else if ((stepToRotate < 0) && (relativePositionBySteps > MIN_MOTOR_PP_ACUMULADOS)){
					switch (stateMotorPP) { //1->4->2->3->1
						case STATE_MOTOR_1:
							relativePositionBySteps--;
							stepToRotate++;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_4;
							break;

						case STATE_MOTOR_2:
							relativePositionBySteps--;
							stepToRotate++;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_3;
							break;

						case STATE_MOTOR_3:
							relativePositionBySteps--;
							stepToRotate++;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_1;
							break;

						case STATE_MOTOR_4:
							relativePositionBySteps--;
							stepToRotate++;
							uchEstadoRotar = MOTOR_PP_ROTAR_STEP_2;
							break;
					}
					uchEstadoMotorPP = MOTOR_PP_ROTAR;
				}
				else{
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);
					if ((stepToRotate > 0) && (relativePositionBySteps == MAX_MOTOR_PP_ACUMULADOS)){
						stepToRotate=0;
					}
					else if ((stepToRotate < 0) && (relativePositionBySteps == MIN_MOTOR_PP_ACUMULADOS)){
						stepToRotate=0;
					}
				}
			}
			break;


		case MOTOR_PP_ROTAR:
			switch (uchEstadoRotar) {
				case MOTOR_PP_ROTAR_STEP_1:
					SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
					stateMotorPP = STATE_MOTOR_STOP;
					uchEstadoRotar = MOTOR_PP_ROTAR_STEP_12;
					break;

				case MOTOR_PP_ROTAR_STEP_12:
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
					stateMotorPP = STATE_MOTOR_1;
					break;

				case MOTOR_PP_ROTAR_STEP_2:
					SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
					stateMotorPP = STATE_MOTOR_STOP;
					uchEstadoRotar = MOTOR_PP_ROTAR_STEP_23;
					break;

				case MOTOR_PP_ROTAR_STEP_23:
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
					stateMotorPP = STATE_MOTOR_2;
					break;

				case MOTOR_PP_ROTAR_STEP_3:
					SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
					stateMotorPP = STATE_MOTOR_STOP;
					uchEstadoRotar = MOTOR_PP_ROTAR_STEP_34;
					break;

				case MOTOR_PP_ROTAR_STEP_34:
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
					stateMotorPP = STATE_MOTOR_3;
					break;

				case MOTOR_PP_ROTAR_STEP_4:
					SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);
					stateMotorPP = STATE_MOTOR_STOP;
					uchEstadoRotar = MOTOR_PP_ROTAR_STEP_41;
					break;

				case MOTOR_PP_ROTAR_STEP_41:
					CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);
					stateMotorPP = STATE_MOTOR_4;
					break;

				default:
					stateMotorPP = STATE_MOTOR_1;
					uchEstadoRotar = MOTOR_PP_ROTAR_STEP_1;
					break;
			}
			uContMotorPP = TIEMPO_ENTRE_STEP;
			uchEstadoMotorPP = MOTOR_PP_STANDBY;
			break;

		default:
			Inicio_Motor_PP();
			break;
	}
}

/*****************************************************************************
* inicio motor PP
*****************************************************************************/
void Inicio_Motor_PP (void) {
	CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
	CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
	CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
	CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);

	// cierro completo
	stepToRotate = -STEP_FULL_CERRADO_INICIO;
	while (stepToRotate < 0){//1->4->2->3->1
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);
		_delay_ms(TIEMPO_ENTRE_STEP);
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
		_delay_ms(TIEMPO_ENTRE_STEP);
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
		_delay_ms(TIEMPO_ENTRE_STEP);
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
		_delay_ms(TIEMPO_ENTRE_STEP);

		stepToRotate += 4;
	}

	// abro al minimo
	stepToRotate = MIN_MOTOR_PP_ACUMULADOS;
	while (stepToRotate > 0){//1->3->2->4->1
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_1);
		_delay_ms(TIEMPO_ENTRE_STEP);
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_3);
		_delay_ms(TIEMPO_ENTRE_STEP);
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_2);
		_delay_ms(TIEMPO_ENTRE_STEP);
		SET_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);
		_delay_ms(TIEMPO_ENTRE_STEP);
		CLEAR_BIT(MOTOR_PP_PORT, MOTOR_PP_PIN_4);
		_delay_ms(TIEMPO_ENTRE_STEP);
		stepToRotate -= 4;
	}

	stepToRotate = 0;
	relativePositionBySteps = MIN_MOTOR_PP_ACUMULADOS;
	stateMotorPP = STATE_MOTOR_4;
	uchEstadoRotar = MOTOR_PP_ROTAR_STEP_4;
	uchEstadoMotorPP = MOTOR_PP_STANDBY;
}
