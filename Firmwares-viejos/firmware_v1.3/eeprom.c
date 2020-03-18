
/** \file	eeprom.c
 *  \brief	Contiene las funciones para el manejo de datos desde y hacia la eeprom.
 */

#include "eeprom.h"
#include "macros.h"
#include "USART0.h"
#include "inicioUc.h"
#include "alarmas.h"
#include "medicion_ADC.h"
#include "PWM420mA.h"
#include "terminal.h"

#include <avr/io.h>
#include <inttypes.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <avr/pgmspace.h>

/*****************************************************************************
* Variables externas
*****************************************************************************/
extern char	chBufferTx_USART0[USART0_BUFFER_ZISE];	/* USART.c, buffer de transmisión USART del uc */

/* GRUPO_SERIE */
extern char						chNumeroSerie[NUMERO_SERIE_MAX_DIGITOS];	/* terminal.c, numero de serie del equipo */

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
extern float 					flOffsetADCcond2;						/* medicion_ADC.c, corracion de offset del cond 2 para la puesta a cero */

/* GRUPO_PWM */
extern unsigned int				uGananciaPWM1;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
extern unsigned int				uGananciaPWM2;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */

/*****************************************************************************
* Variables en eeprom
*****************************************************************************/
/* asigno para cada veriable un lugar en la eeprom  y le doy un valor por defecto para el inicio depues de grabar el uc */

/* GRUPO_SERIE -0- */
char			chNumeroSerie_Eeprom[NUMERO_SERIE_MAX_DIGITOS]		EEMEM;						/* terminal.c, numero de serie del equipo */
unsigned int	uChecksumGeneral_Eeprom								EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

char			chNumeroSerie_Eeprom_BK[NUMERO_SERIE_MAX_DIGITOS]	EEMEM;						/* terminal.c, numero de serie del equipo */
unsigned int	uChecksumGeneral_Eeprom_BK							EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

/* GRUPO_ALARMAS -1- */
float			flValorAlarmaCond1_Eeprom					EEMEM;						/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
unsigned int	uTiempoAlarmaCond1_Eeprom					EEMEM;						/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
float			flValorAlarmaCond2_Eeprom					EEMEM;						/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
unsigned int	uTiempoAlarmaCond2_Eeprom					EEMEM;						/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
unsigned int	uTiempoSilenciarAlarma_Eeprom				EEMEM;						/* alarmas.c, tiempo en minutos por el cual se pospone el sonnido de la alarma */
unsigned int	uChecksumAlarmas_Eeprom						EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

float			flValorAlarmaCond1_Eeprom_BK				EEMEM;						/* alarmas.c, valor para la activación de la alarma del conductimetro 1 */
unsigned int	uTiempoAlarmaCond1_Eeprom_BK				EEMEM;						/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
float			flValorAlarmaCond2_Eeprom_BK				EEMEM;						/* alarmas.c, valor para la activación de la alarma del conductimetro 2 */
unsigned int	uTiempoAlarmaCond2_Eeprom_BK				EEMEM;						/* alarmas.c, tiempo de permanencia por encima del valor para la activacion de la alarma */
unsigned int	uTiempoSilenciarAlarma_Eeprom_BK			EEMEM;						/* alarmas.c, tiempo en minutos por el cual se pospone el sonnido de la alarma */
unsigned int	uChecksumAlarmas_Eeprom_BK					EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

/* GRUPO_ADC -2- */
unsigned int	uMuestrasADCpwm1_Eeprom 					EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flPWM1aMA_Eeprom							EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
unsigned int	uMuestrasADCpwm2_Eeprom 					EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flPWM2aMA_Eeprom							EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
unsigned int	uRangoCond1_Eeprom							EEMEM;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
unsigned int	uMuestrasADCcond1_Eeprom					EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flCond1auS_Eeprom							EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
float 			flOffsetADCcond1_Eeprom 					EEMEM;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
unsigned int	uRangoCond2_Eeprom							EEMEM;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
unsigned int	uMuestrasADCcond2_Eeprom					EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flCond2auS_Eeprom							EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
float 			flOffsetADCcond2_Eeprom 					EEMEM;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
unsigned int	uChecksumADC_Eeprom							EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

unsigned int	uMuestrasADCpwm1_Eeprom_BK					EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flPWM1aMA_Eeprom_BK 						EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
unsigned int	uMuestrasADCpwm2_Eeprom_BK					EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flPWM2aMA_Eeprom_BK 						EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a corriente */
unsigned int	uRangoCond1_Eeprom_BK						EEMEM;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
unsigned int	uMuestrasADCcond1_Eeprom_BK 				EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flCond1auS_Eeprom_BK						EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
float 			flOffsetADCcond1_Eeprom_BK					EEMEM;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
unsigned int	uRangoCond2_Eeprom_BK						EEMEM;						/* medicion_ADC.c, rango de trabajo del conductimetro segun la configuracion del hardware */
unsigned int	uMuestrasADCcond2_Eeprom_BK 				EEMEM;						/* medicion_ADC.c, cantidad de muestras para promediar */
float			flCond2auS_Eeprom_BK						EEMEM;						/* medicion_ADC.c, valor de conversion del ADC a uS/cm */
float 			flOffsetADCcond2_Eeprom_BK					EEMEM;						/* medicion_ADC.c, corracion de offset del cond 1 para la puesta a cero */
unsigned int	uChecksumADC_Eeprom_BK						EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

/* GRUPO_PWM -3- */
unsigned int	uGananciaPWM1_Eeprom						EEMEM;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
unsigned int	uGananciaPWM2_Eeprom						EEMEM;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
unsigned int	uChecksumPWM_Eeprom							EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

