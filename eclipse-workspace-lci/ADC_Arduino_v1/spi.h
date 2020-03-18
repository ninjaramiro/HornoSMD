


#include <avr/io.h>

#ifndef SPI_H
#define SPI_H


	#define SPI_PORT 	PORTB
	#define SPI_DDR 	DDRB

	//PINS
	#define SCK_POS 	PB7
	#define MISO_POS 	PB6
	#define MOSI_POS	PB5
	#define SS_POS		PB4


//CS : Chip Select
#define CS_PORT PORTB
#define CS_DDR	DDRB
#define CS_POS PB4

#define CS_HIGH() CS_PORT|=(1<<CS_POS)
#define CS_LOW() CS_PORT&=(~(1<<CS_POS))

void SPIInit();
void SPIClose();
uint8_t SPIWrite(uint8_t data);

#endif
