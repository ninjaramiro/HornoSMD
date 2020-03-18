
/** \file	macros.h
 *  \brief	Contiene macros de uso común.
 *
 *	Contiene macros para activar o desactivar pines, definir direcciones de puertos, \n 
 *	esperar al SPI y además definiciones de utilidad general como SET, CLEAR, etc.
 */

#define COMPILE_WITH_DEBUG		/**< \brief define si se compila con debug */

/*****************************************************************************
* Defniciones varias
*****************************************************************************/
/* mascaras */
/*--------------------------------------------------------------------------*/
#define	BIT_0_MASK			1			/**< \brief mascara para tomar el primer bit */
#define	BIT_1_MASK			2			/**< \brief mascara para tomar el segundo bit */
#define	BIT_2_MASK			4			/**< \brief mascara para tomar el tercer bit */
#define	BIT_012_MASK		7			/**< \brief mascara para tomar los ultimos tres bits */
#define	BYTE_MASK			0x000000FF	/**< \brief mascara para tomar el primer byte */
#define FIRST_NIBBLE_MASK	0x0F
#define SECOND_NIBBLE_MASK	0XF0

/* otras */
/*--------------------------------------------------------------------------*/
#define	F_CPU			8000000UL	/**< \brief frecuencia del oscilador en Hz */

/* Definiciones previas */
/*--------------------------------------------------------------------------*/

#define	ERROR	1
#define OK		0

#define	SET		1
#define	CLEAR	0

#define	HIGH	1
#define	LOW		0

#define	ON		1
#define	OFF		0

#define	SI		1
#define	NO		0

#define	LLENO	1
#define	VACIO	0

#define	OCUPADO	1
#define	LIBRE	0

#define ABRIR	2
#define CERRAR	3

#define	CERO	0x00
#define	UNOS	0xFF

#define	ENTRADA	0	/**< \brief Port Pin de Entrada */
#define	SALIDA	1	/**< \brief Port Pin de Salida */

/*****************************************************************************
* Defnición de macros
*****************************************************************************/
/** \brief Funcion que hace un reset del uc por vencimiento del WDT
 */
#define UC_RESET()	wdt_enable(WDTO_2S); wdt_reset(); while(1){}

/** \brief Invierte un bit determinado en el registro.
 */
#define	TOGGLE_BIT(registro, bitNumero)		registro ^= (SET<<bitNumero)

/** \brief Activa un bit determinado en el registro.
 */
#define	SET_BIT(registro, bitNumero)		registro |= (SET<<bitNumero)	

/** \brief Pasiva un bit determinado en n registro. 
 */
#define	CLEAR_BIT(registro, bitNumero)		registro &= ~(SET<<bitNumero)
	
/** \brief Define la dirección de un pin determinado de un puerto.
 */
#define	DIRECCION_PIN(DDR_puerto, pinNumero, direccion)	if (direccion == SALIDA) { DDR_puerto |= (SALIDA<<pinNumero); } else { DDR_puerto &= ~(SALIDA<<pinNumero); }	\

/** \brief Activa o desactiba el pull up de un pin determiando de entrada. 
 */
#define	PULL_UP_PIN(puerto, pinNumero, pullUp)	if (pullUp == ON) { puerto |= (ON<<pinNumero); } else { puerto &= ~(ON<<pinNumero);	}	\

