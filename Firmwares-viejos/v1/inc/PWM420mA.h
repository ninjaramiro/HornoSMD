/** \file	PWM420mA.h
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente al lazo de control PWM para las salidas 4-20mA
 *
 */

#ifndef PWM420MA_H_
#define PWM420MA_H_

/*****************************************************************************
* Definicion de los prototipos de las funciones
*****************************************************************************/
/** \brief	Maquina de estados de actualizacion del lazo de control para las salidas PWM de 4-20mA.
 *
 */
void Actualizar_PWM_420mA (void);

/** \brief	Funcion de inicio del PWM para las salidas de 4-20mA.
 *
 */
void Inicio_PWM_420mA (void);

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/
#define PWM_STANDBY				1
#define PWM_1_420MA_ACTUALIZAR	2
#define PWM_2_420MA_ACTUALIZAR	3

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define MAXIMO_GANANCIA_LAZO_PWM		1000
#define MINIMO_GANANCIA_LAZO_PWM		1

#define DEFAULT_GANACIA_LAZO_PWM_1		300
#define DEFAULT_GANACIA_LAZO_PWM_2		300

#define I_MAX_MA						20
#define I_MIN_MA						4
#define MAX_PWM_RES_MA					0.02f
#define VALOR_PWM_SIN_GANANCIA_MA		0.2f
#define CUENTAS_MAX_PWM					1024

#endif /* PWM420MA_H_ */
