
/** \file	USART0.h
 *  \brief	Contiene los prototipos de las maquinas de estados correspondiente a la recepción y \n
 *	transmisión por USART0.\n
 *	El funcionamiento de estas rutinas entan cosevidas para que el uc se comporte principalmente\n
 *	como un DCE, debido a que estan orientadas para el uso de un hiperterminal desde una PC.
 */

#ifndef	_USART0_H
#define	_USART0_H

//#define TRANSMITIR_NULL_AL_FINAL	/** \brief define si deseo transmitir \0 al final de la transmisión del buffer */
#define TRANSMITIR_ECO				/** \brief define si transmito el eco del caracter recibido */
//#define RECIBIR_NULL_AL_FINAL		/** \brief define si termino la recepción al recibir un \0 o sino cuando recibo \n o \r */

/*****************************************************************************
* Definiciones varias
*****************************************************************************/
#define	UBRR0								25										/**< \brief UBRR para una velocidad de transmisión de 38400bps con: fosc = 8MHz, U2X = 1 */
#define VALOR_U2X0							1
#define	USART0_BUFFER_ZISE					200										/**< \brief tamaño máximo del buffer de datos del USART */
#define TIMEOUT_ESPERA_USART_0				5										/**< \brief timepo de timeout de espera para USART en cilclos del timer2 */
#define TIMEOUT_TX_USART_0					5										/**< \brief tiempo de timeout para transmision del buffer por USART en cilclos del timer2 */
#define TIMEOUT_LECTURA_RX_USART_0			2*TIMEOUT_TX_USART_0*USART0_BUFFER_ZISE	/**< \brief tiempo de timeout para lectura del buffer de recepcion del USART en cilclos del timer2 */
#define TIMEOUT_RX_USART_0					30000									/**< \brief tiempo de timeout para recepcion de USART, cierra el buffer de recepcion si no recive en ese tiempo, en cilclos del timer2 */

/*****************************************************************************
* Definiciones máquina de estado de recepción
*****************************************************************************/
#define	STANDBY			0
#define	RECIBIR_USART0	1

/*****************************************************************************
* Definiciones máquina de estado de transmisión
*****************************************************************************/
#define	STANDBY				0
#define	TRANSMITIR_USART0	1

/*****************************************************************************
* Prototipos
*****************************************************************************/
/** \brief	Funcion que inicia el USART del uc.
 *	
 *	Función que inicia el USART 0 en velocidad de transmisión es de 19,200 kbps, 8 bit de datos, \n
 *	sin control de paridad y 1 bit de parada.
 */
void Inicio_USART_0 (void);

/** \brief	Máquina de estados de recepción por el USART del uc.
 *	
 *	Esta máquina de estados verifica permanentemente el comienzo de recepción de un mensaje por USART. \n
 *	En caso de que el buffer de recepción se encuentre lleno espera a que este se vacíe para comenzar a recibir. \n
 *	La máquina va recibiendo y llenando el buffer de recepción hasta que se reciba un caracter NULL ('\0') o se llegue \n
 *	máximo espacio asignado al buffer de recepción. \n
 *	Cuendo da por finalizada la recepción marca al buffer de recepción como lleno para su posterior procesamiento en otra \n
 *	máquina de estados.
 */
void Recepcion_USART_0(void);

///** \brief	Máquina de estados de transmisión por el USART del uc.
// *
// *	Esta máquina de estados al detectar el flag de buffer de transmisión lleno comienza el envío de la información. \n
// *	El envío se da por finalizado al encontrar el primer caracter NULL ('\0') o al llegar a la máxima dimensión permitida \n
// *	del buffer, por lo que se trunca el mensaje y de agrega un caracter NULL al final. Una vez enviado el mensaje se marca como \n
// *	vacio el buffer.
// */
//void Transmision_USART_0(void);

/** \brief	USART0 vacio el buffer de recepción.
 *
 *	Purga cualquir cosa que pueda haber quedado en el buffer de recepción.
 */
void USART0_Flush(void);

/** \brief	USART0 fuerzo la transmisión del buffer completo.
 *
 *	Fuerza la transmisión del buffer completo de corrido hasta '\0' o hasta el tamaño máximo del buffer indicado \n
 *	para el cual trunca la transmisión y la termina con \'0'.\n
 *	Si las interrupciones estan desabilitadas esta funcion puede bloquear al uc.\n
 *	\param[in] char *chBufferPtr, puntero al buffer a transmitir.
 *	\param[in] int maxBufferSize, máximo tamaño del buffer a transmitir.
 */
void Transmitir_Buffer_USART0 (char *chBufferPtr, int maxBufferSize);

/** \brief	USART0 fuerzo la transmisión del string de memoria de programa.
 *
 *	Fuerza la transmisión del string de memoria de programa hasta '\0'\n
 *	Si las interrupciones estan desabilitadas esta funcion puede bloquear al uc.\n
 *	\param[in] const char *chStringPtr, puntero al string a transmitir.
 */
void Transmitir_Buffer_USART0_P (const char *chStringPtr);

/** \brief	USART0 transmito un float.
 *
 *	Transmite por puesto serie el valor de un float \n
 *	Si las interrupciones estan desabilitadas esta funcion puede bloquear al uc.\n
 *	\param[in] float flValor, valor a transmitir.
 *	\param[in] char chCantDigitos, máxima cantidad de dígitos a tomar.
 *	\param[in] char chDecimales, máxima cantidad de decimales a tomar.
 */
void Transmitir_Float_USART0 (float flValor, char chCantDigitos, char chDecimales);

/** \brief	USART0 fuerzo la recepción del buffer completo de prepo.
 *
 *	Fuerza la recepcion del buffer completo de corrido hasta '\0' o  '\n' o '\r' ohasta el tamaño máximo del buffer indicado \n
 *	para el cual trunca la transmisión y la termina con \'0'.\n
 *	Si estan funcionando las interrupciones tambien termina po timeout.\n
 *	Si las interrupciones estan desabilitadas esta funcion puede bloquear al uc.\n
 *	\param[in] char *chBufferPtr, puntero al buffer a transmitir.
 *	\param[in] int maxBufferSize, máximo tamaño del buffer a transmitir.
 */
void Recibir_Buffer_USART0 (char *chBufferPtr, int maxBufferSize);

/** \brief	Funcion de espera de buffer libre para transmisión por USART 0.
 *
 * 	La función espera a que se desocupe el buffer por un tiempo dado por TIMEOUT_USART_0, \n
 * 	si el tiempo se vence la función continua para no bloquear la máquina de estados.\n
 * 	Si las interrupciones estan desabilitadas esta funcion puede bloquear al uc.\n
 * 	\return, devuelve 1 si sale por timeout.
 */
char Esperar_USART_O_TX (void);

/** \brief	Funcion de espera de buffer lleno para recepción por USART 0.
 *
 * 	La función espera a que se desocupe el buffer por un tiempo dado por TIMEOUT_USART_0, \n
 * 	si el tiempo se vence la función continua para no bloquear la máquina de estados.\n
 * 	Si las interrupciones estan desabilitadas esta funcion puede bloquear al uc.\n
 * 	\return, devuelve 1 si sale por timeout.
 */
char Esperar_USART_0_RX (void);

#endif /* _USART0_H */
