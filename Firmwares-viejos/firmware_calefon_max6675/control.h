/*
 * control.h
 *
 *  Created on: Feb 16, 2018
 *      Author: poroto
 */

#ifndef CONTROL_H_
#define CONTROL_H_

/*****************************************************************************
* Definicion de los prototipos de las funciones
*****************************************************************************/
/** \brief	Maquina de estados de control de temperatura.
 *
 */
void Control_Temp (void);

/** \brief	Funcion de inicio del lazo de control de tempertura.
 *
 */
void Inicio_Control_Temp (void);

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/
#define CONTROL_STANDBY			0
#define CONTROL_PID_TEMP		1

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define MAX_TEMPERATURA_SEGURIDAD			80.0 // °C
#define MIN_TEMPERATURA_SEGURIDAD			0.0 // °C
#define MAX_CAUDAL_SEGURIDAD				15.0 // l/min
#define MIN_CAUDAL_SEGURIDAD				0.0 // l/min
#define MAX_VARIACION_CAUDAL_SEGURIDAD		1.0 // x100%

#define CAUDAL_ARRANQUE						3.0 // l/min caudal de arranque
#define CAUDAL_PARADA						2.5 // l/min

#define PERFIL_DEFAULT						0
#define DEFAULT_PID_PERIODO					3000
#define DEFAULT_PID_KP						1
#define DEFAULT_PID_KI						0.01
#define DEFAULT_PID_KD						50
#define DEFAULT_MAX_PID_INTEG				5.0
#define DEFAULT_VARIACION_CAUDAL			25.0
#define DEFAULT_PERIODO_VEDA_PID			12000
#define DEFAULT_PERIODO_VEDA_CAUDAL			65000
#define DEFAULT_CAUDAL_ESCALA_KS_PID		5.0
#define DEFAULT_ESCALA_VARIACION_CAUDAL		1.1

#define PERFIL_1							1
#define PERFIL_1_PID_PERIODO				2000
#define PERFIL_1_PID_KP						2
#define PERFIL_1_PID_KI						0.2
#define PERFIL_1_PID_KD						60
#define PERFIL_1_MAX_PID_INTEG				5.0
#define PERFIL_1_VARIACION_CAUDAL			30.0
#define PERFIL_1_PERIODO_VEDA_PID			12000
#define PERFIL_1_PERIODO_VEDA_CAUDAL		65000
#define PERFIL_1_CAUDAL_ESCALA_KS_PID		0.0
#define PERFIL_1_ESCALA_VARIACION_CAUDAL	1.0

#define PERFIL_2							2
#define PERFIL_2_PID_PERIODO				2000
#define PERFIL_2_PID_KP						4.86
#define PERFIL_2_PID_KI						0.278
#define PERFIL_2_PID_KD						21.3
#define PERFIL_2_MAX_PID_INTEG				5.0
#define PERFIL_2_VARIACION_CAUDAL			30.0
#define PERFIL_2_PERIODO_VEDA_PID			12000
#define PERFIL_2_PERIODO_VEDA_CAUDAL		65000
#define PERFIL_2_CAUDAL_ESCALA_KS_PID		0.0
#define PERFIL_2_ESCALA_VARIACION_CAUDAL	1.0

#endif /* CONTROL_H_ */
