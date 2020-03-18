/** \file	Terminal.c
 *  \brief	Contiene las maquinas de estados correspondientes al manejo de la termianl.
 */

#include "USART0.h"
#include "macros.h"
#include "inicioUc.h"
#include "eeprom.h"
#include "discretas.h"
#include "medicion_ADC.h"
#include "utiles.h"
#include "terminal.h"
#include "PWM420mA.h"
#include "alarmas.h"

#include <avr/io.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*****************************************************************************
* Numero de serie del equipo
*****************************************************************************/
const char Conductimetro_Info[]		PROGMEM = "F.J. LLAMAS S.A.\n\rTratamiento de Aguas-Filtracion\n\rConductimetro de Dos Canales\n\rFirmware: v1.00\n\rS/N: 0001\n\r\0";

/*****************************************************************************
* Variables Máquina de estados de manejo de la terminal
*****************************************************************************/
unsigned char				uchEstadoTerminal;						/**< \brief variable de la maquina de estados de manejo de terminal */
int							auxIndexTerminal;						/**< \brief contador auxiliar para la maquina de estados */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern unsigned char		uchEstadoBufferRx_USART0;				/* USART0.c, variable de estado del buffer de recepción del UASART, lleno o vacio */
extern char					chBufferRx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de recepcion USART del uc */

extern char					chADCprint;								/* medicion_ADC.c, indica si se imprime la medicion de los ADC */
extern char					chPrintDiscretas;						/* discretas.c, variable que indica la impresion del estado de las entradas por puerto serie */

extern float				flCond1valoruS;							/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern float				flCond2valoruS;							/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern float				flPWM1valorMA;							/* medicion_ADC.c, valor medio en mA medidos por el ADC */
extern float				flPWM2valorMA;							/* medicion_ADC.c, valor medio en mA medidos por el ADC */

/* GRUPO_ALARMAS */
extern volatile float			flValorAlarmaCond1;					/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
extern volatile unsigned int	uTiempoAlarmaCond1;					/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern volatile float			flValorAlarmaCond2;					/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
extern volatile unsigned int	uTiempoAlarmaCond2;					/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
/* GRUPO_ADC */
extern unsigned int				uMuestrasADCpwm1;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flPWM1aMA;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int				uMuestrasADCpwm2;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flPWM2aMA;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int				uMuestrasADCcond1;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flCond1auS;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 					flOffsetCond1;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
extern unsigned int				uMuestrasADCcond2;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flCond2auS;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
/* GRUPO_PWM */
extern unsigned int				uGananciaPWM1;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
extern unsigned int				uGananciaPWM2;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */

/*****************************************************************************
* Variables externas de eeprom
*****************************************************************************/
/* GRUPO_ALARMAS -1- */
extern float			flValorAlarmaCond1_Eeprom;					/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
extern unsigned int		uTiempoAlarmaCond1_Eeprom;					/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern float			flValorAlarmaCond2_Eeprom;					/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
extern unsigned int		uTiempoAlarmaCond2_Eeprom;					/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern float			flValorAlarmaCond1_Eeprom_BK;				/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
extern unsigned int		uTiempoAlarmaCond1_Eeprom_BK;				/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern float			flValorAlarmaCond2_Eeprom_BK;				/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
extern unsigned int		uTiempoAlarmaCond2_Eeprom_BK;				/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
/* GRUPO_ADC -2- */
extern unsigned int		uMuestrasADCpwm1_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM1aMA_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uMuestrasADCpwm2_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM2aMA_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uMuestrasADCcond1_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond1auS_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 			flOffsetCond1_Eeprom;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
extern unsigned int		uMuestrasADCcond2_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond2auS_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern unsigned int		uMuestrasADCpwm1_Eeprom_BK;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM1aMA_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uMuestrasADCpwm2_Eeprom_BK;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM2aMA_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uMuestrasADCcond1_Eeprom_BK;				/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond1auS_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 			flOffsetCond1_Eeprom_BK;					/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
extern unsigned int		uMuestrasADCcond2_Eeprom_BK;				/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond2auS_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
/* GRUPO_PWM -3- */
extern unsigned int		uGananciaPWM1_Eeprom;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
extern unsigned int		uGananciaPWM2_Eeprom;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
extern unsigned int		uGananciaPWM1_Eeprom_BK;					/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
extern unsigned int		uGananciaPWM2_Eeprom_BK;					/* PWM420mA.c, ganacia del lazo de control de la salida PWM */

