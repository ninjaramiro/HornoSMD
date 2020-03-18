/** \file	alarmas.c
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente al control de alarmas.
 *
 */

#include "alarmas.h"
#include "macros.h"
#include "medicion_ADC.h"
#include "inicioUc.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoAlarmas;		/**< \brief variable de la maquina de estados de alarmas */
volatile float			flValorAlarmaCond1;		/**< \brief valor para la activación de la alarma del conductimetro 1 */
volatile unsigned int	uTiempoAlarmaCond1;		/**< \brief tiempo de permanencia por encima del valor para la activacion de la alarma */
volatile unsigned int	uContTiempoAlarmaCond1;	/**< \brief contador de tiempo de permanencia por encima del valor para la activacion de la alarma */
volatile float			flValorAlarmaCond2;		/**< \brief valor para la activación de la alarma del conductimetro 2 */
volatile unsigned int	uTiempoAlarmaCond2;		/**< \brief tiempo de permanencia por encima del valor para la activacion de la alarma */
volatile unsigned int	uContTiempoAlarmaCond2;	/**< \brief contador de tiempo de permanencia por encima del valor para la activacion de la alarma */
int						OnOffAlarmas;			/**< \brief cada bit corresponde al estado de activacion de una alarma */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern volatile char			chBuzzerModo;	/* alarmas.c, modo de operacion del buzzer */

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Actualizar_Alarmas (void) {
	switch (uchEstadoAlarmas) {

		case ALARMA_CONDUCTIMETRO_1:
			if (flValorAlarmaCond1 != 0){
				if (uContTiempoAlarmaCond1 == uTiempoAlarmaCond1) {	// si integro el tiempo es que se activo la alarma
					SET_BIT(OnOffAlarmas, ALARMA_1);
				}
				else if (uContTiempoAlarmaCond1 == 0){
					CLEAR_BIT(OnOffAlarmas, ALARMA_1);
				}
			}
			else {
				uContTiempoAlarmaCond1=0;
				CLEAR_BIT(OnOffAlarmas, ALARMA_1);
			}

			uchEstadoAlarmas = ALARMA_CONDUCTIMETRO_2;
			break;

		case ALARMA_CONDUCTIMETRO_2:
			if (flValorAlarmaCond2 != 0){
				if (uContTiempoAlarmaCond2 == uTiempoAlarmaCond2) {	// si integro el tiempo es que se activo la alarma
					SET_BIT(OnOffAlarmas, ALARMA_2);
				}
				else if (uContTiempoAlarmaCond2 == 0){
					CLEAR_BIT(OnOffAlarmas, ALARMA_2);
				}
			}
			else {
				uContTiempoAlarmaCond2=0;
				CLEAR_BIT(OnOffAlarmas, ALARMA_2);
			}

			uchEstadoAlarmas = ALARMA_CONTROL;
			break;

		case ALARMA_CONTROL:
			if (OnOffAlarmas != 0){
				((OnOffAlarmas&(SET<<ALARMA_1)) != 0) ? (SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1)) : (CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1));
				((OnOffAlarmas&(SET<<ALARMA_2)) != 0) ? (SET_BIT(SALIDA_DIG_PORT, SALIDA_DIG_2)) : (CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_2));

				chBuzzerModo = BUZZER_INTERMITENTE;
			}
			else {
				CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
				CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_2);

				chBuzzerModo = BUZZER_OFF;
			}

			uchEstadoAlarmas = ALARMA_CONDUCTIMETRO_1;
			break;

		default:
			uchEstadoAlarmas = ALARMA_CONDUCTIMETRO_1;
			break;
	}
}

/*****************************************************************************
* Inicio de Maquina de estados
*****************************************************************************/
void Inicio_Alarmas (void) {
	flValorAlarmaCond1 = DEFAULT_VALOR_ALARMA_COND_1;
	uTiempoAlarmaCond1 = DEFAULT_TIEMPO_ALARMA_COND_1;
	uContTiempoAlarmaCond1 = 0;

	flValorAlarmaCond2 = DEFAULT_VALOR_ALARMA_COND_2;
	uTiempoAlarmaCond2 = DEFAULT_TIEMPO_ALARMA_COND_2;
	uContTiempoAlarmaCond1 = 0;

	OnOffAlarmas = 0;

	CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
	CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_2);

	uchEstadoAlarmas = ALARMA_CONDUCTIMETRO_1;
}