unsigned int	uGananciaPWM1_Eeprom_BK 					EEMEM;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
unsigned int	uGananciaPWM2_Eeprom_BK 					EEMEM;						/* PWM420mA.c, ganacia del lazo de control de la salida PWM */
unsigned int	uChecksumPWM_Eeprom_BK						EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

/*-----------------*/
float			flDireccionVaciaEeprom						EEMEM;						/**< \brief dirección vacia en la eeprom por si hay un error y se modifica algo, siempre dejo apuntado a ella */

/*****************************************************************************
* Mensajes de debug guardados en memoria de programa
*****************************************************************************/
const char validacion_Serie_MSG[]		PROGMEM = "Validacion GRUPO_SERIE (#0):\0";
const char validacion_Alarmas_MSG[] 	PROGMEM = "Validacion GRUPO_ALARMAS (#1):\0";
const char validacion_ADC_MSG[] 		PROGMEM = "Validacion GRUPO_ADC (#2):\0";
const char validacion_PWM_MSG[] 		PROGMEM = "Validacion GRUPO_PWM (#3):\0";
const char datosOriginalesOk_MSG[] 		PROGMEM = "Datos originales OK\0";
const char datosBackUpError_MSG[]		PROGMEM = "Datos de backup ERRONEOS\0";
const char datosOriginalesError_MSG[]	PROGMEM = "Datos originales ERRONEOS\0";
const char datosBackUpOK_MSG[]			PROGMEM = "Datos de backup OK\0";
const char datosOrigBackUPError_MSG[]	PROGMEM = "Datos originales y de backup ERRONEOS\0";
const char cargaPorDefecto_MSG[]		PROGMEM = "Carga valores por defecto\0";
const char numeroGrupo_MSG[]			PROGMEM = "Numero de Grupo =\0";

/*****************************************************************************
* Funcion de carga de variables desde eeprom al inicio
*****************************************************************************/