/*****************************************************************************
* Strings de comandos
*****************************************************************************/
const char Comando_Info[]					PROGMEM = "INFO\0";
const char Comando_Print[]					PROGMEM = "PRINT \0";				/**< \brief comando para dar inicio a la impresion de una variable por terminal */
const char Comando_Discretas[]				PROGMEM = "DISCRETAS\0";			/**< \brief indicacion para habilitar las impresiones de las entradas discretas */
const char Comando_ADC[]					PROGMEM = "ADC\0";					/**< \brief indicacion para habilitar la impresion de GPS por serie */
const char Comando_Factory_Reset[]			PROGMEM = "FABRICA RESET\0";
const char Comando_Reset[]					PROGMEM = "RESET\0";
const char Comando_Muestras_PWM_1[]			PROGMEM = "FILTRO PWM 1\0";
const char Comando_Muestras_PWM_2[]			PROGMEM = "FILTRO PWM 2\0";
const char Comando_Muestras_Cond_1[]		PROGMEM = "FILTRO COND 1\0";
const char Comando_Muestras_Cond_2[]		PROGMEM = "FILTRO COND 2\0";
const char Comando_Ganancia_PWM_1[]			PROGMEM = "GANANCIA PWM 1\0";
const char Comando_Ganancia_PWM_2[]			PROGMEM = "GANANCIA PWM 2\0";
const char Comando_Tiempo_Alarma_Cond_1[]	PROGMEM = "TIEMPO ALARMA 1\0";
const char Comando_Tiempo_Alarma_Cond_2[]	PROGMEM = "TIEMPO ALARMA 2\0";
const char Comando_Set_Alarma_1[]			PROGMEM = "SET ALARMA 1\0";
const char Comando_Set_Alarma_2[]			PROGMEM = "SET ALARMA 2\0";
const char Comando_Calibrar_PWM_1[]			PROGMEM = "CAL PWM 1\0";
const char Comando_Calibrar_PWM_2[]			PROGMEM = "CAL PWM 2\0";
const char Comando_Calibrar_Cond_1[]		PROGMEM = "CAL COND 1\0";
const char Comando_Calibrar_Cond_2[]		PROGMEM = "CAL COND 2\0";
const char Comando_Set_PWM_1[]				PROGMEM = "SET PWM 1\0";
const char Comando_Set_PWM_2[]				PROGMEM = "SET PWM 2\0";
const char Comando_Set_Cond_1[]				PROGMEM = "SET COND 1\0";
const char Comando_Set_Cond_2[]				PROGMEM = "SET COND 2\0";
const char Comando_Set_Offset_1[]			PROGMEM = "SET OFFSET 1\0";

/*****************************************************************************
* Strings de mensajes
*****************************************************************************/
const char MSG_Comando[]					PROGMEM = "COMANDO\0";
const char MSG_Parametro[]					PROGMEM = "PARAMETRO\0";
const char MSG_Desconocido[]				PROGMEM = "DESCONOCIDO\0";
const char MSG_Borrado[]					PROGMEM = "BORRADO\0";
const char MSG_Ok[]							PROGMEM = "OK\0";
const char MSG_End[]						PROGMEM = "END\0";
const char MSG_Terminal_Lista[]				PROGMEM = "\n\n\rTerminal Lista!\n\n\r\0";
const char MSG_FueraDeRango[]				PROGMEM = "FUERA DE RANGO\0";
const char MSG_Offset[]						PROGMEM = "OFFSET\0";

/************************************************************************
* Máquina de estados de recepción y transmision por terminal
************************************************************************/

