/** \file	USART.h
 *  \brief	Contiene los prototipos de las maquinas de estados correspondiente a la recepción y \n
 *	transmisión por USART.
 */

#ifndef	_USART_H
#define	_USART_H


#include <inttypes.h>

#define TRANSMITIR_NULL_AL_FINAL	/** \brief define si deseo transmitir \0 al final de la transmisión del buffer */
#define TRANSMITIR_ECO				/** \brief define si transmito el eco del caracter recibido */
//#define RECIBIR_NULL_AL_FINAL		/** \brief define si termino la recepción al recibir un \0 o sino cuando recibo \n o \r */

/*****************************************************************************
* Prototipos
*****************************************************************************/
/** \brief	Funcion que inicia el USART del uc.
 *
 *	Función que inicia el USART 0 en velocidad de transmisión es de 19,200 kbps, 8 bit de datos, \n
 *	sin control de paridad y 1 bit de parada.
 */



//void Inicio_USART (void);
void Inicio_USART( unsigned int ubrr);


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

/** \brief	Máquina de estados de transmisión por el USART del uc.
 *
 *	Esta máquina de estados al detectar el flag de buffer de transmisión lleno comienza el envío de la información. \n
 *	El envío se da por finalizado al encontrar el primer caracter NULL ('\0') o al llegar a la máxima dimensión permitida \n
 *	del buffer, por lo que se trunca el mensaje y de agrega un caracter NULL al final. Una vez enviado el mensaje se marca como \n
 *	vacio el buffer.
 */

void USART_Transmit( unsigned char data );
unsigned char USART_Receive( void );



void Transmision_USART_0(void);

/** \brief	USART0 vacio el buffer de recepción.
 *
 *	Purga cualquir cosa que pueda haber quedado en el buffer de recepción.
 */
void USART0_Flush(void);

/** \brief	USART0 fuerzo la transmisión del buffer completo.
 *
 *	Fuerza la transmisión del buffer completo de corrido hasta '\0' o hasta el tamaño máximo del buffer indicado \n
 *	para el cual trunca la transmisión y la termina con \'0'.
 *	\param[in] char *chBufferPtr, puntero al buffer a transmitir.
 *	\param[in] int maxBufferSize, máximo tamaño del buffer a transmitir.
 */
void Transmitir_Buffer_USART0 (char *chBufferPtr, int maxBufferSize);

/** \brief	USART0 transmito un float.
 *
 *	Transmite por puesto serie el valor de un float \n
 *	\param[in] float flValor, valor a transmitir.
 *	\param[in] char chCantDigitos, máxima cantidad de dígitos a tomar.
 *	\param[in] char chDecimales, máxima cantidad de decimales a tomar.
 */
void Transmitir_Float_USART0 (float flValor, char chCantDigitos, char chDecimales);

/** \brief	Funcion de espera de buffer libre para transmisión por USART 0.
 *
 * 	La función espera a que se desocupe el buffer por un tiempo dado por TIMEOUT_USART_0, \n
 * 	si el tiempo se vence la función continua para no bloquear la máquina de estados.
 */
void Esperar_USART_O_TX (void);


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
 *	\param[in] char chCantDigitos, máxima cantidad de dígitos a tomar (incluye a los decimales y la coma).
 *	\param[in] char chDecimales, máxima cantidad de decimales a tomar.
 */
void Transmitir_Float_USART0 (float flValor, char chCantDigitos, char chDecimales);

/** \brief	Transmite un byte, uint8_t, char por puerto serie.
 *
 *	Transmite un byte, char, uint8_t por puerto serie, solo se recomienta para debug.\n
 *	Esta funcion tiene un loop de espera sin timeout, puede bloquear al uc.\n
 *	\param[in] uint8_t c,  char c, byte a transmitir.
 */
void Transmitir_Uint8_USART0(uint8_t c);

/** \brief	Transmite un byte, uint8_t, char por puerto serie representado en hex por dos caracteres ASCII.
 *
 *	Transmite un byte, char, uint8_t por puerto serie representado en hex por dos caracteres ASCII,\n
 *	solo se recomienta para debug.\n
 *	Esta funcion tiene un loop de espera sin timeout, puede bloquear al uc.\n
 *	\param[in] uint8_t c,  char c, byte a transmitir en forma de dos caracteres en hex.
 */
