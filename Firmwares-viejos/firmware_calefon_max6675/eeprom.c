
/** \file	eeprom.c
 *  \brief	Contiene las funciones para el manejo de datos desde y hacia la eeprom.
 */

#include "eeprom.h"
#include "macros.h"
#include "USART0.h"
#include "control.h"
#include "terminal.h"
#include "caudales.h"

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

/* GRUPO_PID */
extern float				flPIDkP;
extern float				flPIDkI;
extern float				flPIDkD;
extern float				flPIDintMax;
extern float				flVariacionCaudalSetP;					/**< \brief porcentaje para el cual si varia el caudal corrijo fuera del PID */
extern float 				flCaudalEscalaKsPID;					/**< \brief valor de caudal para el cual se escala los valores de k del PID */
extern float 				flEscalaVariacionCaudal;				/**< \brief factor de escala para el movimiento de la valvula con el cambio brusco de caudal */
extern unsigned int			uPeriodoPID;							/**< \brief periodo de actualizacion del PID */
extern unsigned int 		uPeriodoVedaPID;
extern unsigned int 		uPeriodoVedaCaudal;

/*****************************************************************************
* Variables en eeprom
*****************************************************************************/
/* asigno para cada veriable un lugar en la eeprom  y le doy un valor por defecto para el inicio depues de grabar el uc */

/* GRUPO_PID */
float			flPIDkP_Eeprom						EEMEM;
float			flPIDkI_Eeprom						EEMEM;
float			flPIDkD_Eeprom						EEMEM;
float			flPIDintMax_Eeprom					EEMEM;
float			flVariacionCaudalSetP_Eeprom		EEMEM;
float 			flCaudalEscalaKsPID_Eeprom			EEMEM;
float 			flEscalaVariacionCaudal_Eeprom		EEMEM;
unsigned int	uPeriodoPID_Eeprom					EEMEM;
unsigned int 	uPeriodoVedaPID_Eeprom				EEMEM;
unsigned int 	uPeriodoVedaCaudal_Eeprom			EEMEM;
unsigned int	uChecksumPID_Eeprom					EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

float			flPIDkP_Eeprom_BK					EEMEM;
float			flPIDkI_Eeprom_BK					EEMEM;
float			flPIDkD_Eeprom_BK					EEMEM;
float			flPIDintMax_Eeprom_BK				EEMEM;
float			flVariacionCaudalSetP_Eeprom_BK		EEMEM;
float 			flCaudalEscalaKsPID_Eeprom_BK		EEMEM;
float 			flEscalaVariacionCaudal_Eeprom_BK	EEMEM;
unsigned int	uPeriodoPID_Eeprom_BK				EEMEM;
unsigned int 	uPeriodoVedaPID_Eeprom_BK			EEMEM;
unsigned int 	uPeriodoVedaCaudal_Eeprom_BK		EEMEM;
unsigned int	uChecksumPID_Eeprom_BK				EEMEM = CHECKSUM_EEPROM;	/**< \brief checksum de datos en eeprom, lo inicializo con basura */

/*-----------------*/
float			flDireccionVaciaEeprom				EEMEM;						/**< \brief dirección vacia en la eeprom por si hay un error y se modifica algo, siempre dejo apuntado a ella */

