/** \file	eeprom.h
 *  \brief	Contiene los prototipos de las funciones para el manejo de datos desde y hacia la eeprom.
 */

#ifndef EEPROM_H_
#define EEPROM_H_

/*****************************************************************************
* Prototipos
*****************************************************************************/

/** \brief carga todas las variables de configuración desde la eeprom.
 */
void Carga_Configuracion_Desde_Eeprom (void);

/** \brief carga todas las variables de configuración desde la eeprom con valores de fabrica.
 *
 * 	\param[in] int grupo, indica a que grupo se le deben cargar los valores de fabrica.
 */
void Carga_Configuracion_Fabrica_Eeprom (int grupo);

/** \brief	graba un float a la eeprom.
 *
 *	\param[in] float *flEepromPtr, puntero con la dirección a la cual grabar el dato. \n
 *	\param[in] float *flDatoEscritoPtr, puntero a la variable a guardar en la dirección dada. \n
 */
void Float_a_Eeprom (float *flEepromPtr, float *flDatoEscritoPtr);

/** \brief	Lee un float de la eeprom.
 *
 *	\param[in] float *flEepromPtr, puntero con la dirección de la cual leer el dato. \n
 *	\param[in] float *flDatoLeidoPtr, puntero a la variable leida de la dirección dada. \n
 */
void Eeprom_a_Float (float *flEepromPtr, float *flDatoLeidoPtr);

/** \brief	graba un unsigned int a la eeprom.
 *
 *	\param[in] unsigned int *uEepromPtr, puntero con la dirección a la cual grabar el dato. \n
 *	\param[in] unsigned int *uDatoEscritoPtr, puntero a la variable a guardar en la dirección dada. \n
 */
void UnsignedInt_a_Eeprom (unsigned int *uEepromPtr, unsigned int *uDatoEscritoPtr);

/** \brief	lee un unsigned int a la eeprom.
 *
 *	\param[in] unsigned int *uEepromPtr, puntero con la dirección de la cual leer el dato. \n
 *	\param[in] unsigned int *uDatoLeidoPtr, puntero a la variable leida de la dirección dada. \n
 */
void Eeprom_a_UnsignedInt (unsigned int *uEepromPtr, unsigned int *uDatoLeidoPtr);

/** \brief	graba un char a la eeprom.
 *
 *	\param[in] char *chEepromPtr, puntero con la dirección a la cual grabar el dato. \n
 *	\param[in] char *chDatoEscritoPtr, puntero a la variable a guardar en la dirección dada. \n
 */
void Char_a_Eeprom (char *chEepromPtr, char *chDatoEscritoPtr);

/** \brief	lee un char de la eeprom.
 *
 *	\param[in] char *chEepromPtr, puntero con la dirección a la cual grabar el dato. \n
 *	\param[in] char *chDatoLeidoPtr, variable a guardar en la dirección dada. \n
 */
void Eeprom_a_Char (char *chEepromPtr, char *chDatoLeidoPtr);

/** \brief	cálcula el checksum de una dirección origen a una destino de la eeprom.
 *
 *	\param[in] char *flInicioEepromPtr, puntero a la primer dirección a partir de la cual se calcula el checsum. \n
 *	\param[in] unsigned int *uChecksumEepromPtr, puntero a la dirección del checksum que se encuentra al final de los datos. \n
 *	\return unsigned int, devuelve el checksum de 16bit.
 */
unsigned int Calculo_Checksum (char *flInicioEepromPtr, unsigned int *uChecksumEepromPtr);

/** \brief	cálcula el checksum de un float.
 *
 *	\param[in]float *flDatoPtr, puntero a la dirección del float. \n
 *	\return unsigned int, devuelve el checksum de 16bit.
 */
unsigned int Float_a_Checksum (float *flDatoPtr);

/** \brief	cálcula el checksum de un unsigned int.
 *
 *	\param[in] unsigned int *uDatoPtr, puntero a la dirección del unsigned int. \n
 *	\return unsigned int, devuelve el checksum de 16bit.
 */
unsigned int UnsignedInt_a_Checksum (unsigned int *uDatoPtr);

/** \brief	cálcula el checksum de un char.
 *
 *	\param[in]char *chDatoPtr, puntero a la dirección del char. \n
 *	\return unsigned int, devuelve el checksum de 16bit.
 */
unsigned int Char_a_Checksum (char *chDatoPtr);

/** \brief	cálcula el checksum de un grupo de variables y lo escrive en la eeprom, original y copia.
 *
 *	\param[in]char chGrupo, grupo al cual se le actualiza en checksum \n
 */
void Escribir_Checksum_Eeprom (char chGrupo);

/*****************************************************************************
* Definiciones varias
*****************************************************************************/

#define 	CANT_INTENTOS_ESCRITURA_EEPROM		100	/**< \brief cantidad máxima de intentos de escritura en eeprom */
#define 	CANT_INTENTOS_LECTURA_EEPROM		100	/**< \brief cantidad máxima de intentos de lectura de eeprom */
#define 	CANT_INTENTOS_VALIDACION_CHECKSUM	10	/**< \brief cantidad de intentos de validar el checksum en caso de dar error en la integridad de datos */

/*****************************************************************************
* Definiciones de valores por defecto de fábrica
*****************************************************************************/

#define 	CHECKSUM_EEPROM			10000			/**< \brief valor cualquiera de inicio de checksum para que sea distinto que 0 */

/* grupos de checksum */
#define 	GRUPO_ALARMAS			1				/**< \brief grupo 1 para el cual se calcula el checksum */
#define 	GRUPO_ADC				2				/**< \brief grupo 2 para el cual se calcula el checksum */
#define 	GRUPO_PWM				3				/**< \brief grupo 3 para el cual se calcula el checksum */
#define 	GRUPO_TODOS				4

#endif /* EEPROM_H_ */
