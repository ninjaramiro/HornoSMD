/** \file	utiles.c
 *  \brief	Contiene funciones de uso general.
 */

#include "macros.h"
#include "utiles.h"

#include <avr/pgmspace.h>

/**************************************************************************
* Funcion que busca un string de memoria de programa dentro de uno en RAM
***************************************************************************/
char Buscar_en_Buffer_P (const char *chBufferOriginalPtr, char *chBufferRecibidoPtr, int bufferRecibidoZise) {

	char chError=ERROR;
	int i = 0, j = 0;

	while ((*(chBufferRecibidoPtr+i) != '\0') && (i < bufferRecibidoZise) && (pgm_read_byte(chBufferOriginalPtr+j) != '\0')) {	// Comparo lo recibido

		if (pgm_read_byte(chBufferOriginalPtr+j) != *(chBufferRecibidoPtr+i)) {				// verifico que coincidan
			chError = ERROR;
			j = 0;
		}
		else {
			j++;
			if (pgm_read_byte(chBufferOriginalPtr+j) == '\0'){
				chError = OK;
			}
		}

		i++;
	}
	return chError;
}

/**************************************************************************
* Funcion que busca un string de memoria ram dentro de uno en RAM
***************************************************************************/
char Buscar_en_Buffer (char *chBufferOriginalPtr, char *chBufferRecibidoPtr, int bufferRecibidoZise) {

	char chError=ERROR;
	int i = 0, j = 0;

	while ((*(chBufferRecibidoPtr+i) != '\0') && (i < bufferRecibidoZise-1) && (*(chBufferOriginalPtr+j) != '\0')) {	// Comparo lo recibido

		if (*(chBufferOriginalPtr+j) != *(chBufferRecibidoPtr+i)) {				// verifico que coincidan
			chError = ERROR;
			j = 0;
		}
		else {
			j++;
			if (*(chBufferOriginalPtr+j) == '\0'){
				chError = OK;
			}
		}

		i++;
	}
	return chError;
}

/************************************************************************
* Funcion que reconoce el inicio de un mesaje recibido
************************************************************************/
char Comparar_Inicio_Buffer_P (const char *chBufferOriginalPtr, char *chBufferRecibidoPtr) {

	char chError = OK;
	int i = 0, j = 0;

	while ((*(chBufferRecibidoPtr+j) == '\n') || (*(chBufferRecibidoPtr+j) == '\r'))	// si lo recibido comienza con \n o \r lo salto
		j++;

	while (pgm_read_byte(chBufferOriginalPtr+i) != '\0') {								// Comparo lo recibido

		if (pgm_read_byte(chBufferOriginalPtr+i) != *(chBufferRecibidoPtr+j)) {			// verifico que coincidan
			chError = ERROR;
			break;
		}

		i++;
		j++;
	}
	return chError;
}

/************************************************************************
* Funcion que reconoce el inicio de un mesaje recibido
************************************************************************/
char Comparar_Inicio_Buffer (char *chBufferOriginalPtr, char *chBufferRecibidoPtr) {

	char chError = OK;
	int i = 0, j = 0;

	while ((*(chBufferRecibidoPtr+j) == '\n') || (*(chBufferRecibidoPtr+j) == '\r'))	// si lo recibido comienza con \n o \r lo salto
		j++;

	while (*(chBufferOriginalPtr+i) != '\0') {											// Comparo lo recibido

		if (*(chBufferOriginalPtr+i) != *(chBufferRecibidoPtr+j)) {						// verifico que coincidan
			chError = ERROR;
			break;
		}

		i++;
		j++;
	}
	return chError;
}

/************************************************************************
* Funcion que conviertes un string a mayusculas
************************************************************************/

int String_a_Mayusculas (char * chStringPtr, int maxStringSize) {
	int i = 0;

	while ((*(chStringPtr+i) != '\0') && (i<maxStringSize)) {
		if ((*(chStringPtr+i) >= 'a' ) && (*(chStringPtr+i) <= 'z'))
			*(chStringPtr+i) -= 32;
		else if (*(chStringPtr+i) == 164 )	// ñ -> Ñ
			*(chStringPtr+i) += 1;
		i++;
	}

	return i;
}

/**************************************************************************
* Funcion que extrae un float de un string en RAM
***************************************************************************/
float From_String_to_Float (char *chBuffer, int maxBufferZise) {
	float flValor = (float)atoi(chBuffer);
	int index = 0;
	while ((*(chBuffer+index) != '.') && (*(chBuffer+index) != '\0') && (index<maxBufferZise))
		index++;

	if(*(chBuffer+index) == '.') {
		index++;
		int j=10;
		while ((*(chBuffer+index) >= '0') && (*(chBuffer+index) <= '9') && (index<maxBufferZise)) {
			flValor += (float)(*(chBuffer+index) - '0')/j;
			index++;
			j*=10;
		}
	}
	return flValor;
}
