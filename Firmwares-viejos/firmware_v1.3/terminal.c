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
* Variables Máquina de estados de manejo de la terminal
*****************************************************************************/
unsigned char				uchEstadoTerminal;							/**< \brief variable de la maquina de estados de manejo de terminal */
int							auxIndexTerminal;							/**< \brief contador auxiliar para la maquina de estados */
char						chNumeroSerie[NUMERO_SERIE_MAX_DIGITOS];	/**< \brief numero de serie del equipo */

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern unsigned char		uchEstadoBufferRx_USART0;				/* USART0.c, variable de estado del buffer de recepción del UASART, lleno o vacio */
extern char					chBufferRx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de recepcion USART del uc */

extern char					chADCprint;								/* medicion_ADC.c, indica si se imprime la medicion de los ADC */
extern char					chPrintEntradas;						/* discretas.c, variable que indica la impresion del estado de las entradas por puerto serie */
extern char					chPrintSalidas;							/* discretas.c, variable que indica la impresion del estado de las salidas por puerto serie */

extern volatile float		flCond1valoruS;							/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern volatile float		flCond2valoruS;							/* medicion_ADC.c, valor medio en uS/cm medidos por el ADC */
extern volatile float		flValorADCcond1;						/* medicion_ADC.c, valor del ADC en cuentas para los rangos segun correccion de linealidad */
extern volatile float		flValorADCcond2;						/* medicion_ADC.c, valor del ADC en cuentas para los rangos segun correccion de linealidad */
extern float				flPWM1valorMA;							/* medicion_ADC.c, valor medio en mA medidos por el ADC */
extern float				flPWM2valorMA;							/* medicion_ADC.c, valor medio en mA medidos por el ADC */
extern int					cond1MaxUs;
extern int					cond1MinUs;
extern int					cond2MaxUs;
extern int					cond2MinUs;

/* GRUPO_SERIE -0- */
extern char						chNumeroSerie_Eeprom[NUMERO_SERIE_MAX_DIGITOS];		/* terminal.c, numero de serie del equipo */
extern char						chNumeroSerie_Eeprom_BK[NUMERO_SERIE_MAX_DIGITOS];	/* terminal.c, numero de serie del equipo */

