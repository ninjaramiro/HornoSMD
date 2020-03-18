/** \file	medicion_ADC.c
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente a la medicion de las entradas del ADC.
 */

#include "inicioUc.h"
#include "macros.h"
#include "USART0.h"
#include "medicion_ADC.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <avr/sleep.h>

/*****************************************************************************
* Tabla correccion linealidad
*****************************************************************************/
#define CANT_STEPS_CORR_LIN						31
const float flCuentasADCcorrLin[]			= {997,		991,	961,	945,	935,	903,	902,	846,	821,	559,	548,	325,	214,	157,	121,	112,	77,		75,		64,		50,		47,		29,		25,		23,		20,		19,		18,		12,		8,		5,		0		};

const float Cond_0_20_G_Factor[]	PROGMEM = {1.148,	1.148,	1.148,	1.057,	1.057,	1.057,	1.057,	1.057,	1.057,	1.000,	1.000,	1.000,	1.000,	1.000,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.779,	0.692,	0.843	};
const float Cond_2_200_G_Factor[]	PROGMEM = {1.157,	1.157,	1.157,	1.157,	1.071,	1.071,	1.071,	1.071,	1.071,	1.071,	1.071,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	0.974,	0.974,	0.974,	0.944,	0.944,	0.908,	0.908,	0.908,	0.908,	0.883,	0.883,	0.883,	0.883	};
const float Cond_20_2500_G_Factor[]	PROGMEM = {1.647,	1.156,	1.118,	1.118,	1.118,	1.118,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	0.949,	0.949,	0.949,	0.949,	0.949,	0.829,	0.829,	0.829,	0.829,	0.829,	0.829,	0.829,	0.829,	0.829,	0.829,	0.829	};
const float Cond_0_20_C_Factor[]	PROGMEM = {1.021,	1.021,	1.021,	1.021,	1.021,	0.904,	0.904,	0.904,	0.904,	0.904,	0.904,	0.904,	0.904,	0.904,	0.904,	0.904,	0.904,	1.000,	1.000,	1.000,	1.000,	1.000,	1.117,	1.117,	1.117,	1.117,	1.001,	1.001,	1.001,	1.001,	1.001	};
const float Cond_2_200_C_Factor[]	PROGMEM = {1.110,	1.110,	1.110,	1.110,	1.110,	1.110,	1.110,	1.009,	1.009,	1.009,	0.980,	0.980,	1.000,	1.000,	1.000,	1.000,	1.046,	1.046,	1.046,	1.046,	1.046,	1.046,	1.046,	1.046,	1.046,	0.980,	0.980,	0.980,	0.980,	0.817,	0.817	};
const float Cond_20_2500_C_Factor[]	PROGMEM = {0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	0.977,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	1.000,	1.074,	1.074,	1.175,	1.175	};

/*****************************************************************************
* Variables Máquina de estados de ADC
*****************************************************************************/
unsigned char			uchEstadoADC;			/**< \brief variable de la maquina de estados del ADC */
char					chADCprint;				/**< \brief indica si se imprime la medicion de los ADC */
//unsigned int			uContMuestrasADC;		/**< \brief contador de promedio de muestras */

unsigned int			uMuestrasADCpwm1;		/**< \brief cantidad de muestras para promediar */
float					flPWM1aMA;				/**< \brief valor de conversion del ADC a corriente */
float					flPWM1valorMA;			/**< \brief valor medio en mA medidos por el ADC */
char					chValorNuevoPWM1;		/**< \brief idica la obtencion de un valor nuevo medido */

unsigned int			uMuestrasADCpwm2;		/**< \brief cantidad de muestras para promediar */
float					flPWM2aMA;				/**< \brief valor de conversion del ADC a corriente */
float					flPWM2valorMA;			/**< \brief valor medio en mA medidos por el ADC */
char					chValorNuevoPWM2;		/**< \brief idica la obtencion de un valor nuevo medido */

unsigned int			uRangoCond1;			/**< \brief rango de trabajo del conductimetro segun la configuracion del hardware */
unsigned int			uMuestrasADCcond1;		/**< \brief cantidad de muestras para promediar */
float					flCond1auS;				/**< \brief valor de conversion del ADC a uS/cm */
volatile float			flCond1valoruS;			/**< \brief valor medio en uS/cm medidos por el ADC */
volatile float	 		flValorADCcond1;		/**< \brief valor del ADC en cuentas para los rangos segun correccion de linealidad */
float 					flOffsetADCcond1;			/**< \brief corracion de offset del cond 1 para la puesta a cero */
int						cond1MaxUs;
int						cond1MinUs;
float					flCond1Factor[CANT_STEPS_CORR_LIN];