void Carga_Configuracion_Desde_Eeprom (void) {
	unsigned int	uCRC;								/* Checksum */
	unsigned int	uCRC_Eeprom;						/* Checksum eeprom */
	unsigned int	uCRC_BK;							/* Checksum backup */
	unsigned int	uCRC_Eeprom_BK;						/* Checksum eeprom backup */

	/* GRUPO_GENRAL -0- */
	unsigned int uContIntentosValidacionChecksum = 0;
	while (uContIntentosValidacionChecksum < CANT_INTENTOS_VALIDACION_CHECKSUM) {
		sprintf (chBufferTx_USART0, "\n\r%S %d", (wchar_t*)validacion_Serie_MSG, uContIntentosValidacionChecksum);
		Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

		uContIntentosValidacionChecksum++;

		// leo los valores originales
		uCRC = 0;
		for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++) {
			Eeprom_a_Char(chNumeroSerie_Eeprom+i, chNumeroSerie+i);										/* leeo los valores originales de la eeprom */
			uCRC += Char_a_Checksum(chNumeroSerie+i);													/* calculo el checksum de los valores lebantados a la ram */
		}
		Eeprom_a_UnsignedInt (&uChecksumGeneral_Eeprom, &uCRC_Eeprom);									/* levanto el checksum de la eeprom */

		// leo los valores de la copia
		uCRC_BK = 0;
		for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++) {
			Eeprom_a_Char(chNumeroSerie_Eeprom_BK+i, chNumeroSerie+i);										/* leeo los valores originales de la eeprom */
			uCRC_BK += Char_a_Checksum(chNumeroSerie+i);													/* calculo el checksum de los valores lebantados a la ram */
		}
		Eeprom_a_UnsignedInt (&uChecksumGeneral_Eeprom, &uCRC_Eeprom_BK);									/* levanto el checksum de la eeprom */

		// si ambas dan bien
		if ((uCRC == uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			break;
		}
		// si la copia da mal vuelvo a leer el original porque habia dado bien
		else if ((uCRC == uCRC_Eeprom) && (uCRC_BK != uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// leo los valores originales nuevamente
			uCRC = 0;
			for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++) {
				Eeprom_a_Char(chNumeroSerie_Eeprom+i, chNumeroSerie+i);										/* leeo los valores originales de la eeprom */
				uCRC += Char_a_Checksum(chNumeroSerie+i);													/* calculo el checksum de los valores lebantados a la ram */
			}
			Eeprom_a_UnsignedInt (&uChecksumGeneral_Eeprom, &uCRC_Eeprom);									/* levanto el checksum de la eeprom */

			// si lei bien los originales grabo la copia sobre el backup
			if (uCRC == uCRC_Eeprom) {
				for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++) {
					Char_a_Eeprom(chNumeroSerie_Eeprom_BK+i, chNumeroSerie+i);
				}
				UnsignedInt_a_Eeprom (&uChecksumGeneral_Eeprom_BK, &uCRC);

				break;
			}
		}
		// si el original da mal y la copia bien
		else if ((uCRC != uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// grabo el backup sobre los originales
			for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++) {
				Char_a_Eeprom(chNumeroSerie_Eeprom+i, chNumeroSerie+i);
			}
			UnsignedInt_a_Eeprom (&uChecksumGeneral_Eeprom, &uCRC_BK);

			break;
		}
		// si ambas dan mal y agote los intentos
		else if (uContIntentosValidacionChecksum == CANT_INTENTOS_VALIDACION_CHECKSUM){
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			Carga_Configuracion_Fabrica_Eeprom (GRUPO_SERIE);
		}
	}

	/* GRUPO_ALARMAS -1- */
	uContIntentosValidacionChecksum = 0;
	while (uContIntentosValidacionChecksum < CANT_INTENTOS_VALIDACION_CHECKSUM) {
		sprintf (chBufferTx_USART0, "\n\r%S %d", (wchar_t*)validacion_Alarmas_MSG, uContIntentosValidacionChecksum);
		Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

		uContIntentosValidacionChecksum++;

		// leo los valores originales
		Eeprom_a_Float(&flValorAlarmaCond1_Eeprom, &flValorAlarmaCond1);						/* leo los valores originales de la eeprom */
		uCRC = Float_a_Checksum(&flValorAlarmaCond1);											/* calculo el checksum de los valores lebantados a la ram */
		Eeprom_a_UnsignedInt(&uTiempoAlarmaCond1_Eeprom, &uTiempoAlarmaCond1);
		uCRC += UnsignedInt_a_Checksum(&uTiempoAlarmaCond1);
		Eeprom_a_Float(&flValorAlarmaCond2_Eeprom, &flValorAlarmaCond2);
		uCRC += Float_a_Checksum(&flValorAlarmaCond2);
		Eeprom_a_UnsignedInt(&uTiempoAlarmaCond2_Eeprom, &uTiempoAlarmaCond2);
		uCRC += UnsignedInt_a_Checksum(&uTiempoAlarmaCond2);
		Eeprom_a_UnsignedInt(&uTiempoSilenciarAlarma_Eeprom, &uTiempoSilenciarAlarma);
		uCRC += UnsignedInt_a_Checksum(&uTiempoSilenciarAlarma);
		Eeprom_a_UnsignedInt (&uChecksumAlarmas_Eeprom, &uCRC_Eeprom);									/* levanto el checksum de la eeprom */

		// leo los valores de la copia
		Eeprom_a_Float(&flValorAlarmaCond1_Eeprom_BK, &flValorAlarmaCond1);						/* leo los valores copia de la eeprom */
		uCRC_BK = Float_a_Checksum(&flValorAlarmaCond1);										/* calculo el checksum de los valores lebantados a la ram */
		Eeprom_a_UnsignedInt(&uTiempoAlarmaCond1_Eeprom_BK, &uTiempoAlarmaCond1);
		uCRC_BK += UnsignedInt_a_Checksum(&uTiempoAlarmaCond1);
		Eeprom_a_Float(&flValorAlarmaCond2_Eeprom_BK, &flValorAlarmaCond2);
		uCRC_BK += Float_a_Checksum(&flValorAlarmaCond2);
		Eeprom_a_UnsignedInt(&uTiempoAlarmaCond2_Eeprom_BK, &uTiempoAlarmaCond2);
		uCRC_BK += UnsignedInt_a_Checksum(&uTiempoAlarmaCond2);
		Eeprom_a_UnsignedInt(&uTiempoSilenciarAlarma_Eeprom_BK, &uTiempoSilenciarAlarma);
		uCRC_BK += UnsignedInt_a_Checksum(&uTiempoSilenciarAlarma);
		Eeprom_a_UnsignedInt (&uChecksumAlarmas_Eeprom_BK, &uCRC_Eeprom_BK);							/* levanto el checksum de la eeprom */

		// si ambas dan bien
		if ((uCRC == uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			break;
		}
		// si la copia da mal vuelvo a leer el original porque habia dado bien
		else if ((uCRC == uCRC_Eeprom) && (uCRC_BK != uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// leo los valores originales nuevamente
			Eeprom_a_Float(&flValorAlarmaCond1_Eeprom, &flValorAlarmaCond1);						/* leo los valores originales de la eeprom */
			uCRC = Float_a_Checksum(&flValorAlarmaCond1);											/* calculo el checksum de los valores lebantados a la ram */
			Eeprom_a_UnsignedInt(&uTiempoAlarmaCond1_Eeprom, &uTiempoAlarmaCond1);
			uCRC += UnsignedInt_a_Checksum(&uTiempoAlarmaCond1);
			Eeprom_a_Float(&flValorAlarmaCond2_Eeprom, &flValorAlarmaCond2);
			uCRC += Float_a_Checksum(&flValorAlarmaCond2);
			Eeprom_a_UnsignedInt(&uTiempoAlarmaCond2_Eeprom, &uTiempoAlarmaCond2);
			uCRC += UnsignedInt_a_Checksum(&uTiempoAlarmaCond2);
			Eeprom_a_UnsignedInt(&uTiempoSilenciarAlarma_Eeprom, &uTiempoSilenciarAlarma);
			uCRC += UnsignedInt_a_Checksum(&uTiempoSilenciarAlarma);
			Eeprom_a_UnsignedInt (&uChecksumAlarmas_Eeprom, &uCRC_Eeprom);									/* levanto el checksum de la eeprom */

			// si lei bien los originales grabo la copia sobre el backup
			if (uCRC == uCRC_Eeprom) {
				Float_a_Eeprom(&flValorAlarmaCond1_Eeprom_BK, &flValorAlarmaCond1);
				UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom_BK, &uTiempoAlarmaCond1);
				Float_a_Eeprom(&flValorAlarmaCond2_Eeprom_BK, &flValorAlarmaCond2);
				UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom_BK, &uTiempoAlarmaCond2);
				UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom_BK, &uTiempoSilenciarAlarma);
				UnsignedInt_a_Eeprom (&uChecksumAlarmas_Eeprom_BK, &uCRC);

				break;
			}
		}
		// si el original da mal y la copia bien
		else if ((uCRC != uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// grabo el backup sobre los originales
			Float_a_Eeprom(&flValorAlarmaCond1_Eeprom, &flValorAlarmaCond1);
			UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom, &uTiempoAlarmaCond1);
			Float_a_Eeprom(&flValorAlarmaCond2_Eeprom, &flValorAlarmaCond2);
			UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom, &uTiempoAlarmaCond2);
			UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom, &uTiempoSilenciarAlarma);
			UnsignedInt_a_Eeprom (&uChecksumAlarmas_Eeprom, &uCRC_BK);

			break;
		}
		// si ambas dan mal y agote los intentos
		else if (uContIntentosValidacionChecksum == CANT_INTENTOS_VALIDACION_CHECKSUM){
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			Carga_Configuracion_Fabrica_Eeprom (GRUPO_ALARMAS);
		}
	}


	/* GRUPO_ADC -2- */
	uContIntentosValidacionChecksum = 0;
	while (uContIntentosValidacionChecksum < CANT_INTENTOS_VALIDACION_CHECKSUM) {
		sprintf (chBufferTx_USART0, "\n\r%S %d", (wchar_t*)validacion_ADC_MSG, uContIntentosValidacionChecksum);
		Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

		uContIntentosValidacionChecksum++;

		// leo los valores originales
		Eeprom_a_UnsignedInt(&uMuestrasADCpwm1_Eeprom, &uMuestrasADCpwm1);
		uCRC = UnsignedInt_a_Checksum(&uMuestrasADCpwm1);
		Eeprom_a_Float(&flPWM1aMA_Eeprom, &flPWM1aMA);
		uCRC += Float_a_Checksum(&flPWM1aMA);
		Eeprom_a_UnsignedInt(&uMuestrasADCpwm2_Eeprom, &uMuestrasADCpwm2);
		uCRC += UnsignedInt_a_Checksum(&uMuestrasADCpwm2);
		Eeprom_a_Float(&flPWM2aMA_Eeprom, &flPWM2aMA);
		uCRC += Float_a_Checksum(&flPWM2aMA);
		Eeprom_a_UnsignedInt(&uRangoCond1_Eeprom, &uRangoCond1);
		uCRC += UnsignedInt_a_Checksum(&uRangoCond1);
		Eeprom_a_UnsignedInt(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
		uCRC += UnsignedInt_a_Checksum(&uMuestrasADCcond1);
		Eeprom_a_Float(&flCond1auS_Eeprom, &flCond1auS);
		uCRC += Float_a_Checksum(&flCond1auS);
		Eeprom_a_Float(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
		uCRC += Float_a_Checksum(&flOffsetADCcond1);
		Eeprom_a_UnsignedInt(&uRangoCond2_Eeprom, &uRangoCond2);
		uCRC += UnsignedInt_a_Checksum(&uRangoCond2);
		Eeprom_a_UnsignedInt(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
		uCRC += UnsignedInt_a_Checksum(&uMuestrasADCcond2);
		Eeprom_a_Float(&flCond2auS_Eeprom, &flCond2auS);
		uCRC += Float_a_Checksum(&flCond2auS);
		Eeprom_a_Float(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
		uCRC += Float_a_Checksum(&flOffsetADCcond2);
		Eeprom_a_UnsignedInt (&uChecksumADC_Eeprom, &uCRC_Eeprom);										/* levanto el checksum de la eeprom */

		// leo los valores de la copia
		Eeprom_a_UnsignedInt(&uMuestrasADCpwm1_Eeprom_BK, &uMuestrasADCpwm1);
		uCRC_BK = UnsignedInt_a_Checksum(&uMuestrasADCpwm1);
		Eeprom_a_Float(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
		uCRC_BK += Float_a_Checksum(&flPWM1aMA);
		Eeprom_a_UnsignedInt(&uMuestrasADCpwm2_Eeprom_BK, &uMuestrasADCpwm2);
		uCRC_BK += UnsignedInt_a_Checksum(&uMuestrasADCpwm2);
		Eeprom_a_Float(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
		uCRC_BK += Float_a_Checksum(&flPWM2aMA);
		Eeprom_a_UnsignedInt(&uRangoCond1_Eeprom_BK, &uRangoCond1);
		uCRC_BK += UnsignedInt_a_Checksum(&uRangoCond1);
		Eeprom_a_UnsignedInt(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
		uCRC_BK += UnsignedInt_a_Checksum(&uMuestrasADCcond1);
		Eeprom_a_Float(&flCond1auS_Eeprom_BK, &flCond1auS);
		uCRC_BK += Float_a_Checksum(&flCond1auS);
		Eeprom_a_Float(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
		uCRC_BK += Float_a_Checksum(&flOffsetADCcond1);
		Eeprom_a_UnsignedInt(&uRangoCond2_Eeprom_BK, &uRangoCond2);
		uCRC_BK += UnsignedInt_a_Checksum(&uRangoCond2);
		Eeprom_a_UnsignedInt(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
		uCRC_BK += UnsignedInt_a_Checksum(&uMuestrasADCcond2);
		Eeprom_a_Float(&flCond2auS_Eeprom_BK, &flCond2auS);
		uCRC_BK += Float_a_Checksum(&flCond2auS);
		Eeprom_a_Float(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
		uCRC_BK += Float_a_Checksum(&flOffsetADCcond2);
		Eeprom_a_UnsignedInt (&uChecksumADC_Eeprom_BK, &uCRC_Eeprom_BK);										/* levanto el checksum de la eeprom */

		// si ambas dan bien
		if ((uCRC == uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			break;
		}
		// si la copia da mal vuelvo a leer el original porque habia dado bien
		else if ((uCRC == uCRC_Eeprom) && (uCRC_BK != uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// leo los valores originales nuevamente
			Eeprom_a_UnsignedInt(&uMuestrasADCpwm1_Eeprom, &uMuestrasADCpwm1);
			uCRC = UnsignedInt_a_Checksum(&uMuestrasADCpwm1);
			Eeprom_a_Float(&flPWM1aMA_Eeprom, &flPWM1aMA);
			uCRC += Float_a_Checksum(&flPWM1aMA);
			Eeprom_a_UnsignedInt(&uMuestrasADCpwm2_Eeprom, &uMuestrasADCpwm2);
			uCRC += UnsignedInt_a_Checksum(&uMuestrasADCpwm2);
			Eeprom_a_Float(&flPWM2aMA_Eeprom, &flPWM2aMA);
			uCRC += Float_a_Checksum(&flPWM2aMA);
			Eeprom_a_UnsignedInt(&uRangoCond1_Eeprom, &uRangoCond1);
			uCRC += UnsignedInt_a_Checksum(&uRangoCond1);
			Eeprom_a_UnsignedInt(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
			uCRC += UnsignedInt_a_Checksum(&uMuestrasADCcond1);
			Eeprom_a_Float(&flCond1auS_Eeprom, &flCond1auS);
			uCRC += Float_a_Checksum(&flCond1auS);
			Eeprom_a_Float(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
			uCRC += Float_a_Checksum(&flOffsetADCcond1);
			Eeprom_a_UnsignedInt(&uRangoCond2_Eeprom, &uRangoCond2);
			uCRC += UnsignedInt_a_Checksum(&uRangoCond2);
			Eeprom_a_UnsignedInt(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
			uCRC += UnsignedInt_a_Checksum(&uMuestrasADCcond2);
			Eeprom_a_Float(&flCond2auS_Eeprom, &flCond2auS);
			uCRC += Float_a_Checksum(&flCond2auS);
			Eeprom_a_Float(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
			uCRC += Float_a_Checksum(&flOffsetADCcond2);
			Eeprom_a_UnsignedInt (&uChecksumADC_Eeprom, &uCRC_Eeprom);										/* levanto el checksum de la eeprom */

			// si lei bien los originales grabo la copia sobre el backup
			if (uCRC == uCRC_Eeprom) {
				UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom_BK, &uMuestrasADCpwm1);
				Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
				UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom_BK, &uMuestrasADCpwm2);
				Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
				UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
				UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
				Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
				Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
				UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
				UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
				Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
				Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);
				UnsignedInt_a_Eeprom(&uChecksumADC_Eeprom_BK, &uCRC);

				break;
			}
		}
		// si el original da mal y la copia bien
		else if ((uCRC != uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// grabo el backup sobre los originales
			UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom, &uMuestrasADCpwm1);
			Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
			UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom, &uMuestrasADCpwm2);
			Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
			UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
			UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
			Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
			Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
			UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
			UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
			Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
			Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);
			UnsignedInt_a_Eeprom(&uChecksumADC_Eeprom, &uCRC_BK);

			break;
		}
		// si ambas dan mal y agote los intentos
		else if (uContIntentosValidacionChecksum == CANT_INTENTOS_VALIDACION_CHECKSUM){
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			Carga_Configuracion_Fabrica_Eeprom (GRUPO_ADC);
		}
	}

	/* GRUPO_PWM -3- */
	uContIntentosValidacionChecksum = 0;
	while (uContIntentosValidacionChecksum <= CANT_INTENTOS_VALIDACION_CHECKSUM) {
		sprintf (chBufferTx_USART0, "\n\r%S %d", (wchar_t*)validacion_PWM_MSG, uContIntentosValidacionChecksum);
		Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

		uContIntentosValidacionChecksum++;

		// leo los valores originales
		Eeprom_a_UnsignedInt(&uGananciaPWM1_Eeprom, &uGananciaPWM1);							/* leeo los valores originales de la eeprom */
		uCRC = UnsignedInt_a_Checksum(&uGananciaPWM1);											/* calculo el checksum de los valores lebantados a la ram */
		Eeprom_a_UnsignedInt(&uGananciaPWM2_Eeprom, &uGananciaPWM2);
		uCRC += UnsignedInt_a_Checksum(&uGananciaPWM2);
		Eeprom_a_UnsignedInt (&uChecksumPWM_Eeprom, &uCRC_Eeprom);

		// leo los valores de la copia
		Eeprom_a_UnsignedInt(&uGananciaPWM1_Eeprom_BK, &uGananciaPWM1);
		uCRC_BK = UnsignedInt_a_Checksum(&uGananciaPWM1);
		Eeprom_a_UnsignedInt(&uGananciaPWM2_Eeprom_BK, &uGananciaPWM2);
		uCRC_BK += UnsignedInt_a_Checksum(&uGananciaPWM2);
		Eeprom_a_UnsignedInt (&uChecksumPWM_Eeprom_BK, &uCRC_Eeprom_BK);


		// si ambas dan bien
		if ((uCRC == uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			break;
		}
		// si la copia da mal vuelvo a leer el original porque habia dado bien
		else if ((uCRC == uCRC_Eeprom) && (uCRC_BK != uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesOk_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// leo los valores originales nuevamente
			Eeprom_a_UnsignedInt(&uGananciaPWM1_Eeprom, &uGananciaPWM1);							/* leeo los valores originales de la eeprom */
			uCRC = UnsignedInt_a_Checksum(&uGananciaPWM1);											/* calculo el checksum de los valores lebantados a la ram */
			Eeprom_a_UnsignedInt(&uGananciaPWM2_Eeprom, &uGananciaPWM2);
			uCRC += UnsignedInt_a_Checksum(&uGananciaPWM2);
			Eeprom_a_UnsignedInt (&uChecksumPWM_Eeprom, &uCRC_Eeprom);

			// si lei bien los originales grabo la copia sobre el backup
			if (uCRC == uCRC_Eeprom) {
				UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom_BK, &uGananciaPWM1);
				UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom_BK, &uGananciaPWM2);
				UnsignedInt_a_Eeprom (&uChecksumPWM_Eeprom_BK, &uCRC);

				break;
			}
		}
		// si el original da mal y la copia bien
		else if ((uCRC != uCRC_Eeprom) && (uCRC_BK == uCRC_Eeprom_BK)) {
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpOK_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			// grabo el backup sobre los originales
			UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom, &uGananciaPWM1);
			UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom, &uGananciaPWM2);
			UnsignedInt_a_Eeprom (&uChecksumPWM_Eeprom, &uCRC_BK);

			break;
		}
		// si ambas dan mal y agote los intentos
		else if (uContIntentosValidacionChecksum == CANT_INTENTOS_VALIDACION_CHECKSUM){
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			Carga_Configuracion_Fabrica_Eeprom (GRUPO_PWM);
		}
	}
}

/*****************************************************************************
* Funcion de carga de variables con valores de fabrica
*****************************************************************************/

void Carga_Configuracion_Fabrica_Eeprom (int grupo) {
	/////GRUPO GENERAL/////
	if (grupo == GRUPO_SERIE) {
		for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++){
			chNumeroSerie[i] = '\0';
		}

		for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++) {
			Char_a_Eeprom(chNumeroSerie_Eeprom+i, chNumeroSerie+i);										/* leeo los valores originales de la eeprom */
		}

		for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++) {
			Char_a_Eeprom(chNumeroSerie_Eeprom_BK+i, chNumeroSerie+i);										/* leeo los valores originales de la eeprom */
		}

		Escribir_Checksum_Eeprom (GRUPO_SERIE);
	}

	/////GRUPO ALARMAS/////
	if ((grupo == GRUPO_ALARMAS)||(grupo == GRUPO_TODOS)) {
		flValorAlarmaCond1 = DEFAULT_VALOR_ALARMA_COND_1;
		uTiempoAlarmaCond1 = DEFAULT_TIEMPO_ALARMA_COND_1;
		flValorAlarmaCond2 = DEFAULT_VALOR_ALARMA_COND_2;
		uTiempoAlarmaCond2 = DEFAULT_TIEMPO_ALARMA_COND_2;
		uTiempoSilenciarAlarma = DEFAULT_TIEMPO_SILENCIAR_ALARMA;

		Float_a_Eeprom(&flValorAlarmaCond1_Eeprom, &flValorAlarmaCond1);
		UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom, &uTiempoAlarmaCond1);
		Float_a_Eeprom(&flValorAlarmaCond2_Eeprom, &flValorAlarmaCond2);
		UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom, &uTiempoAlarmaCond2);
		UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom, &uTiempoSilenciarAlarma);

		Float_a_Eeprom(&flValorAlarmaCond1_Eeprom_BK, &flValorAlarmaCond1);
		UnsignedInt_a_Eeprom(&uTiempoAlarmaCond1_Eeprom_BK, &uTiempoAlarmaCond1);
		Float_a_Eeprom(&flValorAlarmaCond2_Eeprom_BK, &flValorAlarmaCond2);
		UnsignedInt_a_Eeprom(&uTiempoAlarmaCond2_Eeprom_BK, &uTiempoAlarmaCond2);
		UnsignedInt_a_Eeprom(&uTiempoSilenciarAlarma_Eeprom_BK, &uTiempoSilenciarAlarma);

		Escribir_Checksum_Eeprom (GRUPO_ALARMAS);
	}

	/////GRUPO ADC/////
	if ((grupo == GRUPO_ADC)||(grupo == GRUPO_TODOS)) {
		uMuestrasADCpwm1 = DEFAULT_MUESTRAS_ADC_PWM1;
		flPWM1aMA = DEFAULT_PWM1_A_MA;
		uMuestrasADCpwm2 = DEFAULT_MUESTRAS_ADC_PWM2;
		flPWM2aMA = DEFAULT_PWM2_A_MA;
		uRangoCond1 = DEFAULT_RANGO_COND_1;
		uMuestrasADCcond1 = DEFAULT_MUESTRAS_COND1;
		flCond1auS = DEFAULT_COND1_A_US;
		flOffsetADCcond1 = DEFAULT_OFFSET_COND1;
		uRangoCond2 = DEFAULT_RANGO_COND_2;
		uMuestrasADCcond2 = DEFAULT_MUESTRAS_COND2;
		flCond2auS = DEFAULT_COND2_A_US;
		flOffsetADCcond2 = DEFAULT_OFFSET_COND2;

		UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom, &uMuestrasADCpwm1);
		Float_a_Eeprom(&flPWM1aMA_Eeprom, &flPWM1aMA);
		UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom, &uMuestrasADCpwm2);
		Float_a_Eeprom(&flPWM2aMA_Eeprom, &flPWM2aMA);
		UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom, &uRangoCond1);
		UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom, &uMuestrasADCcond1);
		Float_a_Eeprom(&flCond1auS_Eeprom, &flCond1auS);
		Float_a_Eeprom(&flOffsetADCcond1_Eeprom, &flOffsetADCcond1);
		UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom, &uRangoCond2);
		UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom, &uMuestrasADCcond2);
		Float_a_Eeprom(&flCond2auS_Eeprom, &flCond2auS);
		Float_a_Eeprom(&flOffsetADCcond2_Eeprom, &flOffsetADCcond2);

		UnsignedInt_a_Eeprom(&uMuestrasADCpwm1_Eeprom_BK, &uMuestrasADCpwm1);
		Float_a_Eeprom(&flPWM1aMA_Eeprom_BK, &flPWM1aMA);
		UnsignedInt_a_Eeprom(&uMuestrasADCpwm2_Eeprom_BK, &uMuestrasADCpwm2);
		Float_a_Eeprom(&flPWM2aMA_Eeprom_BK, &flPWM2aMA);
		UnsignedInt_a_Eeprom(&uRangoCond1_Eeprom_BK, &uRangoCond1);
		UnsignedInt_a_Eeprom(&uMuestrasADCcond1_Eeprom_BK, &uMuestrasADCcond1);
		Float_a_Eeprom(&flCond1auS_Eeprom_BK, &flCond1auS);
		Float_a_Eeprom(&flOffsetADCcond1_Eeprom_BK, &flOffsetADCcond1);
		UnsignedInt_a_Eeprom(&uRangoCond2_Eeprom_BK, &uRangoCond2);
		UnsignedInt_a_Eeprom(&uMuestrasADCcond2_Eeprom_BK, &uMuestrasADCcond2);
		Float_a_Eeprom(&flCond2auS_Eeprom_BK, &flCond2auS);
		Float_a_Eeprom(&flOffsetADCcond2_Eeprom_BK, &flOffsetADCcond2);

		Escribir_Checksum_Eeprom (GRUPO_ADC);
	}

	/////GRUPO PWM/////
	if ((grupo == GRUPO_PWM)||(grupo == GRUPO_TODOS)) {
		uGananciaPWM1 = DEFAULT_GANACIA_LAZO_PWM_1;
		uGananciaPWM2 = DEFAULT_GANACIA_LAZO_PWM_2;

		UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom, &uGananciaPWM1);							/* leeo los valores originales de la eeprom */
		UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom, &uGananciaPWM2);

		UnsignedInt_a_Eeprom(&uGananciaPWM1_Eeprom_BK, &uGananciaPWM1);							/* leeo los valores originales de la eeprom */
		UnsignedInt_a_Eeprom(&uGananciaPWM2_Eeprom_BK, &uGananciaPWM2);

		Escribir_Checksum_Eeprom (GRUPO_PWM);
	}
}

