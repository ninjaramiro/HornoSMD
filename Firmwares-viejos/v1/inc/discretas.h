/** \file	discretas.h
 *  \brief	Contiene los prototipos correspondientes a las funciones de de lectura de las entradas discretas \n
 */

#ifndef DISCRETAS_H_
#define DISCRETAS_H_

/*****************************************************************************
* Prototipos
*****************************************************************************/

/** \brief	funcion que lee las entradas discretas por medio de una maquina de estados
 */
void Leer_Entradas (void);

/** \brief funcion de inicio de la maquina de estado de lectura de entradas discretas.
 */
void Inicio_Entradas (void);

/*****************************************************************************
* Definicion de estados
*****************************************************************************/

#define BARRER_ENTRADAS			1
#define ENTRADA_TECLADO_1		2
#define ENTRADA_TECLADO_2		3
#define ENTRADA_TECLADO_3		4
#define ENTRADA_TECLADO_4		5
#define ENTRADA_TECLADO_5		6
#define ENTRADA_TECLADO_6		7
#define IMPRIMIR_DISCRETAS		8

/*****************************************************************************
* Definicion de entradas
*****************************************************************************/

#define BOTON_TECLADO_1			0
#define BOTON_TECLADO_2			1
#define BOTON_TECLADO_3			2
#define BOTON_TECLADO_4			3
#define BOTON_TECLADO_5			4
#define BOTON_TECLADO_6			5

/*****************************************************************************
* Definiciones varias
*****************************************************************************/

#define CANTIDAD_ENTRADAS					6				/**< \brief cantidad de entradas */

#define TIEMPO_INTEGRACION_ENTRADAS			50				/**< \brief Tiempo de integracion en periodos del Timer 2 para la deteccion de las entradas discretas */
#define VALOR_INTEGRACION_ENTRADAS			30				/**< \brief Valor de tiempo de integracion en periodos del Timer 2 para la deteccion de las entradas discretas */
#define TIMEOUT_ENTRADAS					10000			/**< \brief timeout para la lectura y borrado de los flag de los botones detectados */

#endif /* DISCRETAS_H_ */
