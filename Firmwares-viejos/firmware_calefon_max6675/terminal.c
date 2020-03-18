/** \file	Terminal.c
 *  \brief	Contiene las maquinas de estados correspondientes al manejo de la termianl.
 */

#include "USART0.h"
#include "macros.h"
#include "inicio.h"
#include "terminal.h"
#include "utiles.h"
#include "eeprom.h"
#include "control.h"
#include "motorPP.h"

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

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern unsigned char	uchEstadoBufferRx_USART0;				/* USART0.c, variable de estado del buffer de recepción del UASART, lleno o vacio */
extern char				chBufferRx_USART0[USART0_BUFFER_ZISE];	/* USART0.c, buffer de recepcion USART del uc */

extern int 				relativePositionBySteps;				/**< \brief posicion relativa de la valvula en cantidad de steps realizados */
extern int 				stepToRotate;							/**< \brief cantidad de pasos a rotar, + derecha, - izquierda */
extern char 			chPrintEstado;
extern char 			chPrintPID;

/* GRUPO_PID */
extern float			flPIDkP;
extern float			flPIDkI;
extern float			flPIDkD;
extern float			flPIDintMax;
extern float			flVariacionCaudalSetP;						/**< \brief porcentaje para el cual si varia el caudal corrijo fuera del PID */
extern float 			flCaudalEscalaKsPID;
extern float 			flEscalaVariacionCaudal;
extern unsigned int		uPeriodoPID;
extern unsigned int 	uPeriodoVedaPID;
extern unsigned int 	uPeriodoVedaCaudal;

extern float			flPIDkP_Eeprom;
extern float			flPIDkI_Eeprom;
extern float			flPIDkD_Eeprom;
extern float			flPIDintMax_Eeprom;
extern float			flVariacionCaudalSetP_Eeprom;
extern float 			flCaudalEscalaKsPID_Eeprom;
extern float 			flEscalaVariacionCaudal_Eeprom;
extern unsigned int		uPeriodoPID_Eeprom;
extern unsigned int 	uPeriodoVedaPID_Eeprom;
extern unsigned int 	uPeriodoVedaCaudal_Eeprom;

extern float			flPIDkP_Eeprom_BK;
extern float			flPIDkI_Eeprom_BK;
extern float			flPIDkD_Eeprom_BK;
extern float			flPIDintMax_Eeprom_BK;
extern float			flVariacionCaudalSetP_Eeprom_BK;
extern float 			flCaudalEscalaKsPID_Eeprom_BK;
extern float 			flEscalaVariacionCaudal_Eeprom_BK;
extern unsigned int		uPeriodoPID_Eeprom_BK;
extern unsigned int 	uPeriodoVedaPID_Eeprom_BK;
extern unsigned int 	uPeriodoVedaCaudal_Eeprom_BK;

