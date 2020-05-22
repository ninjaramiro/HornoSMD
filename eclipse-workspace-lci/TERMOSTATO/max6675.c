/*
* max6675.c
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "max6675.h"
#include "macros.h"
#include "inicioUc.h"

/*****************************************************************************
* variables
*****************************************************************************/
unsigned char			uchEstadoSensorTemp;
float					flTempMedida;
float 					flTempAux;
int						auxLowBite;
int						auxHighBite;
volatile unsigned int	uContMax6675;

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void Max6675_Get_Temp (void) {
	switch (uchEstadoSensorTemp) {
		case MAX6675_TX_RX:
			if(uContMax6675==0){
				// habilito el cs
				// CLEAR_BIT(TEMP_AGUA_PORT,TEMP_AGUA_PIN); // Esto seria para un Atmega328p
															// TEMP_AGUA_PORT: PORTB
														 	// TEMP_AGUA_PIN: PB2
													     	// 		SS (SPI bus master slave select)
															// 		OC1B (Timer/Counter1 output compare match B output)
															// 		PCINT2 (pin change interrupt 2)

				CLEAR_BIT(CS3_PORT,CS3_PIN); // CS3_PIN: PB4 - SS (SPI Slave Select Input)

				auxHighBite = (int)Spi_Max6675_TxRx(0x00);
				auxLowBite = (int)Spi_Max6675_TxRx(0x00);

				// deshabilito el cs para que realize una nueva conversion
				// SET_BIT(TEMP_AGUA_PORT,TEMP_AGUA_PIN);
				SET_BIT(CS3_PORT,CS3_PIN);

				uchEstadoSensorTemp=MAX6675_CALC_TEMP;
			}
			break;

		case MAX6675_CALC_TEMP:
			flTempAux = ((float)((auxHighBite << 5) | (auxLowBite >> 3)) / 4.0) + OFFSET_MAX6675;

			if (flTempMedida == 0){
				flTempMedida=flTempAux;
			}
			else if (abs(flTempAux/flTempMedida-1.0)<MAX_VAR_TEMP_NORMAL){
				flTempMedida = flTempMedida + (flTempAux - flTempMedida) / MUESTRAS_PROMEDIO_MAX6675;
			}

			uContMax6675 = PERIODO_MEDICION_MAX6675;
			uchEstadoSensorTemp = MAX6675_TX_RX;
			break;

		default:
			Spi_Max6675_Inicio();
			break;
	}
}

/*****************************************************************************
* spi_max6675_init
*****************************************************************************/
void Spi_Max6675_Inicio(void) {
	// aseguro que no este dehabilitado por ahorro de consumo
	//CLEAR_BIT(PRR,PRSPI);
	// Creo que el Atmega32 no tiene PRR (Power Reduction Register)

	// Enable SPI, Master, set clock rate fck/16 = 1MHz
	SPCR = (OFF<<SPIE)|(ON<<SPE)|(OFF<<DORD)|(ON<<MSTR)|(OFF<<CPOL)|(ON<<CPHA)|(OFF<<SPR1)|(ON<<SPR0);

	// subo el cs para que realize una conversion
	// SET_BIT(TEMP_AGUA_PORT,TEMP_AGUA_PIN);
	SET_BIT(CS3_PORT,CS3_PIN);

	flTempMedida=0;

	uContMax6675=PERIODO_MEDICION_MAX6675;
	uchEstadoSensorTemp = MAX6675_TX_RX;
}

/*****************************************************************************
* spi_max6675_TxRx
*****************************************************************************/
char Spi_Max6675_TxRx(char dataTx) {
	// leo el flag SPIF para que se limpie
	char dummy = SPSR;
	// Start transmission
	SPDR = dataTx;
	// Wait for transmission complete
	while(!(SPSR & (ON<<SPIF)));
	// devuelvo el valor recivido
	return SPDR;
}