/*****************************************************************************
* Mensajes de debug guardados en memoria de programa
*****************************************************************************/
const char validacion_PID_MSG[]			PROGMEM = "Validacion GRUPO_PID (#0):\0";
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

	/* GRUPO_ALARMAS -0- */
	unsigned int uContIntentosValidacionChecksum = 0;
	while (uContIntentosValidacionChecksum < CANT_INTENTOS_VALIDACION_CHECKSUM) {
		sprintf (chBufferTx_USART0, "\n\r%S %d", (wchar_t*)validacion_PID_MSG, uContIntentosValidacionChecksum);
		Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

		uContIntentosValidacionChecksum++;

		// leo los valores originales
		Eeprom_a_Float(&flPIDkP_Eeprom, &flPIDkP);						/* leo los valores originales de la eeprom */
		uCRC = Float_a_Checksum(&flPIDkP);								/* calculo el checksum de los valores levantados a la ram */
		Eeprom_a_Float(&flPIDkI_Eeprom, &flPIDkI);						/* leo los valores originales de la eeprom */
		uCRC += Float_a_Checksum(&flPIDkI);								/* calculo el checksum de los valores levantados a la ram */
		Eeprom_a_Float(&flPIDkD_Eeprom, &flPIDkD);						/* leo los valores originales de la eeprom */
		uCRC += Float_a_Checksum(&flPIDkD);								/* calculo el checksum de los valores levantados a la ram */
		Eeprom_a_Float(&flPIDintMax_Eeprom, &flPIDintMax);				/* leo los valores originales de la eeprom */
		uCRC += Float_a_Checksum(&flPIDintMax);							/* calculo el checksum de los valores levantados a la ram */
		Eeprom_a_Float(&flVariacionCaudalSetP_Eeprom, &flVariacionCaudalSetP);
		uCRC += Float_a_Checksum(&flVariacionCaudalSetP);
		Eeprom_a_Float(&flCaudalEscalaKsPID_Eeprom, &flCaudalEscalaKsPID);
		uCRC += Float_a_Checksum(&flCaudalEscalaKsPID);
		Eeprom_a_Float(&flEscalaVariacionCaudal_Eeprom, &flEscalaVariacionCaudal);
		uCRC += Float_a_Checksum(&flEscalaVariacionCaudal);
		Eeprom_a_UnsignedInt(&uPeriodoPID_Eeprom, &uPeriodoPID);
		uCRC += UnsignedInt_a_Checksum(&uPeriodoPID);
		Eeprom_a_UnsignedInt(&uPeriodoVedaPID_Eeprom, &uPeriodoVedaPID);
		uCRC += UnsignedInt_a_Checksum(&uPeriodoVedaPID);
		Eeprom_a_UnsignedInt(&uPeriodoVedaCaudal_Eeprom, &uPeriodoVedaCaudal);
		uCRC += UnsignedInt_a_Checksum(&uPeriodoVedaCaudal);
		Eeprom_a_UnsignedInt (&uChecksumPID_Eeprom, &uCRC_Eeprom);		/* levanto el checksum de la eeprom */

		// leo los valores de la copia
		Eeprom_a_Float(&flPIDkP_Eeprom_BK, &flPIDkP);
		uCRC_BK = Float_a_Checksum(&flPIDkP);
		Eeprom_a_Float(&flPIDkI_Eeprom_BK, &flPIDkI);
		uCRC_BK += Float_a_Checksum(&flPIDkI);
		Eeprom_a_Float(&flPIDkD_Eeprom_BK, &flPIDkD);
		uCRC_BK += Float_a_Checksum(&flPIDkD);
		Eeprom_a_Float(&flPIDintMax_Eeprom_BK, &flPIDintMax);
		uCRC_BK += Float_a_Checksum(&flPIDintMax);
		Eeprom_a_Float(&flVariacionCaudalSetP_Eeprom_BK, &flVariacionCaudalSetP);
		uCRC_BK += Float_a_Checksum(&flVariacionCaudalSetP);
		Eeprom_a_Float(&flCaudalEscalaKsPID_Eeprom_BK, &flCaudalEscalaKsPID);
		uCRC_BK += Float_a_Checksum(&flCaudalEscalaKsPID);
		Eeprom_a_Float(&flEscalaVariacionCaudal_Eeprom_BK, &flEscalaVariacionCaudal);
		uCRC_BK += Float_a_Checksum(&flEscalaVariacionCaudal);
		Eeprom_a_UnsignedInt(&uPeriodoPID_Eeprom_BK, &uPeriodoPID);
		uCRC_BK += UnsignedInt_a_Checksum(&uPeriodoPID);
		Eeprom_a_UnsignedInt(&uPeriodoVedaPID_Eeprom_BK, &uPeriodoVedaPID);
		uCRC_BK += UnsignedInt_a_Checksum(&uPeriodoVedaPID);
		Eeprom_a_UnsignedInt(&uPeriodoVedaCaudal_Eeprom_BK, &uPeriodoVedaCaudal);
		uCRC_BK += UnsignedInt_a_Checksum(&uPeriodoVedaCaudal);
		Eeprom_a_UnsignedInt (&uChecksumPID_Eeprom_BK, &uCRC_Eeprom_BK);

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
			Eeprom_a_Float(&flPIDkP_Eeprom, &flPIDkP);						/* leo los valores originales de la eeprom */
			uCRC = Float_a_Checksum(&flPIDkP);								/* calculo el checksum de los valores levantados a la ram */
			Eeprom_a_Float(&flPIDkI_Eeprom, &flPIDkI);						/* leo los valores originales de la eeprom */
			uCRC += Float_a_Checksum(&flPIDkI);								/* calculo el checksum de los valores levantados a la ram */
			Eeprom_a_Float(&flPIDkD_Eeprom, &flPIDkD);						/* leo los valores originales de la eeprom */
			uCRC += Float_a_Checksum(&flPIDkD);								/* calculo el checksum de los valores levantados a la ram */
			Eeprom_a_Float(&flPIDintMax_Eeprom, &flPIDintMax);				/* leo los valores originales de la eeprom */
			uCRC += Float_a_Checksum(&flPIDintMax);							/* calculo el checksum de los valores levantados a la ram */
			Eeprom_a_Float(&flVariacionCaudalSetP_Eeprom, &flVariacionCaudalSetP);
			uCRC += Float_a_Checksum(&flVariacionCaudalSetP);
			Eeprom_a_Float(&flCaudalEscalaKsPID_Eeprom, &flCaudalEscalaKsPID);
			uCRC += Float_a_Checksum(&flCaudalEscalaKsPID);
			Eeprom_a_Float(&flEscalaVariacionCaudal_Eeprom, &flEscalaVariacionCaudal);
			uCRC += Float_a_Checksum(&flEscalaVariacionCaudal);
			Eeprom_a_UnsignedInt(&uPeriodoPID_Eeprom, &uPeriodoPID);
			uCRC += UnsignedInt_a_Checksum(&uPeriodoPID);
			Eeprom_a_UnsignedInt(&uPeriodoVedaPID_Eeprom, &uPeriodoVedaPID);
			uCRC += UnsignedInt_a_Checksum(&uPeriodoVedaPID);
			Eeprom_a_UnsignedInt(&uPeriodoVedaCaudal_Eeprom, &uPeriodoVedaCaudal);
			uCRC += UnsignedInt_a_Checksum(&uPeriodoVedaCaudal);
			Eeprom_a_UnsignedInt (&uChecksumPID_Eeprom, &uCRC_Eeprom);		/* levanto el checksum de la eeprom */

			// si lei bien los originales grabo la copia sobre el backup
			if (uCRC == uCRC_Eeprom) {
				Float_a_Eeprom(&flPIDkP_Eeprom_BK, &flPIDkP);
				Float_a_Eeprom(&flPIDkI_Eeprom_BK, &flPIDkI);
				Float_a_Eeprom(&flPIDkD_Eeprom_BK, &flPIDkD);
				Float_a_Eeprom(&flPIDintMax_Eeprom_BK, &flPIDintMax);
				Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom_BK, &flVariacionCaudalSetP);
				Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom_BK, &flCaudalEscalaKsPID);
				Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom_BK, &flEscalaVariacionCaudal);
				UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom_BK, &uPeriodoPID);
				UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom_BK, &uPeriodoVedaPID);
				UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom_BK, &uPeriodoVedaCaudal);
				UnsignedInt_a_Eeprom (&uChecksumPID_Eeprom_BK, &uCRC);

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
			Float_a_Eeprom(&flPIDkP_Eeprom, &flPIDkP);
			Float_a_Eeprom(&flPIDkI_Eeprom, &flPIDkI);
			Float_a_Eeprom(&flPIDkD_Eeprom, &flPIDkD);
			Float_a_Eeprom(&flPIDintMax_Eeprom, &flPIDintMax);
			Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom, &flVariacionCaudalSetP);
			Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom, &flCaudalEscalaKsPID);
			Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom, &flEscalaVariacionCaudal);
			UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom, &uPeriodoPID);
			UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom, &uPeriodoVedaPID);
			UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom, &uPeriodoVedaCaudal);
			UnsignedInt_a_Eeprom (&uChecksumPID_Eeprom, &uCRC_BK);

			break;
		}
		// si ambas dan mal y agote los intentos
		else if (uContIntentosValidacionChecksum == CANT_INTENTOS_VALIDACION_CHECKSUM){
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosOriginalesError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);
			sprintf (chBufferTx_USART0, "\n\r%S", (wchar_t*)datosBackUpError_MSG);
			Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

			Carga_Configuracion_Fabrica_Eeprom (GRUPO_PID);
		}
	}
}

