/** \file	caudales.c
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente al la medicion de caudales.
 *
 */

#include "caudales.h"
#include "macros.h"
#include "inicio.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoCaudales;			/**< \brief variable de la maquina de estados de caudales */
volatile unsigned char	uchEtapaMedicionCaudal;		/**< \brief variable que indica la etapa de medicion en la que se encuentra la captura */
volatile unsigned int	uContStandbyCaudal;			/**< \brief contador de espera entre mediciones de caudal para dar tiempo al recorrido de las maquinas de estado */

volatile unsigned int	uPrimerCaptura;				/**< \brief valores de captura para la medicion de caudal */
volatile unsigned int	uSegundaCaptura;			/**< \brief valores de captura para la medicion de caudal */

float 					flCaudalMedido;				/**< \brief valor medio de caudal en l/min */

/*****************************************************************************
* Variables externas
*****************************************************************************/

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Medicion_Caudales (void) {
	switch (uchEstadoCaudales) {

		case MEDICION_CAUDAL_STANDBY:
				if (uContStandbyCaudal == 0){	// si hay voy a medir
					uchEstadoCaudales = MEDICION_CAUDAL;
				}
			break;

		case MEDICION_CAUDAL:
			if (uchEtapaMedicionCaudal == ETAPA_MEDICION_STANDBY){
				Iniciar_Medicion_Caudal();
			}
			else if (uchEtapaMedicionCaudal == ETAPA_MEDICION_CALCULO){
				float flCuentas = uSegundaCaptura-uPrimerCaptura;
				float flCaudal = 15000000/(flCuentas*PULSOS_X_LITRO_CAUDAL);	// en l/min
				int muestrasPromedio = 65535 / flCuentas;						//32767.5@1L 1000@30L
				if (muestrasPromedio<1){
					muestrasPromedio=1;
				}
				else if (muestrasPromedio>MAXIMO_MUESTRAS_CAUDAL){
					muestrasPromedio=MAXIMO_MUESTRAS_CAUDAL;
				}
				flCaudalMedido = flCaudalMedido + (flCaudal - flCaudalMedido) / muestrasPromedio;	// hago una media movil											// cargo el valor medido

				uContStandbyCaudal = TIEMPO_MEDICION_CAUDAL_STANDBY;
				uchEtapaMedicionCaudal = ETAPA_MEDICION_STANDBY;
				uchEstadoCaudales = MEDICION_CAUDAL_STANDBY;
			}
			else if (uchEtapaMedicionCaudal == ETAPA_MEDICION_OVERFLOW){
				flCaudalMedido = 0;//lValorCaudal - flValorCaudal / MUESTRAS_CAUDAL;	// hago una media movil											// cargo el valor medido

				uContStandbyCaudal = TIEMPO_MEDICION_CAUDAL_STANDBY;
				uchEtapaMedicionCaudal = ETAPA_MEDICION_STANDBY;
				uchEstadoCaudales = MEDICION_CAUDAL_STANDBY;
			}
			break;

		default:
			Configuracion_Timer_Medicion_Caudales();
			break;
	}
}

/*****************************************************************************
* Inicio de Maquina de estados
*****************************************************************************/
void Configuracion_Timer_Medicion_Caudales (void) {

	TCCR1A = (OFF<<COM1A1)|(OFF<<COM1A0)|(OFF<<COM1B1)|(OFF<<COM1B0)|(OFF<<WGM11)|(OFF<<WGM10);				/* el timer opera en modo normal */
	TCCR1B = (ON<<ICNC1)|(ON<<ICES1)|(OFF<<WGM13)|(OFF<<WGM12)|(OFF<<CS12)|(OFF<<CS11)|(OFF<<CS10);			/* con filtro activado, flanco positivo y sin prescaler */
	TCCR1C = (OFF<<FOC1A)|(OFF<<FOC1B);	/* aseguro que no hay ninguna salida por comparación activada mas alla del modo de operacion */
	CLEAR_BIT (TIMSK1, OCIE1A);			/* deshabilito la interrupción del comparador A */
	CLEAR_BIT (TIMSK1, OCIE1B);			/* deshabilito la interrupción del comparador B */
	CLEAR_BIT (TIMSK1, TOIE1);			/* deshabilito la interrupción por overflow */
	CLEAR_BIT (TIMSK1, ICIE1);			/* deshabilito la interrupción por captura */

	SET_BIT (TIFR1, ICF1);				/* reaseguro que no haya ninguna interrupción activa */
	SET_BIT (TIFR1, OCF1A);				/* reaseguro que no haya ninguna interrupción activa */
	SET_BIT (TIFR1, OCF1B);				/* reaseguro que no haya ninguna interrupción activa */
	SET_BIT (TIFR1, TOV1);				/* reaseguro que no haya ninguna interrupción activa */

	flCaudalMedido=0.0;
	uContStandbyCaudal = TIEMPO_MEDICION_CAUDAL_STANDBY;
	uchEtapaMedicionCaudal = ETAPA_MEDICION_STANDBY;
	uchEstadoCaudales = MEDICION_CAUDAL_STANDBY;
}

/************************************************************************
* Inicio medicion
************************************************************************/
void Iniciar_Medicion_Caudal (void) {
	uchEtapaMedicionCaudal = ETAPA_MEDICION_CAPTURA_1;
	TCNT1 = 0;							/* pongo en cero el timer */

	SET_BIT (TIFR1, ICF1);				/* reaseguro que no haya ninguna interrupción activa */
	SET_BIT (TIFR1, OCF1A);				/* reaseguro que no haya ninguna interrupción activa */
	SET_BIT (TIFR1, OCF1B);				/* reaseguro que no haya ninguna interrupción activa */
	SET_BIT (TIFR1, TOV1);				/* reaseguro que no haya ninguna interrupción activa */

	TCCR1B |= (ON<<CS11)|(ON<<CS10);	/* pongo a correr el timer con preescaler en 64 */

	SET_BIT (TIMSK1, ICIE1);			/* habilito la interrupción por captura */
	SET_BIT (TIMSK1, TOIE1);			/* habilito la interrupción por overflow */
}

