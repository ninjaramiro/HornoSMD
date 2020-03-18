/******************************************************************************


*******************************************************************************/

#include <avr/io.h>

#include "spi.h"

/********************************************************************

Function To Initialize the Internal SPI Hardware

********************************************************************/
void SPIInit()
{
	//Set up SPI I/O Ports
	SPI_DDR|=((1<<MOSI_POS)|(1<<SCK_POS)|(1<<SS_POS));

	SPI_PORT|=(1<<SS_POS);

	//Chip Select PIN is initialize as HIGH i.e. Idle
	//CS PIN is always Active Low (inverted)
	CS_DDR|=(1<<CS_POS);
	CS_HIGH();

	/*

	SPI BUS CONFIGURATION
	---------------------
	*Master Mode
	*MSB first
	*CPOL=0
	*CPHA=0
	*Above two implies SPI MODE0
	*SCK Freq = FCPU/16 i.e. 1MHz

	*/

//	SPCR|=(1<<CPOL)|(1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1)|(0<<SPI2X);

	SPCR|=(1<<CPOL)|(1<<SPE)|(1<<MSTR)|(0<<SPR0)|(0<<SPR1);
	SPSR|=(1<<SPI2X);
}



/********************************************************************

Function To Disable the Internal SPI Hardware

********************************************************************/
void SPIClose()
{
	SPCR&=(~(1<<SPE));
	SPI_DDR&=(~((1<<MOSI_POS)|(1<<SCK_POS)));
}



/********************************************************************

Function for SPI Transaction.

Arguments:
	uint8_t data : Data to write

Return Value
	uint8_t : Data Read from BUS

********************************************************************/
uint8_t SPIWrite(uint8_t data)
{
	/* Start transmission */
	SPDR = data;
	/* Wait for transmission complete */
	while(!(SPSR & (1<<SPIF)));

	return (SPDR);
}