/*****************************************************************************
* Actualiza en checksum del original y de la copia de la eeprom
*****************************************************************************/

void Escribir_Checksum_Eeprom (char chGrupo) {
	unsigned int uChecksum;

//	sprintf (chBufferTx_USART0, "\n\r%S %d\n\r", (wchar_t*)numeroGrupo_MSG, chGrupo);
//	Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

	if (chGrupo == GRUPO_SERIE) { /* -0- */
		uChecksum = 0;
		for (int i=0; i<NUMERO_SERIE_MAX_DIGITOS; i++)
			uChecksum += Char_a_Checksum (chNumeroSerie+i);

		UnsignedInt_a_Eeprom (&uChecksumGeneral_Eeprom, &uChecksum);
		UnsignedInt_a_Eeprom (&uChecksumGeneral_Eeprom_BK, &uChecksum);
	}
	else if (chGrupo == GRUPO_ALARMAS) { /* -1- */
		uChecksum = Float_a_Checksum (&flValorAlarmaCond1);
		uChecksum += UnsignedInt_a_Checksum (&uTiempoAlarmaCond1);
		uChecksum += Float_a_Checksum(&flValorAlarmaCond2);
		uChecksum += UnsignedInt_a_Checksum(&uTiempoAlarmaCond2);
		uChecksum += UnsignedInt_a_Checksum(&uTiempoSilenciarAlarma);

		UnsignedInt_a_Eeprom (&uChecksumAlarmas_Eeprom, &uChecksum);
		UnsignedInt_a_Eeprom (&uChecksumAlarmas_Eeprom_BK, &uChecksum);
	}
	else if (chGrupo == GRUPO_ADC) { /* -2- */
		uChecksum = UnsignedInt_a_Checksum (&uMuestrasADCpwm1);
		uChecksum += Float_a_Checksum(&flPWM1aMA);
		uChecksum += UnsignedInt_a_Checksum (&uMuestrasADCpwm2);
		uChecksum += Float_a_Checksum(&flPWM2aMA);
		uChecksum += UnsignedInt_a_Checksum (&uRangoCond1);
		uChecksum += UnsignedInt_a_Checksum (&uMuestrasADCcond1);
		uChecksum += Float_a_Checksum(&flCond1auS);
		uChecksum += Float_a_Checksum(&flOffsetADCcond1);
		uChecksum += UnsignedInt_a_Checksum (&uRangoCond2);
		uChecksum += UnsignedInt_a_Checksum (&uMuestrasADCcond2);
		uChecksum += Float_a_Checksum(&flCond2auS);
		uChecksum += Float_a_Checksum(&flOffsetADCcond2);

		UnsignedInt_a_Eeprom (&uChecksumADC_Eeprom, &uChecksum);
		UnsignedInt_a_Eeprom (&uChecksumADC_Eeprom_BK, &uChecksum);
	}
	else if (chGrupo == GRUPO_PWM) { /* -3- */
		uChecksum = UnsignedInt_a_Checksum (&uGananciaPWM1);
		uChecksum += UnsignedInt_a_Checksum (&uGananciaPWM2);

		UnsignedInt_a_Eeprom (&uChecksumPWM_Eeprom, &uChecksum);
		UnsignedInt_a_Eeprom (&uChecksumPWM_Eeprom_BK, &uChecksum);
	}
}

