/** \file	medicion_ADC.c
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
* Variables Máquina de estados de ADC
*****************************************************************************/
unsigned char			uchEstadoADC;			/**< \brief variable de la maquina de estados del ADC */
char					chADCprint;				/**< \brief indica si se imprime la medicion de los ADC */
char					chValorNuevoADC;		/**< \brief idica la obtencion de un valor nuevo medido */
unsigned int			uContMuestrasADC;		/**< \brief contador de promedio de muestras */

unsigned int			uMuestrasADCpwm1;		/**< \brief cantidad de muestras para promediar */
float					flPWM1aMA;				/**< \brief valor de conversion del ADC a corriente */
float					flPWM1valorMA;			/**< \brief valor medio en mA medidos por el ADC */

unsigned int			uMuestrasADCpwm2;		/**< \brief cantidad de muestras para promediar */
float					flPWM2aMA;				/**< \brief valor de conversion del ADC a corriente */
float					flPWM2valorMA;			/**< \brief valor medio en mA medidos por el ADC */

unsigned int			uMuestrasADCcond1;		/**< \brief cantidad de muestras para promediar */
float					flCond1auS;				/**< \brief valor de conversion del ADC a uS/cm */
volatile float			flCond1valoruS;			/**< \brief valor medio en uS/cm medidos por el ADC */
unsigned int 			uValorADCcond1;			/**< \brief valor del ADC en cuentas para los rangos segun correccion de linealidad */
float 					flOffsetCond1;			/**< \brief corracion de offset del cond 1 para la puesta a cero */

unsigned int			uMuestrasADCcond2;		/**< \brief cantidad de muestras para promediar */
float					flCond2auS;				/**< \brief valor de conversion del ADC a uS/cm */
volatile float			flCond2valoruS;			/**< \brief valor medio en uS/cm medidos por el ADC */
unsigned int 			uValorADCcond2;			/**< \brief valor del ADC en cuentas para los rangos segun correccion de linealidad */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern char					chBufferTx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de transmisión USART del uc */

