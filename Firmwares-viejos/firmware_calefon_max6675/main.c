
/** \file	main.c
 *  \brief	Archivo que llama el inicio de todaslas maquinas de estado y cierra un bucle \n
 *	infinito lamando a cada una.
 */

#include "macros.h"
#include "inicio.h"
#include "lcd.h"
#include "ADC.h"
#include "caudales.h"
#include "ds18b20.h"
#include "max6675.h"
#include "USART0.h"
#include "motorPP.h"
#include "terminal.h"
#include "control.h"
#include "eeprom.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************
* Definiciones
*****************************************************************************/
//#define VALOR_INTEGRACION_DETECTOR_LLAMA	100

/*****************************************************************************
* Variables
*****************************************************************************/
char 					chBuffer[30];
volatile unsigned int	uContActualizarDisplay;	/**< \brief variable que indica si hay que actualizar el display porque se apreto un boton */
volatile char			chLlamaDetectada;
volatile unsigned int	uContLlamaDetectada;

/*****************************************************************************
* externas
*****************************************************************************/
extern volatile unsigned int	uContTimeoutEsperaUSART0;			/**< \brief contador de timeout para espera del USART */
extern volatile unsigned int	uContTimeoutLecturaRxUSART0;		/**< \brief contador de timeout para espera de lectura del buffer de Rx del USART */
extern volatile unsigned int	uContTimeoutRxUSART0;				/**< \brief contador de timeout para espera de Rx del USART, si se vence cierro la recepcion */
//extern volatile unsigned int	uContTimeoutTxUSART0;				/**< \brief contador de timeout para espera de transmision del buffer de Tx del USART */

extern volatile unsigned int	uContStandbyCaudal;					/**< \brief contador de espera entre mediciones de caudal para dar tiempo al recorrido de las maquinas de estado */
extern volatile unsigned char	uchEtapaMedicionCaudal;				/**< \brief variable que indica la etapa de medicion en la que se encuentra la captura */
extern volatile unsigned int	uPrimerCaptura;						/**< \brief valores de captura para la medicion de caudal */
extern volatile unsigned int	uSegundaCaptura;					/**< \brief valores de captura para la medicion de caudal */

extern volatile unsigned int	uContMotorPP;

extern volatile unsigned int 	uContPeriodoPID;
extern volatile unsigned int 	uContPeriodoVedaPID;				/**< \brief periodo de veda del PID */
extern volatile unsigned int 	uContPeriodoVedaCaudal;				/**< \brief periodo de veda de deteccion de variacion de caudal */

extern float 					flCaudalMedido;						/**< \brief valor medio de caudal en m3/h*/
extern float					flTempMedida;
extern float 					flTensionBateria;					/**< \brief tension de bateria en V */
extern float 					flTempSetP;			/**< \brief temperatura seleccionada en °C */

#ifdef COMPILE_WITH_MAX6675
extern volatile unsigned int	uContMax6675;
#endif
/*****************************************************************************
* Main
*****************************************************************************/
/** \brief	Función de entrada del programa
 *
 *	Se inicializan todas las máquinas de estado, y se entra en un bucle infinito \n
 *	donde se ejecutan las mismas. \n
 *	Además se inician diferentes perisféricos del uc que no se encuentran en \n
 *	diferentes máquinas de estado.
 */