/*****************************************************************************
* Calcula el checksum de un float
*****************************************************************************/

unsigned int Float_a_Checksum (float *flDatoPtr) {
	wdt_reset();

	long *lDatoPtr = (long *) flDatoPtr;
	unsigned int CRC;

	CRC = (unsigned int)((*lDatoPtr >> 24) & BYTE_MASK);
	CRC += (unsigned int)((*lDatoPtr >> 16) & BYTE_MASK);
	CRC += (unsigned int)((*lDatoPtr >> 8) & BYTE_MASK);
	CRC += (unsigned int)((*lDatoPtr) & BYTE_MASK);

	return CRC;
}

/*****************************************************************************
* Calcula el checksum de un unsigned int
*****************************************************************************/

unsigned int UnsignedInt_a_Checksum (unsigned int *uDatoPtr) {
	wdt_reset();

	unsigned int CRC;

	CRC = (*uDatoPtr >> 8) & BYTE_MASK;
	CRC += *uDatoPtr & BYTE_MASK;

	return CRC;
}

/*****************************************************************************
* Calcula el checksum de un char
*****************************************************************************/

unsigned int Char_a_Checksum (char *chDatoPtr) {
	wdt_reset();

	unsigned int *uDatoPtr = (unsigned int *) chDatoPtr;
	unsigned int CRC;

	CRC = *uDatoPtr & BYTE_MASK;

	return CRC;
}