/*****************************************************************************
* Maquina de estados del ADC
*****************************************************************************/
void Medicion_ADC(void) {
	switch (uchEstadoADC) {

	case MEDICION_ADC0_PWM_420mA_2:
			if ((ADCSRA & (SET<<ADSC)) == 0) {													// si termino la conversio espero
				flPWM2valorMA = flPWM2valorMA + (((float)ADC / flPWM2aMA) - flPWM2valorMA) / uMuestrasADCpwm2;	// ago una media movil											// cargo el valor medido

				if (uContMuestrasADC < uMuestrasADCpwm2) {									// verifico si es el total de muesras a promediar
					uContMuestrasADC++;
				}
				else {
					uContMuestrasADC=0;
					SW_Mux_ADC(ADC_2);														// cambio el mux
					uchEstadoADC = MEDICION_ADC2_PWM_420mA_1;
				}
				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;


	case MEDICION_ADC2_PWM_420mA_1:
			if ((ADCSRA & (SET<<ADSC)) == 0) {													// si no termino la conversio espero
				flPWM1valorMA = flPWM1valorMA + (((float)ADC / flPWM1aMA) - flPWM1valorMA) / uMuestrasADCpwm1;	// ago una media movil											// cargo el valor medido

				if (uContMuestrasADC < uMuestrasADCpwm1) {
					uContMuestrasADC++;
				}
				else {
					uContMuestrasADC=0;
					SW_Mux_ADC(ADC_6);														// cambio el mux
					uchEstadoADC = MEDICION_ADC6_CONDUCT_2;
				}
				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;


	case MEDICION_ADC6_CONDUCT_2:
			if ((ADCSRA & (SET<<ADSC)) == 0) {													// si no termino la conversio espero
				uValorADCcond2 = ADC;
				float flAux;

				// correcion de linealidad
				if (uValorADCcond2 > ADC_COND_2_MUESTRAS_HIGH_HIGH){
					flAux = (float)uValorADCcond2 * flCond2auS * ADC_COND_2_FACTOR_HIGH_HIGH;
				}
				else if (uValorADCcond2 > ADC_COND_2_MUESTRAS_HIGH){
					flAux = (float)uValorADCcond2 * flCond2auS * ADC_COND_2_FACTOR_HIGH;
				}
				else if (uValorADCcond2 < ADC_COND_2_FACTOR_LOW_LOW){
					flAux = (float)uValorADCcond2 * flCond2auS * ADC_COND_2_FACTOR_LOW_LOW;
				}
				else if (uValorADCcond2 < ADC_COND_2_FACTOR_LOW){
					flAux = (float)uValorADCcond2 * flCond2auS * ADC_COND_2_FACTOR_LOW;
				}
				else{
					flAux = (float)uValorADCcond2 * flCond2auS;
				}

				flCond2valoruS = flCond2valoruS + (flAux - flCond2valoruS) / uMuestrasADCcond2;	// ago una media movil											// cargo el valor medido

				// control de cantifdad de muestras
				if (uContMuestrasADC < uMuestrasADCcond2) {
					uContMuestrasADC++;
				}
				else {
					uContMuestrasADC=0;
					SW_Mux_ADC(ADC_7);														// cambio el mux
					uchEstadoADC = MEDICION_ADC7_CONDUCT_1;
				}
				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;


	case MEDICION_ADC7_CONDUCT_1:
			if ((ADCSRA & (SET<<ADSC)) == 0) {													// si no termino la conversio espero
				uValorADCcond1 = ADC;
				float flAux;

				// correcion de linealidad
				if (uValorADCcond1 > ADC_COND_1_MUESTRAS_HIGH_HIGH){
					flAux = (float)uValorADCcond1 * flCond1auS * ADC_COND_1_FACTOR_HIGH_HIGH;
				}
				else if (uValorADCcond1 > ADC_COND_1_MUESTRAS_HIGH){
					flAux = (float)uValorADCcond1 * flCond1auS * ADC_COND_1_FACTOR_HIGH;
				}
				else if (uValorADCcond1 < ADC_COND_1_FACTOR_LOW_LOW){
					flAux = (float)uValorADCcond1 * flCond1auS * ADC_COND_1_FACTOR_LOW_LOW;
				}
				else if (uValorADCcond1 < ADC_COND_1_FACTOR_LOW){
					flAux = (float)uValorADCcond1 * flCond1auS * ADC_COND_1_FACTOR_LOW;
				}
				else {
					flAux = (float)uValorADCcond1 * flCond1auS;
				}

				flAux -= flOffsetCond1;	// correcion de cero

				flCond1valoruS = flCond1valoruS + (flAux - flCond1valoruS) / uMuestrasADCcond1;	// ago una media movil											// cargo el valor medido

				// control de cantifdad de muestras
				if (uContMuestrasADC < uMuestrasADCcond1) {
					uContMuestrasADC++;
				}
				else {
					uContMuestrasADC=0;
					SW_Mux_ADC(ADC_0);														// cambio el mux
					uchEstadoADC = IMPRIMIR_ADC;
				}
				SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
			}
			break;

	case IMPRIMIR_ADC:
		chValorNuevoADC = SI;

		if (chADCprint == SI) {
			int index = sprintf(chBufferTx_USART0, "\rADC_0= ");
			dtostrf(flPWM1valorMA, 5, 2, chBufferTx_USART0+index);
			index += 5;
			index += sprintf(chBufferTx_USART0+index, "mA; ADC_2= ");
			dtostrf(flPWM2valorMA, 5, 2, chBufferTx_USART0+index);
			index += 5;
			index += sprintf(chBufferTx_USART0+index, "mA; ADC_6= ");
			dtostrf(flCond1valoruS, 7, 2, chBufferTx_USART0+index);
			index += 7;
			index += sprintf(chBufferTx_USART0+index, "uS/cm; ADC_7= ");
			dtostrf(flCond2valoruS, 7, 2, chBufferTx_USART0+index);
			index += 7;
			sprintf(chBufferTx_USART0+index, "uS/cm.");

			Transmitir_Buffer_USART0(chBufferTx_USART0, USART0_BUFFER_ZISE);
		}

		uchEstadoADC = MEDICION_ADC0_PWM_420mA_2;
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
* Funcion de inicio del ADC
*****************************************************************************/
void Inicio_ADC (void) {
	// inicio el ADC en modo de conversión simple, con el preescaler en 128 y sin interrupción y referencia 2.56v interna.
	ADMUX = (SET<<REFS1)|(SET<<REFS0)|(OFF<<ADLAR);							// referencia de 2.56v
	SW_Mux_ADC(ADC_0);														// cambio el mux
	ADCSRA = (ON<<ADEN)|(ON<<ADSC)|(OFF<<ADFR)|(SET<<ADIF)|(OFF<<ADIE)|(SET<<ADPS2)|(SET<<ADPS1)|(SET<<ADPS0);

	//// ACA HAY QUE VER QUE NO VAN LOS VALORES POR DEFECTO O DEBEN SER SOBRESCRITO POR LOS DE LA EEPROM /////
	uMuestrasADCpwm1 = DEFAULT_MUESTRAS_ADC_PWM1;
	flPWM1aMA = DEFAULT_PWM1_A_MA;
	flPWM1valorMA = 0;

	uMuestrasADCpwm2 = DEFAULT_MUESTRAS_ADC_PWM2;
	flPWM2aMA = DEFAULT_PWM2_A_MA;
	flPWM2valorMA = 0;

	uMuestrasADCcond1 = DEFAULT_MUESTRAS_ADC_COND1;
	flCond1auS = DEFAULT_COND1_A_US;
	flOffsetCond1 = DEFAULT_OFFSET_COND1;
	flCond1valoruS = 0;

	uMuestrasADCcond2 = DEFAULT_MUESTRAS_ADC_COND2;
	flCond2auS = DEFAULT_COND2_A_US;
	flCond2valoruS = 0;

	uContMuestrasADC = 0;
	chADCprint = NO;

	chValorNuevoADC=NO;

	uchEstadoADC = MEDICION_ADC0_PWM_420mA_2;
}

