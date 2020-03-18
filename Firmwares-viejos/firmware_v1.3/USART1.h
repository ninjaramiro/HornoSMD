
/** \file	USART1.h
 *  \brief	Contiene los prototipos de las maquinas de estados correspondiente a la recepción y \n
 *	transmisión por USART1.\n
 *	El funcionamiento de estas rutinas entan cosevidas para que el uc se comporte principalmente\n
 *	como un DTE, debido a que están orientadas para el uso con el modulo GSM.
 */

#ifndef	_USART1_H
#define	_USART1_H

/*****************************************************************************
* Prototipos
*****************************************************************************/
/** \brief	Funcion que inicia el USART del uc.
 *	
 *	Función que inicia el USART1 en velocidad de transmisión es de 19,200 kbps, 8 bit de datos, \n
 *	sin control de paridad y 1 bit de parada.
 */
void Inicio_USART_1 (void);

/** \brief	Máquina de estados de recepción por el USART del uc.
 *	
 *	Esta máquina de estados verifica permanentemente el comienzo de recepción de un mensaje por USART. \n
 *	En caso de que el buffer de recepción se encuentre lleno espera a que este se vacíe para comenzar a recibir. \n
 *	La máquina va recibiendo y llenando el buffer de recepción hasta que se reciba un caracter NULL ('\0') o se llegue \n
 *	máximo espacio asignado al buffer de recepción. \n
 *	Cuando da por finalizada la recepción marca al buffer de recepción como lleno para su posterior procesamiento en otra \n
 *	máquina de estados.
 */
void Recepcion_USART_1(void);

/** \brief	Máquina de estados de transmisión por el USART del uc.
 *	
 *	Esta máquina de estados al detectar el flag de buffer de transmisión lleno comienza el envío de la información. \n
 *	El envío se da por finalizado al encontrar el primer caracter NULL ('\0') o al llegar a la máxima dimensión permitida \n
 *	del buffer, por lo que se trunca el mensaje y de agrega un caracter NULL al final. Una vez enviado el mensaje se marca como \n
 *	vacio el buffer.
 */		
void Transmision_USART_1(void);

/** \brief	USART1 vacio el buffer de recepción.
 *
 *	Purga cualquir cosa que pueda haber quedado en el buffer de recepción.
 */
void USART1_Flush(void);

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define	UBRR1								25				/**< \brief UBRR para una velocidad de transmisión de 38400bps con: fosc = 8MHz, U2X = 1 */
#define VALOR_U2X1							1
#define	USART1_BUFFER_ZISE					100				/**< \brief tamaño máximo del buffer de datos del USART */
#define TIMEOUT_TX_USART_1					60				/**< \brief tiempo de timeout para transmision del buffer por USART en cilclos del timer2 */
#define TIMEOUT_RX_USART_1					200				/**< \brief tiempo de timeout para recepcion de USART, cierra el buffer de recepcion si no recive en ese tiempo, en cilclos del timer2 */
#define TIMEOUT_LECTURA_RX_USART_1			500				/**< \brief tiempo de timeout para lectura del buffer de recepcion del USART en cilclos del timer0, debe ser mayor que el tiempo de espera de Rx */

/*****************************************************************************
* Definiciones máquina de estado de recepción
*****************************************************************************/
#define	STANDBY			0
#define	RECIBIR_USART1	1

/*****************************************************************************
* Definiciones máquina de estado de transmisión
*****************************************************************************/
#define	STANDBY				0
#define	TRANSMITIR_USART1	1

#endif /* _USART1_H */