void Transmitir_Uint8_Hex_USART0(uint8_t b);

/** \brief	Transmite dos bytes, uint16_t, int por puerto serie representado en hex por dos caracteres ASCII.
 *
 *	Transmite dos bytes, int, uint16_t por puerto serie representado en hex por dos caracteres ASCII,\n
 *	solo se recomienta para debug.\n
 *	Esta funcion tiene un loop de espera sin timeout, puede bloquear al uc.\n
 *	\param[in] uint16_t w,  int w, bytes a transmitir en forma de dos caracteres en hex.
 */
void Transmitir_Uint16_Hex_USART0(uint16_t w);

/** \brief	Transmite cuatro bytes, uint32_t, long por puerto serie representado en hex por dos caracteres ASCII.
 *
 *	Transmite cuatro bytes, long, uint32_t por puerto serie representado en hex por dos caracteres ASCII,\n
 *	solo se recomienta para debug.\n
 *	Esta funcion tiene un loop de espera sin timeout, puede bloquear al uc.\n
 *	\param[in] uint32_t w,  long w, bytes a transmitir en forma de dos caracteres en hex.
 */
void Transmitir_Uint32_Hex_USART0(uint32_t dw);

/** \brief	Transmite dos bytes, uint16_t, int por puerto serie representado en decimal por caracteres ASCII.
 *
 *	Transmite dos bytes, int, uint16_t por puerto serie representado en decimal por caracteres ASCII,\n
 *	solo se recomienta para debug.\n
 *	Esta funcion tiene un loop de espera sin timeout, puede bloquear al uc.\n
 *	\param[in] uint16_t w,  int w, bytes a transmitir en forma decimal.
 */
void Transmitir_Uint16_Dec_USART0(uint16_t w);

/** \brief	Transmite cuatro bytes, uint32_t, long por puerto serie representado en decimal por caracteres ASCII.
 *
 *	Transmite cuatro bytes, long, uint32_t por puerto serie representado en decimal por caracteres ASCII,\n
 *	solo se recomienta para debug.\n
 *	Esta funcion tiene un loop de espera sin timeout, puede bloquear al uc.\n
 *	\param[in] uint32_t w,  long w, bytes a transmitir en forma decimal.
 */
void Transmitir_Uint32_Dec_USART0(uint32_t dw);


/*****************************************************************************
* Definiciones varias
*****************************************************************************/

//#define	 UBRR				8	/**< \brief UBRR para una velocidad de transmisión de 115200bps con: fosc = 8MHz, U2X = 1 */
//#define	 UBRR				416	/**< \brief UBRR para una velocidad de transmisión de 2400bps con: fosc = 8MHz, U2X = 1 */

//#define	 UBRR				1666	/**< \brief UBRR para una velocidad de transmisión de 600bps con: fosc = 8MHz, U2X = 1 */
//#define VALOR_U2X0			1

#define	 UBRR				832	/**< \brief UBRR para una velocidad de transmisión de 600bps con: fosc = 8MHz, U2X = 0 */
#define VALOR_U2X0			0

//#define	 UBRR				207	/**< \brief UBRR para una velocidad de transmisión de 2400bps con: fosc = 8MHz, U2X = 0 */
//#define VALOR_U2X0			0



//#define	 UBRR				767	/**< \brief UBRR para una velocidad de transmisión de 600bps con: fosc = 7.3728MHz, U2X = 0 */
//#define VALOR_U2X0			0


//#define	 UBRR				3332	/**< \brief UBRR para una velocidad de transmisión de 300bps con: fosc = 8MHz, U2X = 1 */
//#define VALOR_U2X0			0


#define USART0_BUFFER_ZISE	30	/**< \brief tamaño máximo del buffer de datos del USART */
#define TIMEOUT_USART_0		30	/**< \brief timepo de timeout de espera para USART en milesimas segundo (timer2) */

/*****************************************************************************
* Definiciones máquina de estado de recepción
*****************************************************************************/
#define	 STANDBY_USART0_RX	02
#define	 RECIBIR_USART0		1

/*****************************************************************************
* Definiciones máquina de estado de transmisión
*****************************************************************************/
#define	 STANDBY_USART0_TX	0
#define	 TRANSMITIR_USART0	1

#endif /* _USART_H */