unsigned int			uRangoCond2;			/**< \brief rango de trabajo del conductimetro segun la configuracion del hardware */
unsigned int			uMuestrasADCcond2;		/**< \brief cantidad de muestras para promediar */
float					flCond2auS;				/**< \brief valor de conversion del ADC a uS/cm */
volatile float			flCond2valoruS;			/**< \brief valor medio en uS/cm medidos por el ADC */
volatile float 			flValorADCcond2;		/**< \brief valor del ADC en cuentas para los rangos segun correccion de linealidad */
float 					flOffsetADCcond2;			/**< \brief corracion de offset del cond 2 para la puesta a cero */
int						cond2MaxUs;
int						cond2MinUs;
float					flCond2Factor[CANT_STEPS_CORR_LIN];

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern char					chBufferTx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de transmisión USART del uc */

/*****************************************************************************
* Maquina de estados del ADC
*****************************************************************************/
void Medicion_ADC(void) {
	switch (uchEstadoADC) {

	case MEDICION_ADC0_PWM_420mA_2_START_CONV:
		SET_BIT(ADCSRA, ADSC);
		uchEstadoADC = MEDICION_ADC0_PWM_420mA_2;
		break;

	case MEDICION_ADC0_PWM_420mA_2:
			if ((ADCSRA & (SET<<ADSC)) == 0) {													// si termino la conversio espero
				flPWM2valorMA = flPWM2valorMA + (((float)ADC / flPWM2aMA) - flPWM2valorMA) / (float)uMuestrasADCpwm2;	// ago una media movil											// cargo el valor medido
//
//				if (uContMuestrasADC < uMuestrasADCpwm2) {									// verifico si es el total de muesras a promediar
//					uContMuestrasADC++;
//				}
//				else {
//					uContMuestrasADC=0;
					chValorNuevoPWM2 = SI;
					SW_Mux_ADC(ADC_2);														// cambio el mux
					SW_Ref_ADC(ADC_REF_AVCC);												// fijo la referencia de tension de ADC
					uchEstadoADC = MEDICION_ADC2_PWM_420mA_1_START_CONV;
//				}
//				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;

	case MEDICION_ADC2_PWM_420mA_1_START_CONV:
		SET_BIT(ADCSRA, ADSC);
		uchEstadoADC = MEDICION_ADC2_PWM_420mA_1;
		break;

	case MEDICION_ADC2_PWM_420mA_1:
			if ((ADCSRA & (SET<<ADSC)) == 0) {													// si no termino la conversio espero
				flPWM1valorMA = flPWM1valorMA + (((float)ADC / flPWM1aMA) - flPWM1valorMA) / (float)uMuestrasADCpwm1;	// ago una media movil											// cargo el valor medido

//				if (uContMuestrasADC < uMuestrasADCpwm1) {
//					uContMuestrasADC++;
//				}
//				else {
//					uContMuestrasADC=0;
					chValorNuevoPWM1 = SI;
					SW_Mux_ADC(ADC_6);														// cambio el mux
					SW_Ref_ADC(ADC_REF_INTERNAL);											// fijo la referencia de tension de ADC
					uchEstadoADC = MEDICION_ADC6_CONDUCT_2_START_CONV;
//				}
//				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;

	case MEDICION_ADC6_CONDUCT_2_START_CONV:
		SET_BIT(ADCSRA, ADSC);
		uchEstadoADC = MEDICION_ADC6_CONDUCT_2;
		break;

	case MEDICION_ADC6_CONDUCT_2:
			if ((ADCSRA & (SET<<ADSC)) == 0) {													// si no termino la conversio espero
				// media movil
				flValorADCcond2 = flValorADCcond2 + ((float)ADC - flValorADCcond2) / (float)uMuestrasADCcond2;

				// correcion de linealidad
				int i=0;
				while (flValorADCcond2 < flCuentasADCcorrLin[i]){
					i++;
				}

				// calculo de conductividad
				flCond2valoruS = (flValorADCcond2 - flOffsetADCcond2) * flCond2auS * flCond2Factor[i];

//				// control de cantifdad de muestras
//				if (uContMuestrasADC < uMuestrasADCcond2) {
//					uContMuestrasADC++;
//				}
//				else {
//					uContMuestrasADC=0;
					SW_Mux_ADC(ADC_7);														// cambio el mux
					SW_Ref_ADC(ADC_REF_INTERNAL);											// fijo la referencia de tension de ADC
					uchEstadoADC = MEDICION_ADC7_CONDUCT_1_START_CONV;
//				}
				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;

	case MEDICION_ADC7_CONDUCT_1_START_CONV:
		SET_BIT(ADCSRA, ADSC);
		uchEstadoADC = MEDICION_ADC7_CONDUCT_1;
		break;

	case MEDICION_ADC7_CONDUCT_1:
			if ((ADCSRA & (SET<<ADSC)) == 0) {												// si no termino la conversio espero
				// media movil
				flValorADCcond1 = flValorADCcond1 + ((float)ADC - flValorADCcond1) / (float)uMuestrasADCcond1;

				// correcion de linealidad
				int i=0;
				while (flValorADCcond1 < flCuentasADCcorrLin[i]){
					i++;
				}

				// calculo de conductividad
				flCond1valoruS = (flValorADCcond1 - flOffsetADCcond1) * flCond1auS * flCond1Factor[i];

//				// control de cantifdad de muestras
//				if (uContMuestrasADC < uMuestrasADCcond1) {
//					uContMuestrasADC++;
//				}
//				else {
//					uContMuestrasADC=0;
					SW_Mux_ADC(ADC_0);														// cambio el mux
					SW_Ref_ADC(ADC_REF_AVCC);												// fijo la referencia de tension de ADC
					uchEstadoADC = IMPRIMIR_ADC;
//				}
//				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;

	case IMPRIMIR_ADC:
		if (chADCprint == SI) {
			int index = sprintf(chBufferTx_USART0, "\rI_CH1= ");
			dtostrf(flPWM1valorMA, 5, 2, chBufferTx_USART0+index);
			index += 5;
			index += sprintf(chBufferTx_USART0+index, "mA; I_CH2= ");
			dtostrf(flPWM2valorMA, 5, 2, chBufferTx_USART0+index);
			index += 5;
			index += sprintf(chBufferTx_USART0+index, "mA; COND_CH1= ");
			dtostrf(flCond1valoruS, 7, 2, chBufferTx_USART0+index);
			index += 7;
			index += sprintf(chBufferTx_USART0+index, "uS/cm; COND_CH2= ");
			dtostrf(flCond2valoruS, 7, 2, chBufferTx_USART0+index);
			index += 7;
			sprintf(chBufferTx_USART0+index, "uS/cm.");

			Transmitir_Buffer_USART0(chBufferTx_USART0, USART0_BUFFER_ZISE);
		}
		uchEstadoADC = MEDICION_ADC0_PWM_420mA_2_START_CONV;
		break;

		default:
			Inicio_ADC();
			break;
	}
}

/*****************************************************************************
* Funcion de inicio del ADC
*****************************************************************************/
void SW_Mux_ADC (char chCanal) {
	if (chCanal == ADC_0){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
//		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(OFF<<MUX2)|(OFF<<MUX1)|(OFF<<MUX0));
	}
	else if (chCanal == ADC_1){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(OFF<<MUX2)|(OFF<<MUX1)|(SET<<MUX0));
	}
	else if (chCanal == ADC_2){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(OFF<<MUX2)|(SET<<MUX1)|(OFF<<MUX0));
	}
	else if (chCanal == ADC_3){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(OFF<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
	}
	else if (chCanal == ADC_4){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(SET<<MUX2)|(OFF<<MUX1)|(OFF<<MUX0));
	}
	else if (chCanal == ADC_5){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(SET<<MUX2)|(OFF<<MUX1)|(SET<<MUX0));
	}
	else if (chCanal == ADC_6){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(OFF<<MUX0));
	}
	else if (chCanal == ADC_7){
		ADMUX &= ~((SET<<MUX4)|(SET<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
		ADMUX |= ((OFF<<MUX4)|(OFF<<MUX3)|(SET<<MUX2)|(SET<<MUX1)|(SET<<MUX0));
	}
}

/*****************************************************************************
* Funcion de cambio de referencia del ADC
*****************************************************************************/
void SW_Ref_ADC (char chRef) {
	if (chRef == ADC_REF_AVCC){
		CLEAR_BIT(ADMUX, REFS1);
		SET_BIT(ADMUX, REFS0);
	}
	else if (chRef == ADC_REF_INTERNAL){
		SET_BIT(ADMUX, REFS1);
		SET_BIT(ADMUX, REFS0);
	}
	else {
		CLEAR_BIT(ADMUX, REFS1);
		CLEAR_BIT(ADMUX, REFS0);
	}
}

/*****************************************************************************
* Funcion de inicio del ADC
*****************************************************************************/
void Inicio_ADC (void) {
	// inicio el ADC en modo de conversión simple, con el preescaler en 128 y sin interrupción y referencia 2.56v interna.
	ADMUX = (OFF<<REFS1)|(SET<<REFS0)|(OFF<<ADLAR);							// externa AVCC
	SW_Mux_ADC(ADC_0);														// cambio el mux
	ADCSRA = (ON<<ADEN)|(ON<<ADSC)|(OFF<<ADFR)|(SET<<ADIF)|(OFF<<ADIE)|(SET<<ADPS2)|(SET<<ADPS1)|(SET<<ADPS0);

	flPWM1valorMA = 0;
	flPWM2valorMA = 0;
	flCond1valoruS = 0;
	flCond2valoruS = 0;
	flValorADCcond2 = 0;
	flValorADCcond1 = 0;

//	uContMuestrasADC = 0;
	chADCprint = NO;

	chValorNuevoPWM1=NO;
	chValorNuevoPWM2=NO;

	uchEstadoADC = MEDICION_ADC0_PWM_420mA_2_START_CONV;
}

/*****************************************************************************
* Funcion de carga de los valores de correccion de linealidad
*****************************************************************************/
void Cargar_Valores_Rango_Por_Canal (void){
	// cargo los valores para la correccion de linealidad segun el rango para cada cond 1
	if (uRangoCond1 == COND_20_2500_G){
		cond1MaxUs = COND202500G_MAX_US;
		cond1MinUs = COND202500G_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond1Factor[i] = pgm_read_float(&Cond_20_2500_G_Factor[i]);
		}
	}
	else if (uRangoCond1 == COND_0_20_G){
		cond1MaxUs = COND020G_MAX_US;
		cond1MinUs = COND020G_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond1Factor[i] = pgm_read_float(&Cond_0_20_G_Factor[i]);
		}
	}
	else if (uRangoCond1 == COND_2_200_G){
		cond1MaxUs = COND2200G_MAX_US;
		cond1MinUs = COND2200G_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond1Factor[i] = pgm_read_float(&Cond_2_200_G_Factor[i]);
		}
	}
	else if (uRangoCond1 == COND_20_2500_C){
		cond1MaxUs = COND202500C_MAX_US;
		cond1MinUs = COND202500C_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond1Factor[i] = pgm_read_float(&Cond_20_2500_C_Factor[i]);
		}
	}
	else if (uRangoCond1 == COND_0_20_C){
		cond1MaxUs = COND020C_MAX_US;
		cond1MinUs = COND020C_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond1Factor[i] = pgm_read_float(&Cond_0_20_C_Factor[i]);
		}
	}
	else if (uRangoCond1 == COND_2_200_C){
		cond1MaxUs = COND2200C_MAX_US;
		cond1MinUs = COND2200C_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond1Factor[i] = pgm_read_float(&Cond_2_200_C_Factor[i]);
		}
	}

	// cargo los valores para la correccion de linealidad segun el rango para cada cond 2
	if (uRangoCond2 == COND_20_2500_G){
		cond2MaxUs = COND202500G_MAX_US;
		cond2MinUs = COND202500G_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond2Factor[i] = pgm_read_float(&Cond_20_2500_G_Factor[i]);
		}
	}
	else if (uRangoCond2 == COND_0_20_G){
		cond2MaxUs = COND020G_MAX_US;
		cond2MinUs = COND020G_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond2Factor[i] = pgm_read_float(&Cond_0_20_G_Factor[i]);
		}
	}
	else if (uRangoCond2 == COND_2_200_G){
		cond2MaxUs = COND2200G_MAX_US;
		cond2MinUs = COND2200G_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond2Factor[i] = pgm_read_float(&Cond_2_200_G_Factor[i]);
		}
	}
	else if (uRangoCond2 == COND_20_2500_C){
		cond2MaxUs = COND202500C_MAX_US;
		cond2MinUs = COND202500C_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond2Factor[i] = pgm_read_float(&Cond_20_2500_C_Factor[i]);
		}
	}
	else if (uRangoCond2 == COND_0_20_C){
		cond2MaxUs = COND020C_MAX_US;
		cond2MinUs = COND020C_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond2Factor[i] = pgm_read_float(&Cond_0_20_C_Factor[i]);
		}
	}
	else if (uRangoCond2 == COND_2_200_C){
		cond2MaxUs = COND2200C_MAX_US;
		cond2MinUs = COND2200C_MIN_US;
		for (int i=0; i<CANT_STEPS_CORR_LIN; i++){
			flCond2Factor[i] = pgm_read_float(&Cond_2_200_C_Factor[i]);
		}
	}
}