int main (void) {
	wdt_disable();				/* por las dudas deshabilito el watchdog timer para el inicio de todo el programa */
	cli();						/* Deshabilito las interrupciones globales */

	Inicio_Puertos();			/* inicio los puertos */
	Inicio_Timer2();
	Inicio_valor_ADC();
#ifdef COMPILE_WITH_MAX6675
	Spi_Max6675_Inicio();
#endif
	Configuracion_Timer_Medicion_Caudales();
	Inicio_USART_0();
	Inicio_Control_Temp();
	Carga_Configuracion_Desde_Eeprom();
	Inicio_Terminal();

	LCDInit(LS_NONE);
	LCDClear();
	sprintf(chBuffer, "-Pedro-Juan-Ana-");
	LCDWriteLine(0,chBuffer);
	sprintf(chBuffer, "Iniciando...");
	LCDWriteLine(1,chBuffer);

	Inicio_Motor_PP();

	// inicio de variables
	uContActualizarDisplay = 0;
	chLlamaDetectada = NO;
	uContLlamaDetectada = 0;

	wdt_enable(WDTO_1S);		/* prendo el watchdog timer */
	sei();						/* habilito las interrupciones globales */

	while (1) {
		// ejecuto máquinas de estado
		valor_ADC();
#ifdef COMPILE_WITH_DS18B20
		ds18b20_gettemp();
#endif
#ifdef COMPILE_WITH_MAX6675
		Max6675_Get_Temp();
#endif
		Medicion_Caudales();
		Recepcion_USART_0();
		Rotar_Motor_PP();
		Terminal();
		Control_Temp();

		// actualizo en display
		if (uContActualizarDisplay == 0){
			//	1234567890123456
			//	Tmp: 32.1/41.3°C
			//  Tmp: 32.1°C 41°C
			//	30.2l/m  [12.2V]

			// primer linea
			int i = sprintf(chBuffer, "Tmp: ");
			if (flTempMedida < 0.0){
				sprintf(chBuffer+i, "-UL-");
			}
			else if (flTempMedida < 10.0){
				dtostrf(flTempMedida, 4, 2, chBuffer+i);
			}
			else if (flTempMedida < 100.0){
				dtostrf(flTempMedida, 4, 1, chBuffer+i);
			}
			else {
				sprintf(chBuffer+i, "-OL-");
			}
			i += 4;
			if (flTempSetP < 0.0){
				sprintf(chBuffer+i, "%%0C UL%%0C");
			}
			else if (flTempSetP < 100.0){
				i += sprintf(chBuffer+i, "%%0C ");
				dtostrf(flTempSetP, 2, 0, chBuffer+i);
				i += 2;
				sprintf(chBuffer+i, "%%0C");
			}
			else {
				sprintf(chBuffer+i, "%%0C OL%%0C");
			}
			LCDWriteLine(0,chBuffer);

			// segunda linea
			if (flCaudalMedido < 0.0){
				sprintf(chBuffer, "-UL-");
			}
			else if (flCaudalMedido < 10.0){
				dtostrf(flCaudalMedido, 4, 2, chBuffer);
			}
			else if (flCaudalMedido < 100.0){
				dtostrf(flCaudalMedido, 4, 1, chBuffer);
			}
			else {
				sprintf(chBuffer, "-OL-");
			}
			i = 4;
			if (RELAY_CARGA_PIN_PORT & (SET<<RELAY_CARGA_PIN)){	// activa
				i += sprintf(chBuffer+i, "l/m %%3");
			}
			else{
				i += sprintf(chBuffer+i, "l/m  ");
			}
			i += sprintf(chBuffer+i, "%%1");
			if (flTensionBateria < 0.0){
				sprintf(chBuffer+i, "-UL-");
			}
			else if (flTensionBateria < 10.0){
				dtostrf(flTensionBateria, 4, 2, chBuffer+i);
			}
			else if (flTensionBateria < 100.0){
				dtostrf(flTensionBateria, 4, 1, chBuffer+i);
			}
			else {
				sprintf(chBuffer+i, "-OL-");
			}
			i += 4;
			sprintf(chBuffer+i, "V%%2");
			LCDWriteLine(1,chBuffer);

			uContActualizarDisplay = 100;
		}

		// reseteo el WDT
		wdt_reset();
	}
}

/*****************************************************************************
* Interrupcion Timer 2
*****************************************************************************/
/** \brief	Rutina de interrupción del Timer 2.
 *
 *	Este timer genera una interrupción, en el se incrementanlos \n
 *	contadores de cada máquina de estados.
 */

