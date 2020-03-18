/** \file	medicion_ADC.h
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente a la medicion de las entradas del ADC.
 *
 *  El Adquisidor medira en forma ciclica cada entrada analogica al dispositivo.
 *  Actualmente el mismo tiene configurada dos entradas diferenciales, correspondientes al ADC0-ADC1 y
 *  al ADC2-ADC3 y dos entradas simples correspondientes al ADC4 y al ADC5.
 *  Las entradas diferenciales conmutan entre una ganancia X10 y una ganancia X1 para aumentar el rango
 *  dinamico de medicion en forma automatica.
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

/*****************************************************************************
* Definiciones máquina de estado de medicion
*****************************************************************************/

#define MEDICION_ADC0_PWM_420mA_2		0
#define MEDICION_ADC2_PWM_420mA_1		1
#define MEDICION_ADC6_CONDUCT_2			2
#define MEDICION_ADC7_CONDUCT_1			3
#define IMPRIMIR_ADC					4

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define MAXIMO_MUESTRAS_ADC			10000
#define MINIMO_MUESTRAS_ADC			1

// PWM 1
#define DEFAULT_MUESTRAS_ADC_PWM1	500
#define DEFAULT_PWM1_A_MA			48.0f

// PWM 2
#define DEFAULT_MUESTRAS_ADC_PWM2	500
#define DEFAULT_PWM2_A_MA			48.0f

// conductimetro 2
#define MAX_COND2_US					2500
#define MIN_COND2_US					20
#define DEFAULT_MUESTRAS_ADC_COND2		500
#define DEFAULT_COND2_A_US				2.182f
#define ADC_COND_2_MUESTRAS_HIGH_HIGH	990
#define ADC_COND_2_MUESTRAS_HIGH		900
#define ADC_COND_2_MUESTRAS_LOW			100
#define ADC_COND_2_MUESTRAS_LOW_LOW		10
#define ADC_COND_2_FACTOR_HIGH_HIGH		1.401f
#define ADC_COND_2_FACTOR_HIGH			1.126f
#define ADC_COND_2_FACTOR_LOW			0.833f
#define ADC_COND_2_FACTOR_LOW_LOW		0.833f

// conductimetro 1
#define MAX_COND1_US					20
#define MIN_COND1_US					0
#define DEFAULT_MUESTRAS_ADC_COND1		500
#define DEFAULT_COND1_A_US				0.0171f
#define DEFAULT_OFFSET_COND1			0
#define ADC_COND_1_MUESTRAS_HIGH_HIGH	950
#define ADC_COND_1_MUESTRAS_HIGH		900
#define ADC_COND_1_MUESTRAS_LOW			100
#define ADC_COND_1_MUESTRAS_LOW_LOW		10
#define ADC_COND_1_FACTOR_HIGH_HIGH		1.148f
#define ADC_COND_1_FACTOR_HIGH			1.057f
#define ADC_COND_1_FACTOR_LOW			0.792f
#define ADC_COND_1_FACTOR_LOW_LOW		0.692f

// canales ADC
#define ADC_0	0
#define ADC_1	1
#define ADC_2	2
#define ADC_3	3
#define ADC_4	4
#define ADC_5	5
#define ADC_6	6
#define ADC_7	7

#endif /* MEDICION_ADC_H_ */