/*****************************************************************************
* Funcion de carga de variables con valores de fabrica
*****************************************************************************/

void Carga_Configuracion_Fabrica_Eeprom (int grupo) {
	/////GRUPO ALARMAS/////
	if ((grupo == GRUPO_PID)||(grupo == GRUPO_TODOS)) {
		flPIDkP = DEFAULT_PID_KP;
		flPIDkI = DEFAULT_PID_KI;
		flPIDkD = DEFAULT_PID_KD;
		flPIDintMax = DEFAULT_MAX_PID_INTEG;
		flVariacionCaudalSetP = DEFAULT_VARIACION_CAUDAL;
		flCaudalEscalaKsPID = DEFAULT_CAUDAL_ESCALA_KS_PID;
		flEscalaVariacionCaudal = DEFAULT_ESCALA_VARIACION_CAUDAL;
		uPeriodoPID = DEFAULT_PID_PERIODO;
		uPeriodoVedaPID = DEFAULT_PERIODO_VEDA_PID;
		uPeriodoVedaCaudal = DEFAULT_PERIODO_VEDA_CAUDAL;

		Float_a_Eeprom(&flPIDkP_Eeprom, &flPIDkP);
		Float_a_Eeprom(&flPIDkI_Eeprom, &flPIDkI);
		Float_a_Eeprom(&flPIDkD_Eeprom, &flPIDkD);
		Float_a_Eeprom(&flPIDintMax_Eeprom, &flPIDintMax);
		Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom, &flVariacionCaudalSetP);
		Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom, &flCaudalEscalaKsPID);
		Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom, &flEscalaVariacionCaudal);
		UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom, &uPeriodoPID);
		UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom, &uPeriodoVedaPID);
		UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom, &uPeriodoVedaCaudal);

		Float_a_Eeprom(&flPIDkP_Eeprom_BK, &flPIDkP);
		Float_a_Eeprom(&flPIDkI_Eeprom_BK, &flPIDkI);
		Float_a_Eeprom(&flPIDkD_Eeprom_BK, &flPIDkD);
		Float_a_Eeprom(&flPIDintMax_Eeprom_BK, &flPIDintMax);
		Float_a_Eeprom(&flVariacionCaudalSetP_Eeprom_BK, &flVariacionCaudalSetP);
		Float_a_Eeprom(&flCaudalEscalaKsPID_Eeprom_BK, &flCaudalEscalaKsPID);
		Float_a_Eeprom(&flEscalaVariacionCaudal_Eeprom_BK, &flEscalaVariacionCaudal);
		UnsignedInt_a_Eeprom(&uPeriodoPID_Eeprom_BK, &uPeriodoPID);
		UnsignedInt_a_Eeprom(&uPeriodoVedaPID_Eeprom_BK, &uPeriodoVedaPID);
		UnsignedInt_a_Eeprom(&uPeriodoVedaCaudal_Eeprom_BK, &uPeriodoVedaCaudal);

		Escribir_Checksum_Eeprom (GRUPO_PID);
	}
}

