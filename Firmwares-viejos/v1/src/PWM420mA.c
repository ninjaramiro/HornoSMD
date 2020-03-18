/** \file	PWM420mA.c
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente al lazo de control PWM para las salidas 4-20mA
 *
 */

#include "PWM420mA.h"
#include "macros.h"
#include "medicion_ADC.h"
#include "USART0.h"

#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <math.h>

/*****************************************************************************
* Variables Máquina de estados
*****************************************************************************/
unsigned char			uchEstadoPWM420mA;					/**< \brief variable de la maquina de estados del PWM */
unsigned int			uGananciaPWM1;						/**< \brief ganacia del lazo de control de la salida PWM */
unsigned int			uGananciaPWM2;						/**< \brief ganacia del lazo de control de la salida PWM */
float 					flCorrienteObjetivo;				/**< \brief variable para el calculo de la corriente objetivo para la salida PWM */
int						deltaOCR;
float					flDeltaI;

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern float			flPWM1valorMA;			/* medicion_ADC.c, valor medio en mA medidos por el ADC */
extern float			flPWM2valorMA;			/* medicion_ADC.c, valor medio en mA medidos por el ADC */
extern volatile float	flCond1valoruS;			/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern volatile float	flCond2valoruS;			/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern char				chValorNuevoADC;		/* medicion_ADC.c, idica la obtencion de un valor nuevo medido */

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Actualizar_PWM_420mA (void) {
	switch (uchEstadoPWM420mA) {

		case PWM_STANDBY:
			if (chValorNuevoADC == SI) {
				uchEstadoPWM420mA = PWM_1_420MA_ACTUALIZAR;
			}
		break;

		case PWM_1_420MA_ACTUALIZAR:
			// salida 4-20mA Cond. 1 0-20uS/cm
			flCorrienteObjetivo = I_MAX_MA-(MAX_COND1_US-flCond1valoruS)*(I_MAX_MA-I_MIN_MA)/(MAX_COND1_US-MIN_COND1_US);
			flDeltaI = flCorrienteObjetivo-flPWM1valorMA;

			if (fabs(flDeltaI) > MAX_PWM_RES_MA){
				if (fabs(flDeltaI) < VALOR_PWM_SIN_GANANCIA_MA){
					if ((flCorrienteObjetivo > flPWM1valorMA) && (OCR1A < (CUENTAS_MAX_PWM-1))) {
						OCR1A++;
					}
					else if ((flCorrienteObjetivo < flPWM1valorMA) && (OCR1A > 0)) {
						OCR1A--;
					}
				}
				else {
					deltaOCR = (int)roundf((flDeltaI*CUENTAS_MAX_PWM*uGananciaPWM1)/(I_MAX_MA*MAXIMO_GANANCIA_LAZO_PWM));

					if (deltaOCR == 0){
						if ((flCorrienteObjetivo > flPWM1valorMA) && (OCR1A < (CUENTAS_MAX_PWM-1))) {
							OCR1A++;
						}
						else if ((flCorrienteObjetivo < flPWM1valorMA) && (OCR1A > 0)) {
							OCR1A--;
						}
					}
					else if (deltaOCR > 0){
						if ((OCR1A+deltaOCR) < (CUENTAS_MAX_PWM-1)) {
							OCR1A += deltaOCR;
						}
						else if (OCR1A < (CUENTAS_MAX_PWM-1)){
							OCR1A++;
						}
					}
					else if (deltaOCR < 0){
						if ((OCR1A+deltaOCR) > 0) {
							OCR1A += deltaOCR;
						}
						else if (OCR1A > 0){
							OCR1A--;
						}
					}
				}
			}

			uchEstadoPWM420mA = PWM_2_420MA_ACTUALIZAR;
			break;

		case PWM_2_420MA_ACTUALIZAR:
			// salida 4-20mA Cond. 2 20-2500uS/cm
			flCorrienteObjetivo = I_MAX_MA-(MAX_COND2_US-flCond2valoruS)*(I_MAX_MA-I_MIN_MA)/(MAX_COND2_US-MIN_COND2_US);
			flDeltaI = flCorrienteObjetivo-flPWM2valorMA;

			if (fabs(flDeltaI) > MAX_PWM_RES_MA){
				if (fabs(flDeltaI) < VALOR_PWM_SIN_GANANCIA_MA){
					if ((flCorrienteObjetivo > flPWM2valorMA) && (OCR1B < (CUENTAS_MAX_PWM-1))) {
						OCR1B++;
					}
					else if ((flCorrienteObjetivo < flPWM2valorMA) && (OCR1B > 0)) {
						OCR1B--;
					}
				}
				else {
					deltaOCR = (int)roundf((flDeltaI*CUENTAS_MAX_PWM*uGananciaPWM2)/(I_MAX_MA*MAXIMO_GANANCIA_LAZO_PWM));

					if (deltaOCR == 0){
						if ((flCorrienteObjetivo > flPWM2valorMA) && (OCR1B < (CUENTAS_MAX_PWM-1))) {
							OCR1B++;
						}
						else if ((flCorrienteObjetivo < flPWM2valorMA) && (OCR1B > 0)) {
							OCR1B--;
						}
					}
					else if (deltaOCR > 0){
						if ((OCR1B+deltaOCR) < (CUENTAS_MAX_PWM-1)) {
							OCR1B += deltaOCR;
						}
						else if (OCR1B < (CUENTAS_MAX_PWM-1)){
							OCR1B++;
						}
					}
					else if (deltaOCR < 0){
						if ((OCR1B+deltaOCR) > 0) {
							OCR1B += deltaOCR;
						}
						else if (OCR1B > 0){
							OCR1B--;
						}
					}
				}
			}

			chValorNuevoADC=NO;
			uchEstadoPWM420mA = PWM_STANDBY;
			break;

		default:
			Inicio_PWM_420mA();
			break;
	}
}


/*****************************************************************************
* Funcion de inicio
*****************************************************************************/
void Inicio_PWM_420mA (void) {
	// Inicio en Fast PWM-10bits, preescaler 1, para la salida de 4-20mA
	TCCR1A = (ON<<COM1A1)|(OFF<<COM1A0)|(ON<<COM1B1)|(OFF<<COM1B0)|(OFF<<COM1C1)|(OFF<<COM1C0)|(ON<<WGM11)|(ON<<WGM10);
	TCCR1B = (OFF<<ICNC1)|(OFF<<ICES1)|(OFF<<WGM13)|(ON<<WGM12)|(OFF<<CS12)|(OFF<<CS11)|(ON<<CS10);
	OCR1A = 0;
	OCR1B = 0;

//	uGananciaPWM1 = DEFAULT_GANACIA_LAZO_PWM_1;
//	uGananciaPWM2 = DEFAULT_GANACIA_LAZO_PWM_2;

	uchEstadoPWM420mA = PWM_1_420MA_ACTUALIZAR;
}
