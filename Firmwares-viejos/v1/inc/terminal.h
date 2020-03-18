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

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/

#define	TERMINAL_STANDBY		0
#define TERMINAL_PROCESO		1

#endif /* TERMINAL_H_ */
