
/** \file	main.c 
 *  \brief	Archivo que llama el inicio de todaslas maquinas de estado y cierra un bucle \n
 *	infinito lamando a cada una.
 */

#include "inicioUc.h"
#include "macros.h"
#include "USART0.h"
#include "USART1.h"
#include "eeprom.h"
#include "medicion_ADC.h"
#include "discretas.h"
#include "display.h"
#include "PWM420mA.h"
#include "alarmas.h"
#include "terminal.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern volatile unsigned int		uContTimeoutEsperaUSART0;						/* USART0.c, contador de timeout para espera del USART0 */
extern volatile unsigned int		uContTimeoutLecturaRxUSART0;					/* USART0.c, contador de timeout para espera de lectura del buffer de Rx del USART */
extern volatile unsigned int		uContTimeoutRxUSART0;							/* USART0.c, contador de timeout para espera de Rx del USART, si se vence cierro la recepcion */
//extern volatile unsigned int		uContTimeoutTxUSART0;							/* USART0.c, contador de timeout para espera de transmision del buffer de Tx del USART */
extern volatile unsigned int		uContTimeoutLecturaRxUSART1;					/* USART1.c, contador de timeout para espera de lectura del buffer de Rx del USART */
extern volatile unsigned int		uContTimeoutRxUSART1;							/* USART1.c, contador de timeout para espera de Rx del USART, si se vence cierro la recepcion */
extern volatile unsigned int		uContTimeoutTxUSART1;							/* USART1.c, contador de timeout para espera de transmision del buffer de Tx del USART */
extern volatile unsigned int		uEstadoEntradasInstantaneas;					/* discretas.c, en cada bit guarda el estado instantaneo de cada entrada */
extern volatile unsigned int 		uValorIntegracionEntradas[CANTIDAD_ENTRADAS];	/* discretas.c, guarda el tiempo de integracion de cada entrada para detectar activacion */
extern volatile unsigned int		uTimeoutEntradas;								/* discretas.c, variable para el timeout de flag no limpiados de entradas detectadas */
extern volatile unsigned int		uTiempoIntegracionEntradas;						/* discretas.c, tiempo durante el cual se integra el valor de las entradas */
extern volatile unsigned int		uEstadoEntradas;								/* discretas.c, estado de cada entrada luego de la integración */
extern volatile unsigned int		uFlagEntradasActivadas;							/* discretas.c, cada bit corresponde con la activacion de una entrada, el flag debe borrarse en la lectura */
extern volatile unsigned int		uFlagEntradasLiberadas;							/* discretas.c, cada bit corresponde con la liberacion de una entrada, el flag debe borrarse en la lectura */
extern volatile float				flCond1valoruS;									/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern volatile float				flCond2valoruS;									/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern float						flValorAlarmaCond1;								/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
extern unsigned int					uTiempoAlarmaCond1;								/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern volatile unsigned int		uContTiempoAlarmaCond1;							/* alarmas.c, contador de tiempo de permanencia por encima del valor para la activacion de la alarma */
extern float						flValorAlarmaCond2;								/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
extern unsigned int					uTiempoAlarmaCond2;								/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern volatile unsigned int		uContTiempoAlarmaCond2;							/* alarmas.c, contador de tiempo de permanencia por encima del valor para la activacion de la alarma */
extern volatile char				chBuzzerModo;									/* inicioUc.c, modo de operacion del buzzer */
extern volatile unsigned int		uContBuzzerON;									/* inicioUc.c, contador de tiempo encendido del buzzer */
extern volatile unsigned int		uContBuzzerOFF;									/* inicioUc.c, contador de tiempo apagado del buzzer */
extern volatile unsigned int		uContDisplay;									/* display.c, contador para la medicion de tiempos en el manejo del display */
extern volatile unsigned int		uContDisplayOFF;								/* display.c, contador para hacer parpadear el display */
extern volatile unsigned int		uContDisplayON;									/* display.c, contador para hacer parpadear el display */
extern volatile unsigned long		ulContTiempoSilenciarAlarma;					/* alarmas.c, contador para posponer el sonido de la alarma por un determinado tiempo */

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
	wdt_disable();					/* por las dudas deshabilito el watchdog timer para el inicio de todo el programa */
	cli();							/* Deshabilito las interrupciones globales */

	Inicio_Puertos ();
	Inicio_Entradas();
	Inicio_Timer0 ();				/* en modo CTC para que genere una interupción cada 1ms */
	Inicio_Timer2();
	Inicio_USART_0 ();
	Inicio_USART_1 ();
	Inicio_ADC();
	Inicio_PWM_420mA();
	Inicio_Alarmas();
	Carga_Configuracion_Desde_Eeprom();
	Cargar_Valores_Rango_Por_Canal();
	Inicio_Display();
	Inicio_Buzzer();

	sei();							/* habilito las interrupciones globales */

	Pantalla_Presentacion_Display();
	Inicio_Terminal();

	wdt_enable(WDTO_2S);			/* prendo el watchdog timer */

	while (1) {						/* ejecuto máquinas de estado */
		Recepcion_USART_0();
//		Transmision_USART_0();
		Recepcion_USART_1();
		Transmision_USART_1();
		Leer_Entradas();
		Medicion_ADC();
		Actualizar_PWM_420mA();
		Actualizar_Display();
		Actualizar_Alarmas();
		Terminal();

		wdt_reset();
	}
}

