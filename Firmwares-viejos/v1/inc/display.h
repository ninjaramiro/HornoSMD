/** \file	display.h
 * 	\author Abel N. Dammiani
 *
 *  \brief Contiene la maquina de estados correspondiente a la actualizacion del display.
 *
 */

#ifndef DISPLAY_H_
#define DISPLAY_H_

/*****************************************************************************
* Definicion de los prototipos de las funciones
*****************************************************************************/
/** \brief	Maquina de estados de actualizacion del display.
 *
 */
void Actualizar_Display (void);

/** \brief	Funcion de inicio del Display.
 *
 */
void Inicio_Display (void);

/** \brief	Funcion que muestra la pantalla de presentación.
 *	Utiliza la interrupcion periodica.
 */
void Pantalla_Presentacion_Display (void);

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/
#define DISPLAY_STANDBY			0
#define DISPLAY_ACTUALIZAR_L1	1
#define DISPLAY_ACTUALIZAR_L2	2

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define PERIODO_ACTUALIZACION_DISPLAY	100		/**< \brief periodo de actualizacion de display */
#define TIEMPO_PUNTOS_PRESENTACION		500		/**< \brief tiempo en que va cargando los puntos en pantalla durante la presentacion */
#define TIEMPO_DISPLAY_ON				1000
#define TIEMPO_DISPLAY_OFF				500

#endif /* DISPLAY_H_ */