/*****************************************************************************
* Funcion de escritura de un float a la eeprom
*****************************************************************************/

void Float_a_Eeprom (float *flEepromPtr, float *flDatoEscritoPtr) {
	int i = 0;
	float flDatoLeido;

	while (i < CANT_INTENTOS_ESCRITURA_EEPROM) {
		wdt_reset();
		eeprom_busy_wait ();
		cli();
		eeprom_write_float (flEepromPtr, *flDatoEscritoPtr);
		sei();
		eeprom_busy_wait ();
		cli();
		flDatoLeido = eeprom_read_float (flEepromPtr);
		sei();
		(flDatoLeido == *flDatoEscritoPtr) ? (i = CANT_INTENTOS_ESCRITURA_EEPROM + 1) : (i ++);
	}

	EEAR = (uint16_t) &flDireccionVaciaEeprom;
}

/*****************************************************************************
* Funcion de lectura de un float de la eeprom
*****************************************************************************/

void Eeprom_a_Float (float *flEepromPtr, float *flDatoLeidoPtr) {
	int i = 0;
	float flDatoLeido2;

	while (i < CANT_INTENTOS_LECTURA_EEPROM) {
		wdt_reset();
		eeprom_busy_wait ();
		cli();
		*flDatoLeidoPtr = eeprom_read_float (flEepromPtr);
		sei();
		eeprom_busy_wait ();
		cli();
		flDatoLeido2 = eeprom_read_float (flEepromPtr);
		sei();
		(*flDatoLeidoPtr == flDatoLeido2) ? (i = CANT_INTENTOS_ESCRITURA_EEPROM + 1) : (i ++);
	}

	EEAR = (uint16_t) &flDireccionVaciaEeprom;
}

