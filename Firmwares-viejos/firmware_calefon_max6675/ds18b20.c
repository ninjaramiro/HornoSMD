/*
ds18b20 lib 0x02

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "ds18b20.h"
#include "macros.h"

#ifdef COMPILE_WITH_DS18B20

//setup connection
#define DS18B20_PORT 	PORTB
#define DS18B20_DDR 	DDRB
#define DS18B20_PIN 	PINB
#define DS18B20_DQ 		PB2

/*****************************************************************************
* variables
*****************************************************************************/
unsigned char	uchEstadoSensorTemp;
float			flTempMedida;
float 			flTempAux;
uint8_t			auxLowBite;
uint8_t			auxHighBite;

/*****************************************************************************
* Maquina de estados
*****************************************************************************/
void ds18b20_gettemp (void) {
	switch (uchEstadoSensorTemp) {

		case DS18b20_RESET_1:
			ds18b20_reset(); //reset
			uchEstadoSensorTemp = DS18b20_SKIP_ROM_1;
			break;

		case DS18b20_SKIP_ROM_1:
			ds18b20_writebyte(DS18B20_CMD_SKIPROM); //skip ROM
			uchEstadoSensorTemp = DS18b20_START_CONV;
			break;

		case DS18b20_START_CONV:
			ds18b20_writebyte(DS18B20_CMD_CONVERTTEMP); //start temperature conversion
			uchEstadoSensorTemp = DS18b20_WAIT;
			break;

		case DS18b20_WAIT:
			if(ds18b20_readbit()) { //wait until conversion is complete
				uchEstadoSensorTemp = DS18b20_RESET_2;
			}
			break;

		case DS18b20_RESET_2:
			ds18b20_reset(); //reset
			uchEstadoSensorTemp = DS18b20_SKIP_ROM_2;
			break;

		case DS18b20_SKIP_ROM_2:
			ds18b20_writebyte(DS18B20_CMD_SKIPROM); //skip ROM
			uchEstadoSensorTemp = DS18b20_READ_SCRATCHPAD;
			break;

		case DS18b20_READ_SCRATCHPAD:
			ds18b20_writebyte(DS18B20_CMD_RSCRATCHPAD); //read scratchpad
			uchEstadoSensorTemp = DS18b20_READ_TEMP;
			break;

		case DS18b20_READ_TEMP:
			//read 2 byte from scratchpad
			auxLowBite = ds18b20_readbyte();
			auxHighBite = ds18b20_readbyte();
			flTempAux = (float)((auxHighBite << 8) + auxLowBite) * 0.0625;

			if (flTempMedida == 0){
				flTempMedida=flTempAux;
			}
			else if (abs(flTempAux/flTempMedida-1.0)<MAX_VAR_TEMP_NORMAL){
				flTempMedida=flTempAux;
			}

			uchEstadoSensorTemp = DS18b20_RESET_1;
			break;

		default:
			flTempMedida=0;
			uchEstadoSensorTemp = DS18b20_RESET_1;
			break;
	}
}

/*****************************************************************************
* ds18b20 init
*****************************************************************************/
uint8_t ds18b20_reset() {
	uint8_t i;

	//low for 480us
	DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
	DS18B20_DDR |= (1<<DS18B20_DQ); //output
	_delay_us(480);

	//release line and wait for 60uS
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
	_delay_us(60);

	//get value and wait 420us
	i = (DS18B20_PIN & (1<<DS18B20_DQ));
	_delay_us(420);

	//return the read value, 0=ok, 1=error
	return i;
}


/*****************************************************************************
* write one bit
*****************************************************************************/
void ds18b20_writebit(uint8_t bit){
	//low for 1uS
	DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
	DS18B20_DDR |= (1<<DS18B20_DQ); //output
	_delay_us(1);

	//if we want to write 1, release the line (if not will keep low)
	if(bit)
		DS18B20_DDR &= ~(1<<DS18B20_DQ); //input

	//wait 60uS and release the line
	_delay_us(60);
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
}


/*****************************************************************************
* read one bit
*****************************************************************************/
uint8_t ds18b20_readbit(void){
	uint8_t bit=0;

	//low for 1uS
	DS18B20_PORT &= ~ (1<<DS18B20_DQ); //low
	DS18B20_DDR |= (1<<DS18B20_DQ); //output
	_delay_us(1);

	//release line and wait for 14uS
	DS18B20_DDR &= ~(1<<DS18B20_DQ); //input
	_delay_us(14);

	//read the value
	if(DS18B20_PIN & (1<<DS18B20_DQ))
		bit=1;

	//wait 45uS and return read value
	_delay_us(45);
	return bit;
}


/*****************************************************************************
* write one byte
*****************************************************************************/
void ds18b20_writebyte(uint8_t byte){
	uint8_t i=8;
	while(i--){
		ds18b20_writebit(byte&1);
		byte >>= 1;
	}
}

/*****************************************************************************
* read one byte
*****************************************************************************/
uint8_t ds18b20_readbyte(void){
	uint8_t i=8, n=0;
	while(i--){
		n >>= 1;
		n |= (ds18b20_readbit()<<7);
	}
	return n;
}
#endif