/* GRUPO_ALARMAS */
extern float					flValorAlarmaCond1;					/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
extern unsigned int				uTiempoAlarmaCond1;					/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern float					flValorAlarmaCond2;					/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
extern unsigned int				uTiempoAlarmaCond2;					/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern unsigned int				uTiempoSilenciarAlarma;				/* alarmas.c, tiempo en minutos por el cual se pospone el sonnido de la alarma */
/* GRUPO_ADC */
extern unsigned int				uMuestrasADCpwm1;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flPWM1aMA;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int				uMuestrasADCpwm2;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flPWM2aMA;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int				uRangoCond1;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
extern unsigned int				uMuestrasADCcond1;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flCond1auS;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 					flOffsetADCcond1;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
extern unsigned int				uRangoCond2;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
extern unsigned int				uMuestrasADCcond2;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float					flCond2auS;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 					flOffsetADCcond2;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
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
extern unsigned int		uTiempoSilenciarAlarma_Eeprom;				/* alarmas.c, tiempo en minutos por el cual se pospone el sonnido de la alarma */
extern float			flValorAlarmaCond1_Eeprom_BK;				/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
extern unsigned int		uTiempoAlarmaCond1_Eeprom_BK;				/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern float			flValorAlarmaCond2_Eeprom_BK;				/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
extern unsigned int		uTiempoAlarmaCond2_Eeprom_BK;				/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
extern unsigned int		uTiempoSilenciarAlarma_Eeprom_BK;			/* alarmas.c, tiempo en minutos por el cual se pospone el sonnido de la alarma */
/* GRUPO_ADC -2- */
extern unsigned int		uMuestrasADCpwm1_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM1aMA_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uMuestrasADCpwm2_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM2aMA_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uRangoCond1_Eeprom;							/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
extern unsigned int		uMuestrasADCcond1_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond1auS_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 			flOffsetADCcond1_Eeprom;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
extern unsigned int		uRangoCond2_Eeprom;							/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
extern unsigned int		uMuestrasADCcond2_Eeprom;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond2auS_Eeprom;							/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 			flOffsetADCcond2_Eeprom;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
extern unsigned int		uMuestrasADCpwm1_Eeprom_BK;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM1aMA_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uMuestrasADCpwm2_Eeprom_BK;					/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flPWM2aMA_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
extern unsigned int		uRangoCond1_Eeprom_BK;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
extern unsigned int		uMuestrasADCcond1_Eeprom_BK;				/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond1auS_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 			flOffsetADCcond1_Eeprom_BK;					/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
extern unsigned int		uRangoCond2_Eeprom_BK;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
extern unsigned int		uMuestrasADCcond2_Eeprom_BK;				/* medicion_ADC.c, cantidad de muestras para promediar */
extern float			flCond2auS_Eeprom_BK;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
extern float 			flOffsetADCcond2_Eeprom_BK;					/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
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
const char Comando_Entradas[]				PROGMEM = "ENTRADAS\0";				/**< \brief indicacion para habilitar las impresiones de las entradas discretas */
const char Comando_ADC[]					PROGMEM = "ADC\0";					/**< \brief indicacion para habilitar la impresion de GPS por serie */
const char Comando_Salidas[]				PROGMEM = "SALIDAS\0";
const char Comando_Reiniciar[]				PROGMEM = "REINICIAR\0";
const char Comando_Factory_Reset[]			PROGMEM = "FABRICA RESET\0";
const char Comando_Alarmas_Reset[]			PROGMEM = "ALARMAS RESET\0";
const char Comando_PWM_Reset[]				PROGMEM = "PWM RESET\0";
const char Comando_Cal_Reset[]				PROGMEM = "CAL RESET\0";
const char Comando_Reset[]					PROGMEM = "RESET\0";
const char Comando_Serie[]					PROGMEM = "SERIE\0";
const char Comando_Muestras_PWM_1[]			PROGMEM = "FILTRO PWM 1\0";
const char Comando_Muestras_PWM_2[]			PROGMEM = "FILTRO PWM 2\0";
const char Comando_Muestras_Cond_1[]		PROGMEM = "FILTRO COND 1\0";
const char Comando_Muestras_Cond_2[]		PROGMEM = "FILTRO COND 2\0";
const char Comando_Ganancia_PWM_1[]			PROGMEM = "GANANCIA PWM 1\0";
const char Comando_Ganancia_PWM_2[]			PROGMEM = "GANANCIA PWM 2\0";
const char Comando_Tiempo_Alarma_Cond_1[]	PROGMEM = "TIEMPO ALARMA 1\0";
const char Comando_Tiempo_Alarma_Cond_2[]	PROGMEM = "TIEMPO ALARMA 2\0";
const char Comando_Alarma_1[]				PROGMEM = "ALARMA 1\0";
const char Comando_Alarma_2[]				PROGMEM = "ALARMA 2\0";
const char Comando_Cal_PWM_1[]				PROGMEM = "CAL PWM 1\0";
const char Comando_Cal_PWM_2[]				PROGMEM = "CAL PWM 2\0";
const char Comando_Cal_Cond_1[]				PROGMEM = "CAL COND 1\0";
const char Comando_Cal_Cond_2[]				PROGMEM = "CAL COND 2\0";
const char Comando_Autocal_PWM_1[]			PROGMEM = "AUTOCAL PWM 1\0";
const char Comando_Autocal_PWM_2[]			PROGMEM = "AUTOCAL PWM 2\0";
const char Comando_Autocal_Cond_1[]			PROGMEM = "AUTOCAL COND 1\0";
const char Comando_Autocal_Cond_2[]			PROGMEM = "AUTOCAL COND 2\0";
const char Comando_Offset_1[]				PROGMEM = "OFFSET COND 1\0";
const char Comando_Offset_2[]				PROGMEM = "OFFSET COND 2\0";
const char Comando_Rango_1[]				PROGMEM = "RANGO COND 1\0";
const char Comando_Rango_2[]				PROGMEM = "RANGO COND 2\0";
const char Comando_Silenciar_Alarma[]		PROGMEM = "SILENCIAR ALARMA\0";