/*****************************************************************************
* Funcion de escritura de un unsigned int a la eeprom
*****************************************************************************/

void UnsignedInt_a_Eeprom (unsigned int *uEepromPtr, unsigned int *uDatoEscritoPtr) {
	int i = 0;
	unsigned int uDatoLeido;

	while (i < CANT_INTENTOS_ESCRITURA_EEPROM) {
		wdt_reset();
		eeprom_busy_wait ();
		cli();
		eeprom_write_word ((uint16_t *) uEepromPtr, *uDatoEscritoPtr);
		sei();
		eeprom_busy_wait ();
		cli();
		uDatoLeido = eeprom_read_word ((uint16_t *) uEepromPtr);
		sei();
		(uDatoLeido == *uDatoEscritoPtr) ? (i = CANT_INTENTOS_ESCRITURA_EEPROM + 1) : (i ++);
	}

	EEAR = (uint16_t) &flDireccionVaciaEeprom;
}

/*****************************************************************************
* Funcion de lectura de un unsigned int a la eeprom
*****************************************************************************/

void Eeprom_a_UnsignedInt (unsigned int *uEepromPtr, unsigned int *uDatoLeidoPtr) {
	int i = 0;
	unsigned int uDatoLeido2;

	while (i < CANT_INTENTOS_ESCRITURA_EEPROM) {
		wdt_reset();
		eeprom_busy_wait ();
		cli();
		*uDatoLeidoPtr = eeprom_read_word ((uint16_t *) uEepromPtr);
		sei();
		eeprom_busy_wait ();
		cli();
		uDatoLeido2 = eeprom_read_word ((uint16_t *) uEepromPtr);
		sei();
		(*uDatoLeidoPtr == uDatoLeido2) ? (i = CANT_INTENTOS_ESCRITURA_EEPROM + 1) : (i ++);
	}

	EEAR = (uint16_t) &flDireccionVaciaEeprom;
}

