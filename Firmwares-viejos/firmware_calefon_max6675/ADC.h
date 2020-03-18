/** \file	temperatura.h
 *  \brief	Contiene las funciones de manejo de medición de temperatura con LM35DZ.
 *
 *	Contiene las funciones de manejo de medición de temperatura y medicion con el ADC \
 */

#ifndef ADC_H_
#define ADC_H_

/*****************************************************************************
* Prototipos
*****************************************************************************/
/** \brief	maquina de estados que toma el valor de temperatura.
 */
void valor_ADC(void);

/** \brief	función de inicio de la maquina de estados que toma el valor de temperatura.
 */
void Inicio_valor_ADC(void);

/*****************************************************************************
* Definiciones
*****************************************************************************/

#define CANTIDAD_ADC_PROMEDIO	100

#define ADC_6_A_TENSION			64.87f
#define ADC_7_MAX_SET_TEMP		70.0f
#define ADC_7_MIN_SET_TEMP		20.0f

#define TENSION_INICIO_CARGA	13.0f	// tension en V para el inicio de carga de la batria
#define TENSION_FIN_CARGA		15.0f	// tension de fin de carga de la batria

/*****************************************************************************
* Definiciones Estados
*****************************************************************************/
#define CAPTURA_ADC_6_TENSION_BATERIA_START_CONV	0
#define CAPTURA_ADC_6_TENSION_BATERIA				1
#define CAPTURA_ADC_7_SET_TEMP_START_CONV			2
#define CAPTURA_ADC_7_SET_TEMP						3

#endif /* ADC_H_ */
