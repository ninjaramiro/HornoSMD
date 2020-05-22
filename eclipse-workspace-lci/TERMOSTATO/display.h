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
void Actualizar_Display (void);
void Inicio_Display (void);
void Standby_Display(void);
void Menu_Ppal_Display(void);
void Fase_Temp_Display(void);
void Manual_Display(void);
void Manual_Control_Display(void);

/*****************************************************************************
* Definiciones máquina de estado
*****************************************************************************/

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define PERIODO_ACTUALIZACION_DISPLAY	100		/**< \brief periodo de actualizacion de display */
#define TIEMPO_PRESENTACION				3000	/**< \brief tiempo en que va la pantalla de presentacion */
#define TIEMPO_DISPLAY					1000

#endif /* DISPLAY_H_ */
