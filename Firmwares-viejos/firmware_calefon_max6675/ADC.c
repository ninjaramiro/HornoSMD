/** \file	temperatura.c
 *  \brief	Contiene las funciones de manejo de medición de temperatura con LM35DZ.
 *
 *	Contiene las funciones de manejo de medición de temperatura y medicion con el ADC \n
 */

#include "ADC.h"
#include "macros.h"
#include "inicio.h"

#include <avr/io.h>
#include <stdio.h>
#include <avr/wdt.h>

/************************************************************************
* Variables
************************************************************************/
char	chEstadoADC;							/**< \brief variable de estado de la maquina de medicion de ADC */
float 	flTensionBateria;						/**< \brief tension de bateria en V */
float 	flTempSetP;				/**< \brief temperatura seleccionada en °C */

/************************************************************************
* Maquina de estados de medicion de temperatura
************************************************************************/

void valor_ADC(void){
	switch (chEstadoADC) {

	case CAPTURA_ADC_6_TENSION_BATERIA_START_CONV:
		SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
		chEstadoADC = CAPTURA_ADC_6_TENSION_BATERIA;
		break;

	case CAPTURA_ADC_6_TENSION_BATERIA:
			if ((ADCSRA & (SET<<ADSC)) == 0) {
				// calculo de tension
				float flTensionCapturada = (float)ADC / ADC_6_A_TENSION;

				// media movil
				flTensionBateria = flTensionBateria + (flTensionCapturada - flTensionBateria) / CANTIDAD_ADC_PROMEDIO;

				// ejecuto la logica de carga
				if (flTensionBateria >= TENSION_FIN_CARGA){
					CLEAR_BIT(RELAY_CARGA_PORT, RELAY_CARGA_PIN);
				}
				else if (flTensionBateria <= TENSION_INICIO_CARGA){
					SET_BIT(RELAY_CARGA_PORT, RELAY_CARGA_PIN);
				}

				SET_BIT(ADMUX, MUX0);									// paso al ADC_7
				chEstadoADC = CAPTURA_ADC_7_SET_TEMP_START_CONV;
			}
			break;

	case CAPTURA_ADC_7_SET_TEMP_START_CONV:
		SET_BIT(ADCSRA, ADSC);														// comienzo una nueva conversion normal
		chEstadoADC = CAPTURA_ADC_7_SET_TEMP;
		break;

	case CAPTURA_ADC_7_SET_TEMP:
			if ((ADCSRA & (SET<<ADSC)) == 0) {
				// calculo de tension
				float flTemperaturaMedida = ((ADC_7_MAX_SET_TEMP-ADC_7_MIN_SET_TEMP)/1023.0) * (float)ADC + ADC_7_MIN_SET_TEMP;

				// media movil
				flTempSetP = flTempSetP + (flTemperaturaMedida - flTempSetP) / CANTIDAD_ADC_PROMEDIO;

				CLEAR_BIT(ADMUX, MUX0);									// paso al ADC_6
				chEstadoADC = CAPTURA_ADC_6_TENSION_BATERIA_START_CONV;
			}
			break;


			default:
				Inicio_valor_ADC();
				break;
	}
}

/************************************************************************
* Funcion de medición de temperatura
************************************************************************/

void Inicio_valor_ADC(void){
	ADMUX = (OFF<<REFS1)|(ON<<REFS0)|(OFF<<ADLAR)|(OFF<<MUX3)|(ON<<MUX2)|(ON<<MUX1)|(OFF<<MUX0);	// referencia AVcc
	ADCSRA = (ON<<ADEN)|(ON<<ADSC)|(OFF<<ADATE)|(OFF<<ADIF)|(OFF<<ADIE)|(ON<<ADPS2)|(ON<<ADPS1)|(ON<<ADPS0);

	flTensionBateria = 0;
	flTempSetP = 0;

	chEstadoADC = CAPTURA_ADC_6_TENSION_BATERIA_START_CONV;
}