/*****************************************************************************
* Strings de mensajes
*****************************************************************************/
const char MSG_Comando[]					PROGMEM = "COMANDO\0";
const char MSG_Parametro[]					PROGMEM = "PARAMETRO\0";
const char MSG_Desconocido[]				PROGMEM = "DESCONOCIDO\0";
const char MSG_Borrado[]					PROGMEM = "BORRADO\0";
const char MSG_Ok[]							PROGMEM = "OK\0";
const char MSG_End[]						PROGMEM = "END\0";
const char MSG_Error[]						PROGMEM = "ERROR\0";
const char MSG_Terminal_Lista[]				PROGMEM = "\n\n\rTerminal Lista!\n\n\r\0";
const char MSG_FueraDeRango[]				PROGMEM = "FUERA DE RANGO\0";
const char MSG_Offset[]						PROGMEM = "OFFSET\0";
const char MSG_Rango_0_20_G[]				PROGMEM = "0-20G\0";
const char MSG_Rango_0_20_C[]				PROGMEM = "0-20C\0";
const char MSG_Rango_2_200_G[]				PROGMEM = "2-200G\0";
const char MSG_Rango_2_200_C[]				PROGMEM = "2-200C\0";
const char MSG_Rango_20_2500_G[]			PROGMEM = "20-2500G\0";
const char MSG_Rango_20_2500_C[]			PROGMEM = "20-2500C\0";
const char MSG_Llamas[]						PROGMEM = "F.J. LLAMAS S.A.\0";					/**< \brief nombre de la empresa */
const char Conductimetro_Info[]				PROGMEM = "\n\rTratamiento de Aguas-Filtracion\n\rConductimetro de Dos Canales\n\r\0";
const char MSG_Serial_Number[]				PROGMEM = "SN:\0";
const char MSG_Software[]					PROGMEM = "SW:\0";
const char Software_Version[]				PROGMEM = "v1.3\0";

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
			if (Detectar_Comando_Buffer_P(Comando_Info, chBufferRx_USART0)) {
				sprintf(chBufferRx_USART0, "%S%S%S%s %S%S", (wchar_t*) MSG_Llamas, (wchar_t*)Conductimetro_Info, (wchar_t*)MSG_Serial_Number, chNumeroSerie, (wchar_t*)MSG_Software, (wchar_t*)Software_Version);
			}

			// chBufferRx_USART0 = SERIE xxxx.x\0
			else if (Detectar_Comando_Buffer_P(Comando_Serie, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (*(chBufferRx_USART0+auxIndexTerminal) >= '!') && (*(chBufferRx_USART0+auxIndexTerminal) <= '}')) {
					int i=0;
					while ((*(chBufferRx_USART0+auxIndexTerminal+i) >= '!') && (*(chBufferRx_USART0+auxIndexTerminal+i) <= '}') && (i<NUMERO_SERIE_MAX_DIGITOS-1)) {
						chNumeroSerie[i] = *(chBufferRx_USART0+auxIndexTerminal+i);
						Char_a_Eeprom (chNumeroSerie_Eeprom+i, chNumeroSerie+i);
						Char_a_Eeprom (chNumeroSerie_Eeprom_BK+i, chNumeroSerie+i);
						i++;
					}
					while (i<NUMERO_SERIE_MAX_DIGITOS) {
						chNumeroSerie[i] = '\0';
						Char_a_Eeprom (chNumeroSerie_Eeprom+i, chNumeroSerie+i);
						Char_a_Eeprom (chNumeroSerie_Eeprom_BK+i, chNumeroSerie+i);
						i++;
					}
					Escribir_Checksum_Eeprom (GRUPO_SERIE);
					sprintf(chBufferRx_USART0, "%S: %s", (wchar_t*)Comando_Serie, chNumeroSerie);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?'){
					sprintf(chBufferRx_USART0, "%S: %s", (wchar_t*)Comando_Serie, chNumeroSerie);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Serie, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = RANGO COND 1 xxxx.x\0
			else if (Detectar_Comando_Buffer_P(Comando_Rango_1, chBufferRx_USART0)) {
				if (Detectar_Comando_Buffer_P(MSG_Rango_0_20_C, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond1 = COND_0_20_C;
					uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;
					flCond1auS = DEFAULT_COND020C_A_US;
					flOffsetADCcond1 = DEFAULT_COND020C_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_0_20_C);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_0_20_G, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond1 = COND_0_20_G;
					uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;
					flCond1auS = DEFAULT_COND020G_A_US;
					flOffsetADCcond1 = DEFAULT_COND020G_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_0_20_G);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_2_200_C, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond1 = COND_2_200_C;
					uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;
					flCond1auS = DEFAULT_COND2200C_A_US;
					flOffsetADCcond1 = DEFAULT_COND2200C_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_2_200_C);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_2_200_G, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond1 = COND_2_200_G;
					uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;
					flCond1auS = DEFAULT_COND2200G_A_US;
					flOffsetADCcond1 = DEFAULT_COND2200G_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_2_200_G);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_20_2500_C, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond1 = COND_20_2500_C;
					uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;
					flCond1auS = DEFAULT_COND202500C_A_US;
					flOffsetADCcond1 = DEFAULT_COND202500C_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_20_2500_C);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_20_2500_G, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond1 = COND_20_2500_G;
					uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;
					flCond1auS = DEFAULT_COND202500G_A_US;
					flOffsetADCcond1 = DEFAULT_COND202500G_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_20_2500_G);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					if (uRangoCond1 == COND_0_20_C) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_0_20_C);
					}
					else if (uRangoCond1 == COND_0_20_G) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_0_20_G);
					}
					else if (uRangoCond1 == COND_2_200_C) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_2_200_C);
					}
					else if (uRangoCond1 == COND_2_200_G) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_2_200_G);
					}
					else if (uRangoCond1 == COND_20_2500_C) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_20_2500_C);
					}
					else if (uRangoCond1 == COND_20_2500_G) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Rango_20_2500_G);
					}
					else {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Error);
					}
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Rango_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
				Cargar_Valores_Rango_Por_Canal();
			}


			// chBufferRx_USART0 = RANGO COND 2 xxxx.x\0
			else if (Detectar_Comando_Buffer_P(Comando_Rango_2, chBufferRx_USART0)) {
				if (Detectar_Comando_Buffer_P(MSG_Rango_0_20_C, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond2 = COND_0_20_C;
					uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;
					flCond2auS = DEFAULT_COND020C_A_US;
					flOffsetADCcond2 = DEFAULT_COND020C_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_0_20_C);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_0_20_G, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond2 = COND_0_20_G;
					uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;
					flCond2auS = DEFAULT_COND020G_A_US;
					flOffsetADCcond2 = DEFAULT_COND020G_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_0_20_G);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_2_200_C, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond2 = COND_2_200_C;
					uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;
					flCond2auS = DEFAULT_COND2200C_A_US;
					flOffsetADCcond2 = DEFAULT_COND2200C_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_2_200_C);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_2_200_G, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond2 = COND_2_200_G;
					uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;
					flCond2auS = DEFAULT_COND2200G_A_US;
					flOffsetADCcond2 = DEFAULT_COND2200G_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_2_200_G);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_20_2500_C, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond2 = COND_20_2500_C;
					uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;
					flCond2auS = DEFAULT_COND202500C_A_US;
					flOffsetADCcond2 = DEFAULT_COND202500C_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_20_2500_C);
				}
				else if (Detectar_Comando_Buffer_P(MSG_Rango_20_2500_G, chBufferRx_USART0+auxIndexTerminal)) {
					uRangoCond2 = COND_20_2500_G;
					uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;
					flCond2auS = DEFAULT_COND202500G_A_US;
					flOffsetADCcond2 = DEFAULT_COND202500G_OFFSET;

					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_20_2500_G);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					if (uRangoCond2 == COND_0_20_C) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_0_20_C);
					}
					else if (uRangoCond2 == COND_0_20_G) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_0_20_G);
					}
					else if (uRangoCond2 == COND_2_200_C) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_2_200_C);
					}
					else if (uRangoCond2 == COND_2_200_G) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_2_200_G);
					}
					else if (uRangoCond2 == COND_20_2500_C) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_20_2500_C);
					}
					else if (uRangoCond2 == COND_20_2500_G) {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Rango_20_2500_G);
					}
					else {
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Error);
					}
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Rango_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
				Cargar_Valores_Rango_Por_Canal();
			}


			// chBufferRx_USART0 = ALARMA 1 xxxx.x\0
			else if (Detectar_Comando_Buffer_P(Comando_Alarma_1, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flValorAlarmaCond1 = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					if (flValorAlarmaCond1 < cond1MinUs){
						flValorAlarmaCond1 = 0;					// apago la alarma
					}
					else if (flValorAlarmaCond1 > cond1MaxUs){
						flValorAlarmaCond1 = cond1MaxUs;
					}
					Float_a_Eeprom(&flValorAlarmaCond1_Eeprom, &flValorAlarmaCond1);
					Float_a_Eeprom(&flValorAlarmaCond1_Eeprom_BK, &flValorAlarmaCond1);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Alarma_1);
					dtostrf(flValorAlarmaCond1, 6, 1, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Alarma_1);
					dtostrf(flValorAlarmaCond1, 6, 1, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Alarma_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = ALARMA 2 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Alarma_2, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flValorAlarmaCond2 = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					if (flValorAlarmaCond2 < cond2MinUs){
						flValorAlarmaCond2 = 0;					// apago la alarma
					}
					else if (flValorAlarmaCond2 > cond2MaxUs){
						flValorAlarmaCond2 = cond2MaxUs;
					}
					Float_a_Eeprom(&flValorAlarmaCond2_Eeprom, &flValorAlarmaCond2);
					Float_a_Eeprom(&flValorAlarmaCond2_Eeprom_BK, &flValorAlarmaCond2);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Alarma_2);
					dtostrf(flValorAlarmaCond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Alarma_2);
					dtostrf(flValorAlarmaCond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Alarma_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = AUTOCAL COND 1 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Autocal_Cond_1, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					if (flValorReal == 0) {
						flOffsetADCcond1 = flValorADCcond1;

						Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
						Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_1);
						dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
						auxIndexTerminal += 7;
						auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
						dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
					}
					else if ((flValorReal > cond1MinUs) && (flValorReal < cond1MaxUs)){
						flCond1auS *= flValorReal / flCond1valoruS;

						Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
						Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_1);
						dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
						auxIndexTerminal += 7;
						auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
						dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
					}
					else{
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Autocal_Cond_1, (wchar_t*)MSG_FueraDeRango);
					}
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					if (uRangoCond1 == COND_20_2500_G){
						flCond1auS = DEFAULT_COND202500G_A_US;
						flOffsetADCcond1 = DEFAULT_COND202500G_OFFSET;
					}
					else if (uRangoCond1 == COND_0_20_G){
						flCond1auS = DEFAULT_COND020G_A_US;
						flOffsetADCcond1 = DEFAULT_COND020G_OFFSET;
					}
					else if (uRangoCond1 == COND_2_200_G){
						flCond1auS = DEFAULT_COND2200G_A_US;
						flOffsetADCcond1 = DEFAULT_COND2200G_OFFSET;
					}
					else if (uRangoCond1 == COND_20_2500_C){
						flCond1auS = DEFAULT_COND202500C_A_US;
						flOffsetADCcond1 = DEFAULT_COND202500C_OFFSET;
					}
					else if (uRangoCond1 == COND_0_20_C){
						flCond1auS = DEFAULT_COND020C_A_US;
						flOffsetADCcond1 = DEFAULT_COND020C_OFFSET;
					}
					else if (uRangoCond1 == COND_2_200_C){
						flCond1auS = DEFAULT_COND2200C_A_US;
						flOffsetADCcond1 = DEFAULT_COND2200C_OFFSET;
					}

					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Autocal_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = CAL COND 1 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Cal_Cond_1, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flCond1auS = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					if (uRangoCond1 == COND_20_2500_G){
						flCond1auS = DEFAULT_COND202500G_A_US;
					}
					else if (uRangoCond1 == COND_0_20_G){
						flCond1auS = DEFAULT_COND020G_A_US;
					}
					else if (uRangoCond1 == COND_2_200_G){
						flCond1auS = DEFAULT_COND2200G_A_US;
					}
					else if (uRangoCond1 == COND_20_2500_C){
						flCond1auS = DEFAULT_COND202500C_A_US;
					}
					else if (uRangoCond1 == COND_0_20_C){
						flCond1auS = DEFAULT_COND020C_A_US;
					}
					else if (uRangoCond1 == COND_2_200_C){
						flCond1auS = DEFAULT_COND2200C_A_US;
					}

					Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
					Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Cal_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = OFFSET COND 1 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Offset_1, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flOffsetADCcond1 = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					if (uRangoCond1 == COND_20_2500_G){
						flOffsetADCcond1 = DEFAULT_COND202500G_OFFSET;
					}
					else if (uRangoCond1 == COND_0_20_G){
						flOffsetADCcond1 = DEFAULT_COND020G_OFFSET;
					}
					else if (uRangoCond1 == COND_2_200_G){
						flOffsetADCcond1 = DEFAULT_COND2200G_OFFSET;
					}
					else if (uRangoCond1 == COND_20_2500_C){
						flOffsetADCcond1 = DEFAULT_COND202500C_OFFSET;
					}
					else if (uRangoCond1 == COND_0_20_C){
						flOffsetADCcond1 = DEFAULT_COND020C_OFFSET;
					}
					else if (uRangoCond1 == COND_2_200_C){
						flOffsetADCcond1 = DEFAULT_COND2200C_OFFSET;
					}

					Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
					Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_1);
					dtostrf(flCond1auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond1, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Offset_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = OFFSET COND 2 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Offset_2, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flOffsetADCcond2 = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					if (uRangoCond2 == COND_20_2500_G){
						flOffsetADCcond2 = DEFAULT_COND202500G_OFFSET;
					}
					else if (uRangoCond2 == COND_0_20_G){
						flOffsetADCcond2 = DEFAULT_COND020G_OFFSET;
					}
					else if (uRangoCond2 == COND_2_200_G){
						flOffsetADCcond2 = DEFAULT_COND2200G_OFFSET;
					}
					else if (uRangoCond2 == COND_20_2500_C){
						flOffsetADCcond2 = DEFAULT_COND202500C_OFFSET;
					}
					else if (uRangoCond2 == COND_0_20_C){
						flOffsetADCcond2 = DEFAULT_COND020C_OFFSET;
					}
					else if (uRangoCond2 == COND_2_200_C){
						flOffsetADCcond2 = DEFAULT_COND2200C_OFFSET;
					}

					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Offset_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = AUTOCAL COND 2 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Autocal_Cond_2, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					if (flValorReal == 0) {
						flOffsetADCcond2 = flValorADCcond2;

						Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
						Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_2);
						dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
						auxIndexTerminal += 7;
						auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
						dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
					}
					else if ((flValorReal > cond2MinUs) && (flValorReal < cond2MaxUs)){
						flCond2auS *= flValorReal / flCond2valoruS;

						Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
						Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_2);
						dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
						auxIndexTerminal += 7;
						auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
						dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
					}
					else{
						sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Autocal_Cond_2, (wchar_t*)MSG_FueraDeRango);
					}
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					if (uRangoCond2 == COND_20_2500_G){
						flCond2auS = DEFAULT_COND202500G_A_US;
						flOffsetADCcond2 = DEFAULT_COND202500G_OFFSET;
					}
					else if (uRangoCond2 == COND_0_20_G){
						flCond2auS = DEFAULT_COND020G_A_US;
						flOffsetADCcond2 = DEFAULT_COND020G_OFFSET;
					}
					else if (uRangoCond2 == COND_2_200_G){
						flCond2auS = DEFAULT_COND2200G_A_US;
						flOffsetADCcond2 = DEFAULT_COND2200G_OFFSET;
					}
					else if (uRangoCond2 == COND_20_2500_C){
						flCond2auS = DEFAULT_COND202500C_A_US;
						flOffsetADCcond2 = DEFAULT_COND202500C_OFFSET;
					}
					else if (uRangoCond2 == COND_0_20_C){
						flCond2auS = DEFAULT_COND020C_A_US;
						flOffsetADCcond2 = DEFAULT_COND020C_OFFSET;
					}
					else if (uRangoCond2 == COND_2_200_C){
						flCond2auS = DEFAULT_COND2200C_A_US;
						flOffsetADCcond2 = DEFAULT_COND2200C_OFFSET;
					}

					Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
					Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Autocal_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = CAL COND 2 xxxx.x\0
			else if (Detectar_Comando_Buffer_P(Comando_Cal_Cond_2, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flCond2auS = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					if (uRangoCond2 == COND_20_2500_G){
						flCond2auS = DEFAULT_COND202500G_A_US;
					}
					else if (uRangoCond2 == COND_0_20_G){
						flCond2auS = DEFAULT_COND020G_A_US;
					}
					else if (uRangoCond2 == COND_2_200_G){
						flCond2auS = DEFAULT_COND2200G_A_US;
					}
					else if (uRangoCond2 == COND_20_2500_C){
						flCond2auS = DEFAULT_COND202500C_A_US;
					}
					else if (uRangoCond2 == COND_0_20_C){
						flCond2auS = DEFAULT_COND020C_A_US;
					}
					else if (uRangoCond2 == COND_2_200_C){
						flCond2auS = DEFAULT_COND2200C_A_US;
					}

					Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
					Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_Cond_2);
					dtostrf(flCond2auS, 7, 4, chBufferRx_USART0+auxIndexTerminal);
					auxIndexTerminal += 7;
					auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, " ; %S: ", (wchar_t*)MSG_Offset);
					dtostrf(flOffsetADCcond2, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Cal_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = AUTOCAL PWM 1 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Autocal_PWM_1, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

//					if ((flValorReal > I_MIN_MA) && (flValorReal < I_MAX_MA)){
						flPWM1aMA *= flPWM1valorMA/flValorReal;

						Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
						Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_PWM_1);
						dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
