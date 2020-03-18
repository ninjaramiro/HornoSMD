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
unsigned char			uchEstadoAlarmas;				/**< \brief variable de la maquina de estados de alarmas */
float					flValorAlarmaCond1;				/**< \brief valor para la activación de la alarma del conductimetro 1 */
unsigned int			uTiempoAlarmaCond1;				/**< \brief tiempo de permanencia por encima del valor para la activacion de la alarma */
volatile unsigned int	uContTiempoAlarmaCond1;			/**< \brief contador de tiempo de permanencia por encima del valor para la activacion de la alarma */
float					flValorAlarmaCond2;				/**< \brief valor para la activación de la alarma del conductimetro 2 */
unsigned int			uTiempoAlarmaCond2;				/**< \brief tiempo de permanencia por encima del valor para la activacion de la alarma */
volatile unsigned int	uContTiempoAlarmaCond2;			/**< \brief contador de tiempo de permanencia por encima del valor para la activacion de la alarma */
unsigned int			uTiempoSilenciarAlarma;			/**< \brief tiempo en minutos por el cual se pospone el sonnido de la alarma */
volatile unsigned long	ulContTiempoSilenciarAlarma;	/**< \brief contador para posponer el sonido de la alarma por un determinado tiempo */
int						OnOffAlarmas;					/**< \brief cada bit corresponde al estado de activacion de una alarma */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern volatile char	chBuzzerModo;					/* alarmas.c, modo de operacion del buzzer */

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Actualizar_Alarmas (void) {
	switch (uchEstadoAlarmas) {

		case ALARMA_CONDUCTIMETRO_1:
			if (flValorAlarmaCond1 != 0){
				if (uContTiempoAlarmaCond1 == uTiempoAlarmaCond1 * TIMER0_A_SEGUNDOS) {	// si integro el tiempo es que se activo la alarma
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
				if (uContTiempoAlarmaCond2 == uTiempoAlarmaCond2 * TIMER0_A_SEGUNDOS) {	// si integro el tiempo es que se activo la alarma
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
			Inicio_Alarmas();
			break;
	}
}

/*****************************************************************************
* Inicio de Maquina de estados
*****************************************************************************/
void Inicio_Alarmas (void) {
	uContTiempoAlarmaCond1 = 0;
	uContTiempoAlarmaCond2 = 0;
	ulContTiempoSilenciarAlarma = 0;
	OnOffAlarmas = 0;

	CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1);
	CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_2);

	uchEstadoAlarmas = ALARMA_CONDUCTIMETRO_1;
}
