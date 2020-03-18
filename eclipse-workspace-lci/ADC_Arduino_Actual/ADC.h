/** \file	USART.h
 *  \brief	Contiene los prototipos de las maquinas de estados correspondiente a la recepción y \n
 *	transmisión por USART.
 */

#ifndef _ADC_EX_H_
#define _ADC_EX_H_

void InitADCEx();
uint16_t ReadADCEx(uint8_t ch);
uint16_t ReadADC_7380(void);


#endif
