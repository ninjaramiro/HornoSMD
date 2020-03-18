/** \file	caudales.h
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente a la medicion de caudales
 *
 */

#ifndef CAUDALES_H_
#define CAUDALES_H_

/*****************************************************************************
* Definicion de los prototipos de las funciones
*****************************************************************************/
/** \brief	Maquina de estados de medicion de caudales.
 *
 */
void Medicion_Caudales (void);

/** \brief	Funcion de inicio de configuracion del timer 1 para medir caudales.
 *
 */
void Configuracion_Timer_Medicion_Caudales (void);

/** \brief	Funcion de inicio de medicion de caudales.
 */
void Iniciar_Medicion_Caudal (void);

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/
#define MEDICION_CAUDAL_STANDBY			1
#define MEDICION_CAUDAL					2

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
// etapas de medicion
#define ETAPA_MEDICION_STANDBY			0
#define ETAPA_MEDICION_CAPTURA_1		1
#define ETAPA_MEDICION_CAPTURA_2		2
#define ETAPA_MEDICION_CALCULO			3
#define ETAPA_MEDICION_OVERFLOW			4

//varias
#define TIEMPO_MEDICION_CAUDAL_STANDBY	10	//tiempo entre mediciones en ciclos timer 0 para dejar correr las maquinas de estado

// caudal	// 1-30l/min 450pul/ltr 133.3-4.4mseg 7.5-225Hz
#define PULSOS_X_LITRO_CAUDAL			450
#define MAXIMO_MUESTRAS_CAUDAL			10

#endif /* CAUDALES_H_ */
