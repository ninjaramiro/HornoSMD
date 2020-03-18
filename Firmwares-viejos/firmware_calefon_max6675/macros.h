/** \file	macros.h
 *  \brief	Contiene macros de uso comun.
 *
 *	Contiene macros para activar o desactivar pines, definir direcciones de puertos, \n
 *	y ademas definiciones de utilidad general como SET, CLEAR, etc.
 */

#ifndef _MACROS_H
#define _MACROS_H

/*****************************************************************************
* configuracion
*****************************************************************************/
#define COMPILE_WITH_MAX6675

#ifndef COMPILE_WITH_MAX6675
#define COMPILE_WITH_DS18B20
#endif

/*****************************************************************************
* Defnicion de macros
*****************************************************************************/

#define	BYTE_MASK			0x000000FF	/**< \brief mascara para tomar el primer byte */
#define BIT_0_MASK			0x00000001
#define FIRST_NIBBLE_MASK	0x0000000F
#define DOS_PUNTOS			0x00000003
#define UN_PUNTO			0x00000002
#define SHIFT_DIGITO_1		0x00000000
#define SHIFT_DIGITO_2		0x0000000A
#define SHIFT_DIGITO_3		0x00000000
#define SHIFT_DIGITO_4		0x0000000A

/* Definiciones previas */
/*--------------------------------------------------------------------------*/
#define	SET		0x00000001
#define	CLEAR	0

#define	ERROR	1
#define OK		0

#define	SI		1
#define	NO		0

#define	LLENO	1
#define	VACIO	0

#define	OCUPADO	1
#define	LIBRE	0

#define ABRIR	2
#define CERRAR	3

#define	HIGH	1
#define	LOW		0

#define	ON		1
#define	OFF		0

#define	CERO	0x00
#define	UNOS	0xFF

#define	ENTRADA	0	/* Port Pin de Entrada */
#define	SALIDA	1	/* Port Pin de Salida */

/*****************************************************************************
* Defnición de macros
*****************************************************************************/
/** \brief Funcion que hace un reset del uc por vencimiento del WDT
 */
#define UC_RESET()	wdt_enable(WDTO_2S); wdt_reset(); while(1){}

/** \brief Activa un bit determinado en el registro.
 */
#define	SET_BIT(registro, bitNumero)		registro |= (SET<<bitNumero)

/** \brief Pasiva un bit determinado en n registro.
 */
#define	CLEAR_BIT(registro, bitNumero)		registro &= ~(SET<<bitNumero)

/** \brief	If bit is Activo
 */
#define IF_BIT_SET(puerto, pinNumero)		if (puerto & (SET<<pinNumero))

/** \brief	If bit is Pasivo
 */
#define IF_BIT_CLEAR(puerto, pinNumero)		if (!(puerto & (SET<<pinNumero)))

/** \brief Define la dirección de un pin determinado de un puerto.
 */
#define	DIRECCION_PIN(DDR_puerto, pinNumero, direccion)	if (direccion == SALIDA) { DDR_puerto |= (SALIDA<<pinNumero); } else { DDR_puerto &= ~(SALIDA<<pinNumero); }	\

/** \brief Activa o desactiba el pull up de un pin determiando de entrada.
 */
#define	PULL_UP_PIN(puerto, pinNumero, pullUp)	if (pullUp == ON) { puerto |= (ON<<pinNumero); } else { puerto &= ~(ON<<pinNumero);	}	\

#endif /* _MACROS_H */