//					}
//					else {
//						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Autocal_PWM_1, (wchar_t*) MSG_FueraDeRango);
//					}
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPWM1aMA = DEFAULT_PWM1_A_MA;

					Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
					Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Autocal_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = CAL PWM 1 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Cal_PWM_1, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flPWM1aMA = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
					Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPWM1aMA = DEFAULT_PWM1_A_MA;

					Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
					Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_PWM_1);
					dtostrf(flPWM1aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Cal_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = AUTOCAL PWM 2 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Autocal_PWM_2, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					float flValorReal = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

//					if ((flValorReal > I_MIN_MA) && (flValorReal < I_MAX_MA)){
						flPWM2aMA *= flPWM2valorMA/flValorReal;

						Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
						Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
						Escribir_Checksum_Eeprom (GRUPO_ADC);

						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_PWM_2);
						dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
//					}
//					else {
//						auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Autocal_PWM_2, (wchar_t*) MSG_FueraDeRango);
//					}
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPWM2aMA = DEFAULT_PWM2_A_MA;

					Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
					Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_PWM_2);
					dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Autocal_PWM_2);
					dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Autocal_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = CAL PWM 2 xx.xx\0
			else if (Detectar_Comando_Buffer_P(Comando_Cal_PWM_2, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) != '\0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flPWM2aMA = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
					Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_PWM_2);
					dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPWM2aMA = DEFAULT_PWM2_A_MA;

					Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
					Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
					Escribir_Checksum_Eeprom (GRUPO_ADC);

					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_PWM_2);
					dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Cal_PWM_2);
					dtostrf(flPWM2aMA, 7, 4, chBufferRx_USART0+auxIndexTerminal);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Cal_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = FILTRO PWM 1 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Muestras_PWM_1, chBufferRx_USART0)) {
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
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uMuestrasADCpwm1 = DEFAULT_MUESTRAS_ADC_PWM1;

					UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom, &uMuestrasADCpwm1);
					UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom_BK, &uMuestrasADCpwm1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_1, uMuestrasADCpwm1);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_1, uMuestrasADCpwm1);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = FILTRO PWM 2 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Muestras_PWM_2, chBufferRx_USART0)) {
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
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uMuestrasADCpwm2 = DEFAULT_MUESTRAS_ADC_PWM2;

					UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom, &uMuestrasADCpwm2);
					UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom_BK, &uMuestrasADCpwm2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_2, uMuestrasADCpwm2);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_PWM_2, uMuestrasADCpwm2);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = FILTRO COND 1 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Muestras_Cond_1, chBufferRx_USART0)) {
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
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;

					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_1, uMuestrasADCcond1);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_1, uMuestrasADCcond1);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = FILTRO COND 2 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Muestras_Cond_2, chBufferRx_USART0)) {
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
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;

					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
					UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
					Escribir_Checksum_Eeprom (GRUPO_ADC);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_2, uMuestrasADCcond2);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Muestras_Cond_2, uMuestrasADCcond2);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Muestras_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = GANANCIA PWM 1 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Ganancia_PWM_1, chBufferRx_USART0)) {
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
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uGananciaPWM1 = DEFAULT_GANACIA_LAZO_PWM_1;

					UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom, &uGananciaPWM1);
					UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom_BK, &uGananciaPWM1);
					Escribir_Checksum_Eeprom (GRUPO_PWM);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_1, uGananciaPWM1);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_1, uGananciaPWM1);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Ganancia_PWM_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = GANANCIA PWM 2 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Ganancia_PWM_2, chBufferRx_USART0)) {
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
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uGananciaPWM2 = DEFAULT_GANACIA_LAZO_PWM_2;

					UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom, &uGananciaPWM2);
					UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom_BK, &uGananciaPWM2);
					Escribir_Checksum_Eeprom (GRUPO_PWM);
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_2, uGananciaPWM2);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %d", (wchar_t*)Comando_Ganancia_PWM_2, uGananciaPWM2);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Ganancia_PWM_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = TIEMPO ALARMA 1 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Tiempo_Alarma_Cond_1, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uTiempoAlarmaCond1 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uTiempoAlarmaCond1 < MINIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond1 = MINIMO_TIEMPO_ALARMA_COND;
					}
					else if (uTiempoAlarmaCond1 > MAXIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond1 = MAXIMO_TIEMPO_ALARMA_COND;
					}
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom, &uTiempoAlarmaCond1);
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom_BK, &uTiempoAlarmaCond1);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_1, uTiempoAlarmaCond1);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uTiempoAlarmaCond1 = DEFAULT_TIEMPO_ALARMA_COND_1;

					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom, &uTiempoAlarmaCond1);
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom_BK, &uTiempoAlarmaCond1);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_1, uTiempoAlarmaCond1);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %dseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_1, uTiempoAlarmaCond1);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Tiempo_Alarma_Cond_1, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = TIEMPO ALARMA 2 xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Tiempo_Alarma_Cond_2, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uTiempoAlarmaCond2 = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uTiempoAlarmaCond2 < MINIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond2 = MINIMO_TIEMPO_ALARMA_COND;
					}
					else if (uTiempoAlarmaCond2 > MAXIMO_TIEMPO_ALARMA_COND){
						uTiempoAlarmaCond2 = MAXIMO_TIEMPO_ALARMA_COND;
					}
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom, &uTiempoAlarmaCond2);
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom_BK, &uTiempoAlarmaCond2);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_2, uTiempoAlarmaCond2);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uTiempoAlarmaCond2 = DEFAULT_TIEMPO_ALARMA_COND_2;

					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom, &uTiempoAlarmaCond2);
					UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom_BK, &uTiempoAlarmaCond2);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_2, uTiempoAlarmaCond2);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %dseg", (wchar_t*)Comando_Tiempo_Alarma_Cond_2, uTiempoAlarmaCond2);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Tiempo_Alarma_Cond_2, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chBufferRx_USART0 = SILENCIAR ALARMA  xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Silenciar_Alarma, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uTiempoSilenciarAlarma = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (uTiempoSilenciarAlarma < MINIMO_TIEMPO_SILENCIAR_ALARMA){
						uTiempoSilenciarAlarma = MINIMO_TIEMPO_SILENCIAR_ALARMA;
					}
					else if (uTiempoSilenciarAlarma > MAXIMO_TIEMPO_SILENCIAR_ALARMA){
						uTiempoSilenciarAlarma = MAXIMO_TIEMPO_SILENCIAR_ALARMA;
					}
					UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom, &uTiempoSilenciarAlarma);
					UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom_BK, &uTiempoSilenciarAlarma);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dmin", (wchar_t*)Comando_Silenciar_Alarma, uTiempoSilenciarAlarma);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uTiempoSilenciarAlarma = DEFAULT_TIEMPO_SILENCIAR_ALARMA;

					UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom, &uTiempoSilenciarAlarma);
					UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom_BK, &uTiempoSilenciarAlarma);
					Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
					sprintf(chBufferRx_USART0, "%S: %dmin", (wchar_t*)Comando_Silenciar_Alarma, uTiempoSilenciarAlarma);
				}
				else {//if (*(chBufferRx_USART0+auxIndexTerminal) == '?') {
					sprintf(chBufferRx_USART0, "%S: %dmin", (wchar_t*)Comando_Silenciar_Alarma, uTiempoSilenciarAlarma);
				}
