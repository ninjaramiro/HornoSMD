/*
 * motorPP.h
 *
 *  Created on: Feb 7, 2018
 *      Author: poroto
 */

#ifndef MOTORPP_H_
#define MOTORPP_H_

/*****************************************************************************
* Definicion de los prototipos de las funciones
*****************************************************************************/
/** \brief	Maquina de estados de control de motor PP.
 *
 */
void Rotar_Motor_PP (void);

void Inicio_Motor_PP (void);

void Posicionar_MotorPP (unsigned int position);

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/
#define MOTOR_PP_STANDBY			1
#define MOTOR_PP_ROTAR				2

#define MOTOR_PP_ROTAR_STEP_1		50
#define MOTOR_PP_ROTAR_STEP_12		51
#define MOTOR_PP_ROTAR_STEP_2		52
#define MOTOR_PP_ROTAR_STEP_23		53
#define MOTOR_PP_ROTAR_STEP_3		54
#define MOTOR_PP_ROTAR_STEP_34		55
#define MOTOR_PP_ROTAR_STEP_4		56
#define MOTOR_PP_ROTAR_STEP_41		57

#define STATE_MOTOR_1				1
#define STATE_MOTOR_2				2
#define STATE_MOTOR_3				3
#define STATE_MOTOR_4				4
#define STATE_MOTOR_STOP			-10

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
// llama minima relative position 16
// llama maxima relative position 130

#define STEP_FULL_CERRADO_INICIO	300
#define MAX_MOTOR_PP_ACUMULADOS		200
#define MIN_MOTOR_PP_ACUMULADOS		16	// conviene multiplo de 4 por la posicion de inicio

#define TIEMPO_ENTRE_STEP			5

#endif /* MOTORPP_H_ */
