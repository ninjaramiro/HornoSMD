/** \file	medicion_ADC.h
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente a la medicion de las entradas del ADC.
 *
 */

#ifndef MEDICION_ADC_H_
#define MEDICION_ADC_H_

/*****************************************************************************
* Definicion de los prototipos de las funciones
*****************************************************************************/
/** \brief	Maquina de estados de medicion de ADC.
 *
 */
void Medicion_ADC(void);

/** \brief	Funcion de inicio del ADC.
 *
 */
void Inicio_ADC (void);

/** \brief	Conmuta el mux de entrada del ADC.
 *
 *	\param[in]char chCanal, numero de canal al cual conmuta el mux\n
 */
void SW_Mux_ADC (char chCanal);

/** \brief	Conmuta la referencia del ADC.
 *
 *	\param[in]char chRef, conmuta la referencia de tension del ADC\n
 */
void SW_Ref_ADC (char chRef);

/** \brief	Carga los valores de correccion de linealidad segun el rango y sensor
 *
 */
void Cargar_Valores_Rango_Por_Canal (void);

/*****************************************************************************
* Definiciones máquina de estado de medicion
*****************************************************************************/

#define INICIO_MEDICION_ADC_1					0
#define MEDICION_ADC1							1
#define INICIO_MEDICION_ADC_2					2
#define MEDICION_ADC2							3
#define INICIO_MEDICION_ADC_3					4
#define MEDICION_ADC3							5
#define IMPRIMIR_ADC							8

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
// canales ADC
#define ADC_0	0
#define ADC_1	1
#define ADC_2	2
#define ADC_3	3
#define ADC_4	4
#define ADC_5	5
#define ADC_6	6
#define ADC_7	7

// referencias ADC
#define ADC_REF_AVCC		0
#define ADC_REF_INTERNAL	1
#define ADC_REF_OFF			2

// muestras ADC
#define MAXIMO_MUESTRAS_ADC					10000
#define MINIMO_MUESTRAS_ADC					1

// PWM 1
#define DEFAULT_MUESTRAS_ADC_PWM1			100
#define DEFAULT_PWM1_A_MA					51.2f

// PWM 2
#define DEFAULT_MUESTRAS_ADC_PWM2			100
#define DEFAULT_PWM2_A_MA					51.2f

// 0 a 20 Us Sensor Grande
#define COND_0_20_G							2
#define COND020G_MAX_US						20
#define COND020G_MIN_US						0
#define DEFAULT_COND020G_A_US				0.0171f
#define DEFAULT_COND020G_OFFSET				0

// 2 a 200 Us Sensor Grande
#define COND_2_200_G						3
#define COND2200G_MAX_US					200
#define COND2200G_MIN_US					2
#define DEFAULT_COND2200G_A_US				0.1723f
#define DEFAULT_COND2200G_OFFSET			0

// 20 a 2500 Us Sensor Grande
#define COND_20_2500_G						1
#define COND202500G_MAX_US					2500
#define COND202500G_MIN_US					20
#define DEFAULT_COND202500G_A_US			2.1818f
#define DEFAULT_COND202500G_OFFSET			0

// 0 a 20 Us Sensor Chico
#define COND_0_20_C							5
#define COND020C_MAX_US						20
#define COND020C_MIN_US						0
#define DEFAULT_COND020C_A_US				0.0204f
#define DEFAULT_COND020C_OFFSET				0

// 2 a 200 Us Sensor Chico
#define COND_2_200_C						6
#define COND2200C_MAX_US					200
#define COND2200C_MIN_US					2
#define DEFAULT_COND2200C_A_US				0.1973f
#define DEFAULT_COND2200C_OFFSET			0

// 20 a 2500 Us Sensor Chico
#define COND_20_2500_C						4
#define COND202500C_MAX_US					2500
#define COND202500C_MIN_US					20
#define DEFAULT_COND202500C_A_US			3.5518f
#define DEFAULT_COND202500C_OFFSET			0

// COND 1 & 2
#define DEFAULT_RANGO_COND_1				COND_20_2500_G
#define DEFAULT_MUESTRAS_COND1				500
#define DEFAULT_COND1_A_US					DEFAULT_COND202500G_A_US
#define DEFAULT_OFFSET_COND1				DEFAULT_COND202500G_OFFSET
#define DEFAULT_RANGO_COND_2				COND_0_20_G
#define DEFAULT_MUESTRAS_COND2				500
#define DEFAULT_COND2_A_US					DEFAULT_COND020G_A_US
#define DEFAULT_OFFSET_COND2				DEFAULT_COND020G_OFFSET

#endif /* MEDICION_ADC_H_ */