//				else {
//					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Silenciar_Alarma, (wchar_t*)MSG_Parametro, (wchar_t*)MSG_Desconocido);
//				}
			}


			// chTextoSMS = REINICIAR\0
			else if (Detectar_Comando_Buffer_P(Comando_Reiniciar, chBufferRx_USART0)) {
				sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Reiniciar, (wchar_t*)MSG_Ok);
				Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
				UC_RESET();
			}


			// chTextoSMS = FABRICA RESET\0
			else if (Detectar_Comando_Buffer_P(Comando_Factory_Reset, chBufferRx_USART0)) {
				Carga_Configuracion_Fabrica_Eeprom(GRUPO_TODOS);
				sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Factory_Reset, (wchar_t*)MSG_Ok);
				Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
				UC_RESET();
			}


			// chTextoSMS = ALARMAS RESET\0
			else if (Detectar_Comando_Buffer_P(Comando_Alarmas_Reset, chBufferRx_USART0)) {
				Carga_Configuracion_Fabrica_Eeprom(GRUPO_ALARMAS);
				sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Alarmas_Reset, (wchar_t*)MSG_Ok);
				Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
				UC_RESET();
			}


			// chTextoSMS = PWM RESET\0
			else if (Detectar_Comando_Buffer_P(Comando_PWM_Reset, chBufferRx_USART0)) {
				Carga_Configuracion_Fabrica_Eeprom(GRUPO_PWM);
				sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_PWM_Reset, (wchar_t*)MSG_Ok);
				Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
				UC_RESET();
			}


			// chTextoSMS = CAL RESET\0
			else if (Detectar_Comando_Buffer_P(Comando_Cal_Reset, chBufferRx_USART0)) {
				Carga_Configuracion_Fabrica_Eeprom(GRUPO_ADC);
				sprintf(chBufferRx_USART0, "%S: %S", (wchar_t*)Comando_Cal_Reset, (wchar_t*)MSG_Ok);
				Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
				UC_RESET();
			}


			// chTextoSMS = PRINT xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Print, chBufferRx_USART0)) {		// comando para dar inicio a la impresion de una variable
				if (Detectar_Comando_Buffer_P(Comando_ADC, chBufferRx_USART0+auxIndexTerminal)) {
					chADCprint = SI;
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Print, (wchar_t*)Comando_ADC, (wchar_t*)MSG_Ok);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Entradas, chBufferRx_USART0+auxIndexTerminal)) {
					chPrintEntradas = SI;
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Print, (wchar_t*)Comando_Entradas, (wchar_t*)MSG_Ok);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Salidas, chBufferRx_USART0+auxIndexTerminal)) {
					chPrintSalidas = SI;
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Print, (wchar_t*)Comando_Salidas, (wchar_t*)MSG_Ok);
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
				if ((chADCprint == SI)||(chPrintEntradas == SI)||(chPrintSalidas == SI)) {		// si se toca una tecla o entra cualquier cosa corta las impresiones
					chADCprint = NO;
					chPrintEntradas = NO;
					chPrintSalidas = NO;
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

/************************************************************************
* Funcion que reconoce un comando en el inicio de un mesaje recibido y modifica el index de la terminal solo si detecta
************************************************************************/
int Detectar_Comando_Buffer_P (const char *chBufferOriginalPtr, char *chBufferRecibidoPtr) {
	int i = 0, endIndex = 0;
	while ((*(chBufferRecibidoPtr+endIndex) == '\n') || (*(chBufferRecibidoPtr+endIndex) == '\r'))	// si lo recibido comienza con \n o \r lo salto
		endIndex++;

	while (pgm_read_byte(chBufferOriginalPtr+i) != '\0') {								// Comparo lo recibido
		if (pgm_read_byte(chBufferOriginalPtr+i) != *(chBufferRecibidoPtr+endIndex)) {	// verifico que coincidan
			endIndex=0;
			break;
		}
		i++;
		endIndex++;
	}
	// si detecte...
	if (endIndex != 0){
		// si hay un espacio
		if(*(chBufferRecibidoPtr+endIndex) == ' '){
		endIndex++;
		}
		auxIndexTerminal=endIndex;	//modifica el index de la terminal solo si detecta
	}
	return endIndex;
}

/*****************************************************************************
* Funcion de inicio
*****************************************************************************/
void Inicio_Terminal (void) {
	uchEstadoTerminal = TERMINAL_STANDBY;

	sprintf(chBufferRx_USART0, "%S", (wchar_t*)MSG_Terminal_Lista);
	Transmitir_Buffer_USART0 (chBufferRx_USART0, USART0_BUFFER_ZISE);
}