/*****************************************************************************
* Actualiza en checksum del original y de la copia de la eeprom
*****************************************************************************/

void Escribir_Checksum_Eeprom (char chGrupo) {
	unsigned int uChecksum;

//	sprintf (chBufferTx_USART0, "\n\r%S %d\n\r", (wchar_t*)numeroGrupo_MSG, chGrupo);
//	Transmitir_Buffer_USART0 (chBufferTx_USART0, USART0_BUFFER_ZISE);

	if ((chGrupo == GRUPO_PID)||(chGrupo == GRUPO_TODOS)) { /* -0- */
		uChecksum = Float_a_Checksum (&flPIDkP);
		uChecksum += Float_a_Checksum(&flPIDkI);
		uChecksum += Float_a_Checksum(&flPIDkD);
		uChecksum += Float_a_Checksum(&flPIDintMax);
		uChecksum += Float_a_Checksum(&flVariacionCaudalSetP);
		uChecksum += Float_a_Checksum(&flEscalaVariacionCaudal);
		uChecksum += Float_a_Checksum(&flCaudalEscalaKsPID);
		uChecksum += UnsignedInt_a_Checksum(&uPeriodoPID);
		uChecksum += UnsignedInt_a_Checksum(&uPeriodoVedaPID);
		uChecksum += UnsignedInt_a_Checksum(&uPeriodoVedaCaudal);

		UnsignedInt_a_Eeprom (&uChecksumPID_Eeprom, &uChecksum);
		UnsignedInt_a_Eeprom (&uChecksumPID_Eeprom_BK, &uChecksum);
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