ISR (TIMER2_COMPA_vect) {
	/*------------------------------------------------------------------------------------*
	 * control
	 *------------------------------------------------------------------------------------*/
	if (uContPeriodoPID != 0){
		uContPeriodoPID--;
	}

	if (uContPeriodoVedaPID != 0){
		uContPeriodoVedaPID--;
	}

	if (uContPeriodoVedaCaudal != 0){
		uContPeriodoVedaCaudal--;
	}

	/*------------------------------------------------------------------------------------*
	 * motor PP
	 *------------------------------------------------------------------------------------*/
	if (uContMotorPP != 0){
		uContMotorPP--;
	}

//	/*------------------------------------------------------------------------------------*
//	 * detector de llama
//	 *------------------------------------------------------------------------------------*/
//	if (LLAMA_PIN_PORT & (SET<<LLAMA_PIN)){	// activa
//		if (uContLlamaDetectada < VALOR_INTEGRACION_DETECTOR_LLAMA){
//			uContLlamaDetectada ++;
//			if (uContLlamaDetectada == VALOR_INTEGRACION_DETECTOR_LLAMA){
//				chLlamaDetectada = SI;
//			}
//		}
//	}
//	else if (uContLlamaDetectada > 0){
//		uContLlamaDetectada --;
//		if (uContLlamaDetectada == 0){
//			chLlamaDetectada = NO;
//		}
//	}

	/*------------------------------------------------------------------------------------*
	 * max6675
	 *------------------------------------------------------------------------------------*/
#ifdef COMPILE_WITH_MAX6675
	if (uContMax6675 != 0)
		uContMax6675--;
#endif

	/*------------------------------------------------------------------------------------*
	 * display
	 *------------------------------------------------------------------------------------*/
	if (uContActualizarDisplay != 0)
		uContActualizarDisplay--;

	/*------------------------------------------------------------------------------------*
	 * Contadores de operacion de caudales
	 *------------------------------------------------------------------------------------*/
	if (uContStandbyCaudal != 0)
		uContStandbyCaudal--;

	/*------------------------------------------------------------------------------------*
	 * Contadores de las máquinas de estado de USART
	 *------------------------------------------------------------------------------------*/
	/* decremento del contador de tiempo para time out de espera de USART0 */
	if (uContTimeoutEsperaUSART0 != 0)
		uContTimeoutEsperaUSART0 --;

	/* decremento del contador de tiempo para time out de electura del buffer de recepcion */
	if (uContTimeoutLecturaRxUSART0 != 0)
		uContTimeoutLecturaRxUSART0 --;

	/* decremento del contador de tiempo para timeout de recepcion para cerrar el buffer */
	if (uContTimeoutRxUSART0 != 0)
		uContTimeoutRxUSART0 --;

//	/* decremento del contador de tiempo para time out de envio del buffer de transmision */
//	if (uContTimeoutTxUSART0 != 0)
//		uContTimeoutTxUSART0 --;
}

/*****************************************************************************
* Interrupcion timer 1 captura (para medición de periodo)
*****************************************************************************/
/** \brief	Interrupción generada por la captura de un evento en el timer 1.
 */

ISR (TIMER1_CAPT_vect) {
	if (uchEtapaMedicionCaudal == ETAPA_MEDICION_CAPTURA_1){
		uPrimerCaptura = ICR1;
		uchEtapaMedicionCaudal = ETAPA_MEDICION_CAPTURA_2;
	}
	else if (uchEtapaMedicionCaudal == ETAPA_MEDICION_CAPTURA_2){
		TCCR1B &= ~((ON<<CS11)|(ON<<CS10));	/* paro el timer */
		CLEAR_BIT (TIMSK1, ICIE1);			/* deshabilito la interrupción por captura */
		CLEAR_BIT (TIMSK1, TOIE1);			/* deshabilito la interrupción por overflow */
		uSegundaCaptura = ICR1;
		uchEtapaMedicionCaudal = ETAPA_MEDICION_CALCULO;
	}
	else {
		TCCR1B &= ~((ON<<CS11)|(ON<<CS10));	/* paro el timer */
		CLEAR_BIT (TIMSK1, ICIE1);			/* deshabilito la interrupción por captura */
		CLEAR_BIT (TIMSK1, TOIE1);			/* deshabilito la interrupción por overflow */
		uchEtapaMedicionCaudal = ETAPA_MEDICION_STANDBY;
	}
}

/*****************************************************************************
* Interrupcion timer 1 overflow (para medición de periodo)
*****************************************************************************/
/** \brief	Interrupción externa proveniente del overflow del timer 1 al medir eventos.
 */

ISR (TIMER1_OVF_vect) {
	TCCR1B &= ~((ON<<CS11)|(ON<<CS10));	/* paro el timer */
	CLEAR_BIT (TIMSK1, ICIE1);			/* deshabilito la interrupción por captura */
	CLEAR_BIT (TIMSK1, TOIE1);			/* deshabilito la interrupción por overflow */
	uchEtapaMedicionCaudal = ETAPA_MEDICION_OVERFLOW;
}

/*****************************************************************************
* Interrupcion por defecto
*****************************************************************************/
/** \brief	Vector de interrupción por defecto en caso de que no este la interrupción activada.
 *
 *	En caso de alguna falla en el codigo y se activa una interrupción no deseada, o sea \n
 *	que no tiene su vector con su rutina asociada, cae en este vector por defecto que no \n
 *	hace nada.
 */

ISR (__vector_default) {
}