/*****************************************************************************
* Interrupcion Timer 0
*****************************************************************************/
/** \brief	Rutina de interrupción del Timer 0.
 *	
 *	Este timer genera una interrupción cada 1ms, en el se incrementan diferentes \n
 *	contadores de cada máquina de estados y se operan las valvulas servo o se modifican \n
 *	los tiempos de las PWM.
 */
 
ISR (TIMER0_COMP_vect) {
	/*------------------------------------------------------------------------------------*
	 * Contadores de operacion del buzzer
	 *------------------------------------------------------------------------------------*/
	if (ulContTiempoSilenciarAlarma != 0) {
		ulContTiempoSilenciarAlarma--;
		CLEAR_BIT(BUZZER_PORT, BUZZER);
	}
	else {
		if (chBuzzerModo == BUZZER_CORTO) {
			uContBuzzerON = BUZZER_TIEMPO_ON_CORTO;
			chBuzzerModo = BUZZER_OFF;
		}
		else if (chBuzzerModo == BUZZER_LARGO) {
			uContBuzzerON = BUZZER_TIEMPO_ON_LARGO;
			chBuzzerModo = BUZZER_OFF;
		}
		else if ((chBuzzerModo == BUZZER_INTERMITENTE) && (uContBuzzerON == 0) && (uContBuzzerOFF == 0)){
			uContBuzzerON = BUZZER_TIEMPO_ON;
		}

		if (uContBuzzerON > 0){
			SET_BIT(BUZZER_PORT, BUZZER);
			uContBuzzerON--;
			if ((uContBuzzerON == 0) && (chBuzzerModo == BUZZER_INTERMITENTE)){
				uContBuzzerOFF = BUZZER_TIEMPO_OFF;
			}
		}
		else if (uContBuzzerOFF > 0) {
			CLEAR_BIT(BUZZER_PORT, BUZZER);
			uContBuzzerOFF--;
			if ((uContBuzzerOFF == 0) && (chBuzzerModo == BUZZER_INTERMITENTE)){
				uContBuzzerON = BUZZER_TIEMPO_ON;
			}
		}
		else{
			CLEAR_BIT(BUZZER_PORT, BUZZER);
		}
	}

	/*------------------------------------------------------------------------------------*
	 * Contadores de las máquinas de estado de entradas discretas
	 *------------------------------------------------------------------------------------*/
	/* Actualización del tiempo de integración y estado de las entradas discretas */
	if (uTiempoIntegracionEntradas < TIEMPO_INTEGRACION_ENTRADAS) {
		uTiempoIntegracionEntradas++;
		for (int i=0; i<CANTIDAD_ENTRADAS; i++){
			if ((uEstadoEntradasInstantaneas & (SET<<i))!=0){
				uValorIntegracionEntradas[i] ++;
			}
		}
	}
	else {
		for(int i=0; i<CANTIDAD_ENTRADAS; i++){
			if ((uValorIntegracionEntradas[i] >= VALOR_INTEGRACION_ENTRADAS) && ((uEstadoEntradas & (SET<<i)) == 0)) {
				SET_BIT(uEstadoEntradas, i);
				SET_BIT(uFlagEntradasActivadas, i);
				uTimeoutEntradas = TIMEOUT_ENTRADAS;
			}
			else if ((uValorIntegracionEntradas[i] < VALOR_INTEGRACION_ENTRADAS) && ((uEstadoEntradas & (SET<<i)) != 0)) {
				CLEAR_BIT(uEstadoEntradas, i);
				SET_BIT(uFlagEntradasLiberadas, i);
				uTimeoutEntradas = TIMEOUT_ENTRADAS;
			}
			uValorIntegracionEntradas[i] = 0;
		}
		uTiempoIntegracionEntradas = 0;
	}

	/* descuento del Tiemout del flag de las entradas discretas */
	if (uTimeoutEntradas > 0)
		uTimeoutEntradas --;

	/*------------------------------------------------------------------------------------*
	 * Contadores de las máquinas de estado de alarmas
	 *------------------------------------------------------------------------------------*/
	/* conductimetro 1 */
	if (flCond1valoruS > flValorAlarmaCond1){
		if (uContTiempoAlarmaCond1 < uTiempoAlarmaCond1 * TIMER0_A_SEGUNDOS){
			uContTiempoAlarmaCond1++;
		}
	}
	else {
		if (uContTiempoAlarmaCond1 > 0){
			uContTiempoAlarmaCond1--;
		}
	}

	/* conductimetro 2 */
	if (flCond2valoruS > flValorAlarmaCond2){
		if (uContTiempoAlarmaCond2 < uTiempoAlarmaCond2 * TIMER0_A_SEGUNDOS){
			uContTiempoAlarmaCond2++;
		}
	}
	else {
		if (uContTiempoAlarmaCond2 > 0){
			uContTiempoAlarmaCond2--;
		}
	}


	/*------------------------------------------------------------------------------------*
	 * Contadores de las máquinas de estado de Display
	 *------------------------------------------------------------------------------------*/
	/* contador para el refresco del display */
	if (uContDisplay != 0){
		uContDisplay--;
	}

	/* contador para el parpadeo del display */
	if (uContDisplayON > 0){
		uContDisplayON--;
		if (uContDisplayON == 0){
			uContDisplayOFF = TIEMPO_DISPLAY_OFF;
		}
	}
	else if (uContDisplayOFF > 0) {
		uContDisplayOFF--;
		if (uContDisplayOFF == 0){
			uContDisplayON = TIEMPO_DISPLAY_ON;
		}
	}


	/*------------------------------------------------------------------------------------*
	 * Contadores de las máquinas de estado de USART
	 *------------------------------------------------------------------------------------*/
	/* decremento del contador de tiempo para time out de espera de USART0 */
	if (uContTimeoutEsperaUSART0 != 0){
		uContTimeoutEsperaUSART0 --;
	}

	/* decremento del contador de tiempo para time out de electura del buffer de recepcion */
	if (uContTimeoutLecturaRxUSART0 != 0){
		uContTimeoutLecturaRxUSART0 --;
	}

	/* decremento del contador de tiempo para timeout de recepcion para cerrar el buffer */
	if (uContTimeoutRxUSART0 != 0){
		uContTimeoutRxUSART0 --;
	}

//	/* decremento del contador de tiempo para time out de envio del buffer de transmision */
//	if (uContTimeoutTxUSART0 != 0){
//		uContTimeoutTxUSART0 --;
//	}

	/* decremento del contador de tiempo para time out de electura del buffer de recepcion */
	if (uContTimeoutLecturaRxUSART1 != 0)
		uContTimeoutLecturaRxUSART1 --;

	/* decremento del contador de tiempo para timeout de recepcion para cerrar el buffer */
	if (uContTimeoutRxUSART1 != 0)
		uContTimeoutRxUSART1 --;

	/* decremento del contador de tiempo para time out de envio del buffer de transmision */
	if (uContTimeoutTxUSART1 != 0)
		uContTimeoutTxUSART1 --;
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
 
ISR(__vector_default) {
}
