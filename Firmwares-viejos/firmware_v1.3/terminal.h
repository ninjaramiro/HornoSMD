/** \file	termianl.h
 *  \brief	Contiene los prototipos de las maquinas de estados correspondiente al manejo del modulo GSM.
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

/*****************************************************************************
* Prototipos
*****************************************************************************/
/** \brief	Máquina de estados de la terminal.
 */
void Terminal(void);

/** \brief	Inicio de la terminal.
 */
void Inicio_Terminal (void);

/** \brief	Funcion que reconoce un comando en el inicio de un mesaje recibido y modifica el index de la terminal.
 *
 * \param[in] const char *chBufferOriginalPtr, puntero al string en la memoria de programa.
 * \param[in] char *chBufferRecibidolPtr, puntero al string en la memoria RAM.
 * \return, devuelve en el indice donde termina el texto hallado, si no hay conincidencia devuelve 0..
 */
int Detectar_Comando_Buffer_P (const char *chBufferOriginalPtr, char *chBufferRecibidolPtr);

/*****************************************************************************
* Definiciones varias
*****************************************************************************/

#define NUMERO_SERIE_MAX_DIGITOS	6

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/

#define	TERMINAL_STANDBY			0
#define TERMINAL_PROCESO			1

#endif /* TERMINAL_H_ */
