/** \file	alarmas.h
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente al control de las alarmas
 *
 */

#ifndef ALARMAS_H_
#define ALARMAS_H_

/*****************************************************************************
* Definicion de los prototipos de las funciones
*****************************************************************************/
/** \brief	Maquina de estados de actualizacion de alarmas.
 *
 */
void Actualizar_Alarmas (void);


/** \brief	Funcion de inicio del PWM para las salidas de 4-20mA.
 *
 */
void Inicio_Alarmas (void);

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/
#define ALARMA_CONDUCTIMETRO_1	0
#define ALARMA_CONDUCTIMETRO_2	1
#define ALARMA_CONTROL			2

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define ALARMA_1							0	// posicion del bit de encendido de alarma
#define ALARMA_2							1

#define MAXIMO_TIEMPO_SILENCIAR_ALARMA		2880	//min
#define MINIMO_TIEMPO_SILENCIAR_ALARMA		1		//min
#define DEFAULT_TIEMPO_SILENCIAR_ALARMA		60		//min

#define MAXIMO_TIEMPO_ALARMA_COND			60	//seg
#define MINIMO_TIEMPO_ALARMA_COND			1	//seg

#define DEFAULT_TIEMPO_ALARMA_COND_1		30	//seg
#define DEFAULT_VALOR_ALARMA_COND_1			0.0f

#define DEFAULT_TIEMPO_ALARMA_COND_2		30	//seg
#define DEFAULT_VALOR_ALARMA_COND_2			0.0f

#endif /* ALARMAS_H_ */