void Terminal(void) {
	switch (uchEstadoTerminal) {
		case TERMINAL_STANDBY:

			if (uchEstadoBufferRx_USART0 == LLENO) {		// si recibo desde el usuario
				String_a_Mayusculas (chBufferRx_USART0, USART0_BUFFER_ZISE);
				auxIndexTerminal = 0;
				uchEstadoTerminal = TERMINAL_PROCESO;
			}
			break;


		case TERMINAL_PROCESO:
			// chBufferRx_USART0 = INFO\0
			if (Comparar_Inicio_Buffer_P(Comando_Info, chBufferRx_USART0) == OK) {
				sprintf(chBufferRx_USART0, "%S", (wchar_t*)Conductimetro_Info);
			}


			// chBufferRx_USART0 = SET ALARMA 1 xxxx.x\0
			else if (Comparar_Inicio_Buffer_P(Comando_Set_Alarma_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flValorAlarmaCond1 = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValorAlarmaCond1 += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					if (flValorAlarmaCond1 < MIN_COND1_US){
						flValorAlarmaCond1 = 0;					// apago la alarma
					}
					else if (flValorAlarmaCond1 > MAX_COND1_US){
						flValorAlarmaCond1 = MAX_COND1_US;
					}
					Float_a_Eeprom(&flValorAlarmaCond1_Eeprom, (float *)&flValorAlarmaCond1);
					Float_a_Eeprom(&flValorAlarmaCond1_Eeprom_BK, (float *)&flValorAlarmaCond1);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Alarma_1);
					dtostrf(flValorAlarmaCond1, 6, 1, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Alarma_1);
					dtostrf(flValorAlarmaCond1, 6, 1, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Set_Alarma_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = SET ALARMA 2 xx.xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Set_Alarma_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flValorAlarmaCond2 = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValorAlarmaCond2 += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					if (flValorAlarmaCond2 < MIN_COND2_US){
						flValorAlarmaCond2 = 0;					// apago la alarma
					}
					else if (flValorAlarmaCond2 > MAX_COND2_US){
						flValorAlarmaCond2 = MAX_COND2_US;
					}
					Float_a_Eeprom(&flValorAlarmaCond2_Eeprom, (float *)&flValorAlarmaCond2);
					Float_a_Eeprom(&flValorAlarmaCond2_Eeprom_BK, (float *)&flValorAlarmaCond2);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Alarma_2);
					dtostrf(flValorAlarmaCond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Alarma_2);
					dtostrf(flValorAlarmaCond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Set_Alarma_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = CAL COND 1 xx.Xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Calibrar_Cond_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValorReal += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					if (flValorReal == 0) {
						flOffsetCond1 = flCond1valoruS;

						Float_a_Eeprom(&flOffsetCond1_Eeprom, &flOffsetCond1);
						Float_a_Eeprom(&flOffsetCond1_Eeprom_BK, &flOffsetCond1);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_Cond_1);
						dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
						auxIndexTerminal += 7;
						auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
						dtostrf(flOffsetCond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
					}
					else if ((flValorReal > MIN_COND1_US) && (flValorReal < MAX_COND1_US)){
						flCond1auS *= (flValorReal+flOffsetCond1)/(flCond1valoruS+flOffsetCond1);

						Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
						Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_Cond_1);
						dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
						auxIndexTerminal += 7;
						auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
						dtostrf(flOffsetCond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
					}
					else{
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Calibrar_Cond_1, (wchar_t*)MSG_FueraDeRango);
					}
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetCond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Calibrar_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = SET COND 1 xx.Xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Set_Cond_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValor = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValor += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					flCond1auS = flValor;

					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetCond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetCond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Set_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = SET OFFSET 1 xx.xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Set_Offset_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValor = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValor += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					flOffsetCond1 = flValor;

					Float_a_Eeprom(&flOffsetCond1_Eeprom, &flOffsetCond1);
					Float_a_Eeprom(&flOffsetCond1_Eeprom_BK, &flOffsetCond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetCond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetCond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Set_Offset_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = CAL COND 2 xxxx.x\0
			else if (Comparar_Inicio_Buffer_P(Comando_Calibrar_Cond_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValorReal += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					if ((flValorReal > MIN_COND2_US) && (flValorReal < MAX_COND2_US)){
						flCond2auS *= flValorReal/flCond2valoruS;

						Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
						Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_Cond_2);
						dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					}
					else{
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Calibrar_Cond_2, (wchar_t*)MSG_FueraDeRango);
					}
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Calibrar_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = SET COND 2 xxxx.x\0
			else if (Comparar_Inicio_Buffer_P(Comando_Set_Cond_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValor = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValor += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					flCond2auS = flValor;

					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Set_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = CAL PWM 1 xx.xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Calibrar_PWM_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValorReal += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					if ((flValorReal > I_MIN_MA) && (flValorReal < I_MAX_MA)){
						flPWM1aMA *= flPWM1valorMA/flValorReal;

						Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
						Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_PWM_1);
						dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					}
					else {
						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Calibrar_PWM_1, (wchar_t*) MSG_FueraDeRango);
					}
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Calibrar_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = SET PWM 1 xx.xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Set_PWM_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValor = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValor += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					flPWM1aMA = flValor;

					Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
					Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Set_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = CAL PWM 2 xx.xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Calibrar_PWM_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValorReal += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					if ((flValorReal > I_MIN_MA) && (flValorReal < I_MAX_MA)){
						flPWM2aMA *= flPWM2valorMA/flValorReal;

						Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
						Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_PWM_2);
						dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					}
					else {
						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Calibrar_PWM_2, (wchar_t*) MSG_FueraDeRango);
					}
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Calibrar_PWM_2);
					dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Calibrar_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = SET PWM 2 xx.xx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Set_PWM_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValor = (float)atoi(chBufferRx_USART0+auxIndexTerminal);

					while ((*(chBufferRx_USART0+auxIndexTerminal) != '.') && (auxIndexTerminal<USART0_BUFFER_ZISE))
						auxIndexTerminal++;

					if(*(chBufferRx_USART0+auxIndexTerminal) == '.') {
						auxIndexTerminal++;
						int j=10;
						while ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9') && (auxIndexTerminal<USART0_BUFFER_ZISE)) {
							flValor += (float)(*(chBufferRx_USART0+auxIndexTerminal) - '0')/j;
							auxIndexTerminal++;
							j*=10;
						}
					}

					flPWM2aMA = flValor;

					Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
					Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_PWM_2);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Set_PWM_2);
					dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Set_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = FILTRO PWM 1 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Muestras_PWM_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uMuestrasADCpwm1 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uMuestrasADCpwm1 < MINIMO_MUESTRAS_ADC){
						uMuestrasADCpwm1 = MINIMO_MUESTRAS_ADC;
					}
					else if (uMuestrasADCpwm1 > MAXIMO_MUESTRAS_ADC){
						uMuestrasADCpwm1 = MAXIMO_MUESTRAS_ADC;
					}
					UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom, &uMuestrasADCpwm1);
					UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom_BK, &uMuestrasADCpwm1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_1, uMuestrasADCpwm1);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_1, uMuestrasADCpwm1);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = FILTRO PWM 2 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Muestras_PWM_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uMuestrasADCpwm2 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uMuestrasADCpwm2 < MINIMO_MUESTRAS_ADC){
						uMuestrasADCpwm2 = MINIMO_MUESTRAS_ADC;
					}
					else if (uMuestrasADCpwm2 > MAXIMO_MUESTRAS_ADC){
						uMuestrasADCpwm2 = MAXIMO_MUESTRAS_ADC;
					}
					UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom, &uMuestrasADCpwm2);
					UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom_BK, &uMuestrasADCpwm2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_2, uMuestrasADCpwm2);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_2, uMuestrasADCpwm2);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = FILTRO COND 1 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Muestras_Cond_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uMuestrasADCcond1 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uMuestrasADCcond1 < MINIMO_MUESTRAS_ADC){
						uMuestrasADCcond1 = MINIMO_MUESTRAS_ADC;
					}
					else if (uMuestrasADCcond1 > MAXIMO_MUESTRAS_ADC){
						uMuestrasADCcond1 = MAXIMO_MUESTRAS_ADC;
					}
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_1, uMuestrasADCcond1);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_1, uMuestrasADCcond1);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = FILTRO COND 2 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Muestras_Cond_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uMuestrasADCcond2 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uMuestrasADCcond2 < MINIMO_MUESTRAS_ADC){
						uMuestrasADCcond2 = MINIMO_MUESTRAS_ADC;
					}
					else if (uMuestrasADCcond2 > MAXIMO_MUESTRAS_ADC){
						uMuestrasADCcond2 = MAXIMO_MUESTRAS_ADC;
					}
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_2, uMuestrasADCcond2);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_2, uMuestrasADCcond2);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = GANANCIA PWM 1 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Ganancia_PWM_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uGananciaPWM1 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uGananciaPWM1 < MINIMO_GANANCIA_LAZO_PWM){
						uGananciaPWM1 = MINIMO_GANANCIA_LAZO_PWM;
					}
					else if (uGananciaPWM1 > MAXIMO_GANANCIA_LAZO_PWM){
						uGananciaPWM1 = MAXIMO_GANANCIA_LAZO_PWM;
					}
					UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom, &uGananciaPWM1);
					UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom_BK, &uGananciaPWM1);
					Escribir_Checksum_Eeprom (GRUPO_PWM);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_1, uGananciaPWM1);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_1, uGananciaPWM1);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Ganancia_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = GANANCIA PWM 2 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Ganancia_PWM_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uGananciaPWM2 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uGananciaPWM2 < MINIMO_GANANCIA_LAZO_PWM){
						uGananciaPWM2 = MINIMO_GANANCIA_LAZO_PWM;
					}
					else if (uGananciaPWM2 > MAXIMO_GANANCIA_LAZO_PWM){
						uGananciaPWM2 = MAXIMO_GANANCIA_LAZO_PWM;
					}
					UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom, &uGananciaPWM2);
					UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom_BK, &uGananciaPWM2);
					Escribir_Checksum_Eeprom (GRUPO_PWM);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_2, uGananciaPWM2);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_2, uGananciaPWM2);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Ganancia_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = TIEMPO ALARMA 1 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Tiempo_Alarma_Cond_1, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '1')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uTiempoAlarmaCond1 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uTiempoAlarmaCond1 < MINIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond1 = MINIMO_TIEMPO_ALARMA_COND;
					}
					else if (uTiempoAlarmaCond1 > MAXIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond1 = MAXIMO_TIEMPO_ALARMA_COND;
					}
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom, (unsigned int *)&uTiempoAlarmaCond1);
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom_BK, (unsigned int *)&uTiempoAlarmaCond1);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dmseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_1, uTiempoAlarmaCond1);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %dmseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_1, uTiempoAlarmaCond1);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Tiempo_Alarma_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chBufferRx_USART0 = TIEMPO ALARMA 2 xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Tiempo_Alarma_Cond_2, chBufferRx_USART0) == OK) {
				while (*(chBufferRx_USART0+auxIndexTerminal) != '2')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (*(chBufferRx_USART0+auxIndexTerminal) == ' ')
					auxIndexTerminal++;

				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uTiempoAlarmaCond2 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uTiempoAlarmaCond2 < MINIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond2 = MINIMO_TIEMPO_ALARMA_COND;
					}
					else if (uTiempoAlarmaCond2 > MAXIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond2 = MAXIMO_TIEMPO_ALARMA_COND;
					}
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom, (unsigned int *)&uTiempoAlarmaCond2);
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom_BK, (unsigned int *)&uTiempoAlarmaCond2);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dmseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_2, uTiempoAlarmaCond2);
				}
				else if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %dmseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_2, uTiempoAlarmaCond2);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Tiempo_Alarma_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			// chTextoSMS = RESET\0
			else if (Comparar_Inicio_Buffer_P(Comando_Reset, chBufferRx_USART0) == OK) {
				UC_RESET();
			}


			// chTextoSMS = FABRICA RESET\0
			else if (Comparar_Inicio_Buffer_P(Comando_Factory_Reset, chBufferRx_USART0) == OK) {
				Carga_Configuracion_Fabrica_Eeprom(GRUPO_TODOS);
				sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Factory_Reset, (wchar_t*)MSG_Ok);
			}


			// chTextoSMS = PRINT xxxx\0
			else if (Comparar_Inicio_Buffer_P(Comando_Print, chBufferRx_USART0) == OK) {		// comando para dar inicio a la impresion de una variable
				while (*(chBufferRx_USART0+auxIndexTerminal) != ' ')
					auxIndexTerminal++;
				auxIndexTerminal++;
				if (Comparar_Inicio_Buffer_P(Comando_ADC, chBufferRx_USART0+auxIndexTerminal) == OK) {
					chADCprint = SI;
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Print, (wchar_t*)Comando_ADC, (wchar_t*)MSG_Ok);
				}
				else if (Comparar_Inicio_Buffer_P(Comando_Discretas, chBufferRx_USART0+auxIndexTerminal) == OK) {
					chPrintDiscretas = SI;
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Print, (wchar_t*)Comando_Discretas, (wchar_t*)MSG_Ok);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Print, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
				}
			}


			/* else if (...)
			 * 	 ...
			 */


			// chBufferRx_USART0 no contiene un comando conocido
			else {
				if ((chADCprint == SI)||(chPrintDiscretas == SI)) {		// si se toca una tecla o entra cualquier cosa corta las impresiones
					chADCprint = NO;
					chPrintDiscretas = NO;
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Print, (wchar_t*)MSG_End);
				}
				else {
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)MSG_Comando, (wchar_t*)MSG_Desconocido);
				}
			}

			Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
			Transmitir_Buffer_USART0_P (PSTR("\n\r\0"));

			auxIndexTerminal = 0;
			uchEstadoBufferRx_USART0 = VACIO;											// libero el buffer de recepcion
			uchEstadoTerminal = TERMINAL_STANDBY;
			break;


		default:
			auxIndexTerminal = 0;
			Transmitir_Buffer_USART0_P (PSTR("\n\r\0"));
			uchEstadoBufferRx_USART0 = VACIO;											// libero el buffer de recepcion
			uchEstadoTerminal = TERMINAL_STANDBY;
			break;
	}
}


/*****************************************************************************
* Funcion de inicio
*****************************************************************************/
void Inicio_Terminal (void) {
	sprintf(chBufferRx_USART0, "\n\n\r%S", (wchar_t*)Conductimetro_Info);
	Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);

	sprintf(chBufferRx_USART0, "%S", (wchar_t*)MSG_Terminal_Lista);
	Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
}