/*****************************************************************************
* Strings de comandos
*****************************************************************************/
const char Comando_MotorGasPP[]				PROGMEM = "MGPP\0";		// movimiento del motor paso a paso
const char Comando_KP[]						PROGMEM = "KP\0";
const char Comando_KI[]						PROGMEM = "KI\0";
const char Comando_KD[]						PROGMEM = "KD\0";
const char Comando_Max_Integ[]				PROGMEM = "MAXINT\0";	// Maximo valor que puede tomar el termino integral
const char Comando_Periodo_PID[]			PROGMEM = "TPID\0";		// Periodo PID, periodo de actuacion del PID
const char Comando_Periodo_Veda_PID[] 		PROGMEM = "TVPID\0";	// Tiempo Veda PID, tiempo de veda para la actuacion por ajuste de temperatura del PID
const char Comando_Periodo_Veda_Caudal[] 	PROGMEM = "TVC\0";		// Tiempo Veda Caudal, tiempo de veda para la actuacion por variacion brusca de caudal
const char Comando_Porcent_Var_Caudal[]		PROGMEM = "PVC\0";		// Porcentaje Variacion Caudal
const char Comando_Caudal_Escala_PID[]		PROGMEM = "CEKPID\0";	// caudal para el cual Kp, Ki, Kd se escalan en 1. 0 desactiva el modo
const char Comando_Escala_Var_Caudal[]		PROGMEM = "EVC\0";		// Factor de escala para el salto de posicion de la valvula de gas por variacion brusca de caudal
const char Comando_Perfil[]					PROGMEM = "PERFIL\0";
const char Comando_Print[]					PROGMEM = "PRINT\0";
const char Comando_Estado[]					PROGMEM = "ESTADO\0";
const char Comando_PID[]					PROGMEM = "PID\0";
const char Comando_Reset[]					PROGMEM = "RESET\0";
const char Comando_Reiniciar[]				PROGMEM = "REINICIAR\0";
const char Comando_Factory_Reset[]			PROGMEM = "FABRICA RESET\0";

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
			// chBufferRx_USART0 = SERIE xxxx.x\0
			if (Detectar_Comando_Buffer_P(Comando_MotorGasPP, chBufferRx_USART0)) {
				int posFinal = relativePositionBySteps;
				if ((*(chBufferRx_USART0+auxIndexTerminal) == '+') || (*(chBufferRx_USART0+auxIndexTerminal) == '-')){
					stepToRotate = atoi(chBufferRx_USART0+auxIndexTerminal);
					posFinal += stepToRotate;
					if (posFinal > MAX_MOTOR_PP_ACUMULADOS){
						posFinal = MAX_MOTOR_PP_ACUMULADOS;
					}
					else if (posFinal < MIN_MOTOR_PP_ACUMULADOS){
						posFinal = MIN_MOTOR_PP_ACUMULADOS;
					}
				}
				sprintf(chBufferRx_USART0, "%S: %d->%d", (wchar_t*)Comando_MotorGasPP, relativePositionBySteps, posFinal);
			}

			// chBufferRx_USART0 = PERFIL x\0
			else if (Detectar_Comando_Buffer_P(Comando_Perfil, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					int perfil = atoi(chBufferRx_USART0+auxIndexTerminal);
					if (perfil == PERFIL_1){
						flPIDkP = PERFIL_1_PID_KP;
						flPIDkI = PERFIL_1_PID_KI;
						flPIDkD = PERFIL_1_PID_KD;
						flPIDintMax = PERFIL_1_MAX_PID_INTEG;
						flVariacionCaudalSetP = PERFIL_1_VARIACION_CAUDAL;
						flEscalaVariacionCaudal = PERFIL_1_ESCALA_VARIACION_CAUDAL;
						flCaudalEscalaKsPID = PERFIL_1_CAUDAL_ESCALA_KS_PID;
						uPeriodoPID = PERFIL_1_PID_PERIODO;
						uPeriodoVedaPID = PERFIL_1_PERIODO_VEDA_PID;
						uPeriodoVedaCaudal = PERFIL_1_PERIODO_VEDA_CAUDAL;
					}
					else if (perfil == PERFIL_2){
						flPIDkP = PERFIL_2_PID_KP;
						flPIDkI = PERFIL_2_PID_KI;
						flPIDkD = PERFIL_2_PID_KD;
						flPIDintMax = PERFIL_2_MAX_PID_INTEG;
						flVariacionCaudalSetP = PERFIL_2_VARIACION_CAUDAL;
						flEscalaVariacionCaudal = PERFIL_2_ESCALA_VARIACION_CAUDAL;
						flCaudalEscalaKsPID = PERFIL_2_CAUDAL_ESCALA_KS_PID;
						uPeriodoPID = PERFIL_2_PID_PERIODO;
						uPeriodoVedaPID = PERFIL_2_PERIODO_VEDA_PID;
						uPeriodoVedaCaudal = PERFIL_2_PERIODO_VEDA_CAUDAL;
					}
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPIDkP = DEFAULT_PID_KP;
					flPIDkI = DEFAULT_PID_KI;
					flPIDkD = DEFAULT_PID_KD;
					flPIDintMax = DEFAULT_MAX_PID_INTEG;
					flVariacionCaudalSetP = DEFAULT_VARIACION_CAUDAL;
					flEscalaVariacionCaudal = DEFAULT_ESCALA_VARIACION_CAUDAL;
					flCaudalEscalaKsPID = DEFAULT_CAUDAL_ESCALA_KS_PID;
					uPeriodoPID = DEFAULT_PID_PERIODO;
					uPeriodoVedaPID = DEFAULT_PERIODO_VEDA_PID;
					uPeriodoVedaCaudal = DEFAULT_PERIODO_VEDA_CAUDAL;
				}
				Float_a_Eeprom(&flPIDkP_Eeprom, &flPIDkP);
				Float_a_Eeprom(&flPIDkP_Eeprom_BK, &flPIDkP);
				Float_a_Eeprom(&flPIDkI_Eeprom, &flPIDkI);
				Float_a_Eeprom(&flPIDkI_Eeprom_BK, &flPIDkI);
				Float_a_Eeprom(&flPIDkD_Eeprom, &flPIDkD);
				Float_a_Eeprom(&flPIDkD_Eeprom_BK, &flPIDkD);
				Float_a_Eeprom(&flPIDintMax_Eeprom, &flPIDintMax);
				Float_a_Eeprom(&flPIDintMax_Eeprom_BK, &flPIDintMax);
				Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom, &flVariacionCaudalSetP);
				Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom_BK, &flVariacionCaudalSetP);
				Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom, &flEscalaVariacionCaudal);
				Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom_BK, &flEscalaVariacionCaudal);
				Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom, &flCaudalEscalaKsPID);
				Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom_BK, &flCaudalEscalaKsPID);
				UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom, &uPeriodoPID);
				UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom_BK, &uPeriodoPID);
				UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom, &uPeriodoVedaCaudal);
				UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom_BK, &uPeriodoVedaCaudal);
				UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom, &uPeriodoVedaPID);
				UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom_BK, &uPeriodoVedaPID);
				Escribir_Checksum_Eeprom (GRUPO_PID);

				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_KP);
				dtostrf(flPIDkP, 7, 3, chBufferRx_USART0+auxIndexTerminal);
				auxIndexTerminal += 7;
				auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, "\n\r%S: ", (wchar_t*)Comando_KI);
				dtostrf(flPIDkI, 7, 3, chBufferRx_USART0+auxIndexTerminal);
				auxIndexTerminal += 7;
				auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, "\n\r%S: ", (wchar_t*)Comando_KD);
				dtostrf(flPIDkD, 7, 3, chBufferRx_USART0+auxIndexTerminal);
				auxIndexTerminal += 7;
				auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, "\n\r%S: ", (wchar_t*)Comando_Max_Integ);
				dtostrf(flPIDintMax, 4, 1, chBufferRx_USART0+auxIndexTerminal);
				auxIndexTerminal += 4;
				auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, "\n\r%S: ", (wchar_t*)Comando_Porcent_Var_Caudal);
				dtostrf(flVariacionCaudalSetP, 5, 1, chBufferRx_USART0+auxIndexTerminal);
				auxIndexTerminal += 5;
				auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, "\n\r%S: ", (wchar_t*)Comando_Escala_Var_Caudal);
				dtostrf(flEscalaVariacionCaudal, 4, 2, chBufferRx_USART0+auxIndexTerminal);
				auxIndexTerminal += 4;
				auxIndexTerminal += sprintf(chBufferRx_USART0+auxIndexTerminal, "\n\r%S: ", (wchar_t*)Comando_Caudal_Escala_PID);
				dtostrf(flCaudalEscalaKsPID, 5, 2, chBufferRx_USART0+auxIndexTerminal);
				auxIndexTerminal += 5;
				sprintf(chBufferRx_USART0+auxIndexTerminal, "\n\r%S: %u\n\r%S: %u\n\r%S: %u", (wchar_t*)Comando_Periodo_PID, uPeriodoPID, (wchar_t*)Comando_Periodo_Veda_PID, uPeriodoVedaPID, (wchar_t*)Comando_Periodo_Veda_Caudal, uPeriodoVedaCaudal);
			}

			// chBufferRx_USART0 = CEKPID xxx.xxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Caudal_Escala_PID, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flCaudalEscalaKsPID = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom, &flCaudalEscalaKsPID);
					Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom_BK, &flCaudalEscalaKsPID);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flCaudalEscalaKsPID=DEFAULT_CAUDAL_ESCALA_KS_PID;

					Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom, &flCaudalEscalaKsPID);
					Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom_BK, &flCaudalEscalaKsPID);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Caudal_Escala_PID);
				dtostrf(flCaudalEscalaKsPID, 5, 2, chBufferRx_USART0+auxIndexTerminal);
			}

			// chBufferRx_USART0 = EVC xxx.xxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Escala_Var_Caudal, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flEscalaVariacionCaudal = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom, &flEscalaVariacionCaudal);
					Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom_BK, &flEscalaVariacionCaudal);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flEscalaVariacionCaudal=DEFAULT_ESCALA_VARIACION_CAUDAL;

					Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom, &flEscalaVariacionCaudal);
					Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom_BK, &flEscalaVariacionCaudal);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Escala_Var_Caudal);
				dtostrf(flEscalaVariacionCaudal, 4, 2, chBufferRx_USART0+auxIndexTerminal);
			}

			// chBufferRx_USART0 = PVC xxx.xxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Porcent_Var_Caudal, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flVariacionCaudalSetP = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom, &flVariacionCaudalSetP);
					Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom_BK, &flVariacionCaudalSetP);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flVariacionCaudalSetP=DEFAULT_VARIACION_CAUDAL;

					Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom, &flVariacionCaudalSetP);
					Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom_BK, &flVariacionCaudalSetP);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Porcent_Var_Caudal);
				dtostrf(flVariacionCaudalSetP, 5, 1, chBufferRx_USART0+auxIndexTerminal);
			}

			// chBufferRx_USART0 = KP xxx.xxx\0
			else if (Detectar_Comando_Buffer_P(Comando_KP, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flPIDkP = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flPIDkP_Eeprom, &flPIDkP);
					Float_a_Eeprom(&flPIDkP_Eeprom_BK, &flPIDkP);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPIDkP=DEFAULT_PID_KP;

					Float_a_Eeprom(&flPIDkP_Eeprom, &flPIDkP);
					Float_a_Eeprom(&flPIDkP_Eeprom_BK, &flPIDkP);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_KP);
				dtostrf(flPIDkP, 7, 3, chBufferRx_USART0+auxIndexTerminal);
			}

			// chBufferRx_USART0 = KI xxx.xxx\0
			else if (Detectar_Comando_Buffer_P(Comando_KI, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flPIDkI = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flPIDkI_Eeprom, &flPIDkI);
					Float_a_Eeprom(&flPIDkI_Eeprom_BK, &flPIDkI);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPIDkI=DEFAULT_PID_KI;

					Float_a_Eeprom(&flPIDkI_Eeprom, &flPIDkI);
					Float_a_Eeprom(&flPIDkI_Eeprom_BK, &flPIDkI);
					Escribir_Checksum_Eeprom (GRUPO_PID);
					auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_KI);
				}
				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_KI);
				dtostrf(flPIDkI, 7, 3, chBufferRx_USART0+auxIndexTerminal);
			}

			// chBufferRx_USART0 = KD xxx.xxx\0
			else if (Detectar_Comando_Buffer_P(Comando_KD, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flPIDkD = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flPIDkD_Eeprom, &flPIDkD);
					Float_a_Eeprom(&flPIDkD_Eeprom_BK, &flPIDkD);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPIDkD=DEFAULT_PID_KD;

					Float_a_Eeprom(&flPIDkD_Eeprom, &flPIDkD);
					Float_a_Eeprom(&flPIDkD_Eeprom_BK, &flPIDkD);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_KD);
				dtostrf(flPIDkD, 7, 3, chBufferRx_USART0+auxIndexTerminal);
			}

			// chBufferRx_USART0 = MAXINT xxx.xxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Max_Integ, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					flPIDintMax = From_String_to_Float(chBufferRx_USART0+auxIndexTerminal, USART0_BUFFER_ZISE);

					Float_a_Eeprom(&flPIDintMax_Eeprom, &flPIDintMax);
					Float_a_Eeprom(&flPIDintMax_Eeprom_BK, &flPIDintMax);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					flPIDintMax=DEFAULT_MAX_PID_INTEG;

					Float_a_Eeprom(&flPIDintMax_Eeprom, &flPIDintMax);
					Float_a_Eeprom(&flPIDintMax_Eeprom_BK, &flPIDintMax);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				auxIndexTerminal = sprintf(chBufferRx_USART0, "%S: ", (wchar_t*)Comando_Max_Integ);
				dtostrf(flPIDintMax, 7, 3, chBufferRx_USART0+auxIndexTerminal);
			}

			// chBufferRx_USART0 = TPID xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Periodo_PID, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uPeriodoPID = atoi(chBufferRx_USART0+auxIndexTerminal);

					UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom, &uPeriodoPID);
					UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom_BK, &uPeriodoPID);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uPeriodoPID = DEFAULT_PID_PERIODO;

					UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom, &uPeriodoPID);
					UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom_BK, &uPeriodoPID);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				sprintf(chBufferRx_USART0, "%S: %dmseg", (wchar_t*)Comando_Periodo_PID, uPeriodoPID);
			}

			// chBufferRx_USART0 = TVC xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Periodo_Veda_Caudal, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uPeriodoVedaCaudal = atoi(chBufferRx_USART0+auxIndexTerminal);

					UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom, &uPeriodoVedaCaudal);
					UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom_BK, &uPeriodoVedaCaudal);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uPeriodoVedaCaudal = DEFAULT_PERIODO_VEDA_CAUDAL;

					UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom, &uPeriodoVedaCaudal);
					UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom_BK, &uPeriodoVedaCaudal);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				sprintf(chBufferRx_USART0, "%S: %dmseg", (wchar_t*)Comando_Periodo_Veda_Caudal, uPeriodoVedaCaudal);
			}

			// chBufferRx_USART0 = TVPID xxxx\0
			else if (Detectar_Comando_Buffer_P(Comando_Periodo_Veda_PID, chBufferRx_USART0)) {
				if ((*(chBufferRx_USART0+auxIndexTerminal) >= '0') && (*(chBufferRx_USART0+auxIndexTerminal) <= '9')) {
					uPeriodoVedaPID = atoi(chBufferRx_USART0+auxIndexTerminal);

					UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom, &uPeriodoVedaPID);
					UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom_BK, &uPeriodoVedaPID);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				else if (Detectar_Comando_Buffer_P(Comando_Reset, chBufferRx_USART0+auxIndexTerminal)) {
					uPeriodoVedaPID = DEFAULT_PERIODO_VEDA_PID;

					UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom, &uPeriodoVedaPID);
					UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom_BK, &uPeriodoVedaPID);
					Escribir_Checksum_Eeprom (GRUPO_PID);
				}
				sprintf(chBufferRx_USART0, "%S: %dmseg", (wchar_t*)Comando_Periodo_Veda_PID, uPeriodoVedaPID);
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

			// chTextoSMS = PRINT\0
			else if (Detectar_Comando_Buffer_P(Comando_Print, chBufferRx_USART0)) {		// comando para dar inicio a la impresion de una variable
				if (Detectar_Comando_Buffer_P(Comando_Estado, chBufferRx_USART0+auxIndexTerminal)) {
					chPrintEstado = SI;
					sprintf(chBufferRx_USART0, "%S: %S %S", (wchar_t*)Comando_Print, (wchar_t*)Comando_Estado, (wchar_t*)MSG_Ok);
				}
				else if (Detectar_Comando_Buffer_P(Comando_PID, chBufferRx_USART0+auxIndexTerminal)) {
					chPrintPID = SI;
					sprintf(chBufferRx_USART0, "%S: %S %S\n\n\rCAUD;SETT;ACTT;VAL;COMD", (wchar_t*)Comando_Print, (wchar_t*)Comando_PID, (wchar_t*)MSG_Ok);
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
				if ((chPrintEstado == SI)||(chPrintPID == SI)) {		// si se toca una tecla o entra cualquier cosa corta las impresiones
					chPrintEstado = NO;
					chPrintPID = NO;
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
