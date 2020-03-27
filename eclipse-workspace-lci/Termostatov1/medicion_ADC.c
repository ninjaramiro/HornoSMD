/** \file	medicion_ADC.c
 * 	\author Abel N. Dammiani - modificado por Ramiro Alonso
 *
 *  \brief Contiene la maquina de estados correspondiente a la medicion de las entradas del ADC.
 */

#include "inicioUc.h"
#include "macros.h"
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
//unsigned int			uContMuestrasADC;		/**< \brief contador de promedio de muestras */

unsigned int			uMuestrasADC1;		/**< \brief cantidad de muestras para promediar */
unsigned int			uMuestrasADC2;		/**< \brief cantidad de muestras para promediar */
unsigned int			uMuestrasADC3;		/**< \brief cantidad de muestras para promediar */


volatile float ADC1;
volatile float ADC2;
volatile float ADC3;


/*****************************************************************************
* Variables externas
*****************************************************************************/
//extern char					chBufferTx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de transmisión USART del uc */

/*****************************************************************************
* Maquina de estados del ADC
*****************************************************************************/
void Medicion_ADC(void) {

	switch (uchEstadoADC) {

	case INICIO_MEDICION_ADC_1:
		SW_Mux_ADC(ADC_1); // ACTIVO EL CANAL 1
		SET_BIT(ADCSRA, ADSC);
		uchEstadoADC = MEDICION_ADC1;
	//	ADC1 = 1.38;
		break;

	case MEDICION_ADC1:
			//ADC1 = 1.98;
			if ((ADCSRA & (SET<<ADSC)) == 0) {							// si termino la conversio espero
				ADC1 = (float)ADC; 										// cargo el valor medido
				//	chValorNuevoADC = SI;
				uchEstadoADC = INICIO_MEDICION_ADC_2;
			}
			break;



	case INICIO_MEDICION_ADC_2:
		SW_Mux_ADC(ADC_2); // ACTIVO EL CANAL 2
		SET_BIT(ADCSRA, ADSC);
		uchEstadoADC = MEDICION_ADC2;
		break;


	case MEDICION_ADC2:
			if ((ADCSRA & (SET<<ADSC)) == 0) {							// si termino la conversio espero
				ADC2 = (float)ADC; 										// cargo el valor medido
				//	chValorNuevoADC = SI;

					uchEstadoADC = INICIO_MEDICION_ADC_3;
			}
			break;



	case INICIO_MEDICION_ADC_3:
		SW_Mux_ADC(ADC_3); // ACTIVO EL CANAL 3
		SET_BIT(ADCSRA, ADSC);
		uchEstadoADC = MEDICION_ADC3;
		break;

	case MEDICION_ADC3:
			if ((ADCSRA & (SET<<ADSC)) == 0) {							// si termino la conversio espero
				ADC3 = (float)ADC; 										// cargo el valor medido
				//	chValorNuevoADC = SI;

					uchEstadoADC = INICIO_MEDICION_ADC_1;
			}
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
	ADCSRA = (ON<<ADEN)|(ON<<ADSC)|(OFF<<ADATE)|(SET<<ADIF)|(OFF<<ADIE)|(SET<<ADPS2)|(SET<<ADPS1)|(SET<<ADPS0);

	ADC1 = 0;
	ADC2 = 0;
	ADC3 = 0;


//	uContMuestrasADC = 0;
	chADCprint = NO;
	uchEstadoADC = INICIO_MEDICION_ADC_1;
}