/*****************************************************************************
* Funcion de escritura de un char a la eeprom
*****************************************************************************/

void Char_a_Eeprom (char *chEepromPtr, char *chDatoEscritoPtr) {
	int i = 0;
	char chDatoLeido;

	while (i < CANT_INTENTOS_ESCRITURA_EEPROM) {
		wdt_reset();
		eeprom_busy_wait ();
		cli();
		eeprom_write_byte ((uint8_t *) chEepromPtr, *chDatoEscritoPtr);
		sei();
		eeprom_busy_wait ();
		cli();
		chDatoLeido = eeprom_read_byte ((uint8_t *) chEepromPtr);
		sei();
		(chDatoLeido == *chDatoEscritoPtr) ? (i = CANT_INTENTOS_ESCRITURA_EEPROM + 1) : (i ++);
	}

	EEAR = (uint16_t) &flDireccionVaciaEeprom;
}

/*****************************************************************************
* Funcion de lectura de un char de la eeprom
*****************************************************************************/

void Eeprom_a_Char (char *chEepromPtr, char *chDatoLeidoPtr) {
	int i = 0;
	char chDatoLeido2;

	while (i < CANT_INTENTOS_ESCRITURA_EEPROM) {
		wdt_reset();
		eeprom_busy_wait ();
		cli();
		*chDatoLeidoPtr = eeprom_read_byte ((uint8_t *) chEepromPtr);
		sei();
		eeprom_busy_wait ();
		cli();
		chDatoLeido2 = eeprom_read_byte ((uint8_t *) chEepromPtr);
		sei();
		(*chDatoLeidoPtr == chDatoLeido2) ? (i = CANT_INTENTOS_ESCRITURA_EEPROM + 1) : (i ++);
	}

	EEAR = (uint16_t) &flDireccionVaciaEeprom;
}
