/** \file	utiles.h
 *  \brief	Contiene funciones de uso general.
 */

#ifndef UTILES_H_
#define UTILES_H_

/*****************************************************************************
* Prototipos
*****************************************************************************/
/** \brief	Función que compara un string en memoria de programa con otro en RAM.
 *
 * \param[in] const char *chBufferOriginalPtr, puntero al string en la memoria de programa.
 * \param[in] char *chBufferRecibidolPtr, puntero al string en la memoria RAM.
 * \return, OK, o ERROR segun sea igual o no.
 */
char Comparar_Inicio_Buffer_P (const char *chBufferOriginalPtr, char *chBufferRecibidolPtr);

/** \brief	Función que compara un string en memoria de RAM con otro en RAM.
 *
 * \param[in] char *chBufferOriginalPtr, puntero al string en la memoria RAM.
 * \param[in] char *chBufferRecibidolPtr, puntero al string en la memoria RAM.
 * \return, OK, o ERROR segun sea igual o no.
 */
char Comparar_Inicio_Buffer (char *chBufferOriginalPtr, char *chBufferRecibidolPtr);

/** \brief	Función que busca un string de memoria de programa en otro en RAM.
 *
 * \param[in] const char *chBufferOriginalPtr, puntero al string en la memoria de programa.
 * \param[in] char *chBufferRecibidolPtr, puntero al string en la memoria RAM.
 * \param[in] int bufferZise, tamaño maximo del buffer de la memoria RAM.
 * \return, OK, o ERROR segun sea igual o no.
 */
char Buscar_en_Buffer_P (const char *chBufferOriginalPtr, char *chBufferRecibidoPtr, int bufferZise);

/** \brief	Función que busca un string de memoria ram en otro en RAM.
 *
 * \param[in] const char *chBufferOriginalPtr, puntero al string en la memoria ram.
 * \param[in] char *chBufferRecibidolPtr, puntero al string en la memoria RAM.
 * \param[in] int bufferZise, tamaño maximo del buffer de la memoria RAM.
 * \return, OK, o ERROR segun sea igual o no.
 */
char Buscar_en_Buffer (char *chBufferOriginalPtr, char *chBufferRecibidoPtr, int bufferZise);

/** \brief	Funcion que conviertes un string a mayusculas.
 *
 * \param[in] char *chStringPtr, string a convertir.
 * \param[in] int bufferZise, tamaño maximo del buffer de la memoria RAM donde esta el string.
 * \return, longitud del estring barrido.
 */
int String_a_Mayusculas (char * chStringPtr, int maxStringSize);

/** \brief	Funcion que extrae un float de un string en RAM.
 *
 * \param[in] const char *chBuffer, puntero al string en la memoria RAM.
 * \param[in] int maxBufferZise, tamaño maximo del buffer en RAM
 * \return, float con el valor.
 */
float From_String_to_Float (char *chBuffer, int maxBufferZise);

#endif /* UTILES_H_ */
