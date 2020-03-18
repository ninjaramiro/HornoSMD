/*
ds18b20 lib 0x02

copyright (c) Davide Gironi, 2012

Released under GPLv3.
Please refer to LICENSE file for licensing information.

References:
  + Using DS18B20 digital temperature sensor on AVR microcontrollers
    by Gerard Marull Paretas, 2007
    http://teslabs.com/openplayer/docs/docs/other/ds18b20_pre1.pdf
*/

#include "macros.h"

#ifdef COMPILE_WITH_DS18B20

#ifndef DS18B20_H_
#define DS18B20_H_

/*****************************************************************************
* funciones
*****************************************************************************/
// ds18b20 init
uint8_t ds18b20_reset();

// write one bit
void ds18b20_writebit(uint8_t bit);

// read one bit
uint8_t ds18b20_readbit(void);

// write one byte
void ds18b20_writebyte(uint8_t byte);

// read one byte
uint8_t ds18b20_readbyte(void);

// read temp, maquina de estados
void ds18b20_gettemp(void);

/*****************************************************************************
* commands
*****************************************************************************/
#define DS18B20_CMD_CONVERTTEMP 	0x44
#define DS18B20_CMD_RSCRATCHPAD 	0xbe
#define DS18B20_CMD_WSCRATCHPAD 	0x4e
#define DS18B20_CMD_CPYSCRATCHPAD 	0x48
#define DS18B20_CMD_RECEEPROM 		0xb8
#define DS18B20_CMD_RPWRSUPPLY 		0xb4
#define DS18B20_CMD_SEARCHROM 		0xf0
#define DS18B20_CMD_READROM 		0x33
#define DS18B20_CMD_MATCHROM 		0x55
#define DS18B20_CMD_SKIPROM 		0xcc
#define DS18B20_CMD_ALARMSEARCH 	0xec

/*****************************************************************************
* estados
*****************************************************************************/
#define DS18b20_RESET_1				0
#define DS18b20_SKIP_ROM_1			1
#define DS18b20_START_CONV			2
#define DS18b20_WAIT				3
#define DS18b20_RESET_2				4
#define DS18b20_SKIP_ROM_2			5
#define DS18b20_READ_SCRATCHPAD		6
#define DS18b20_READ_TEMP			7

/*****************************************************************************
* varios
*****************************************************************************/
//#define CANTIDAD_MUESTRAS_TEMP_ANTERIOR	20

#define MAX_VAR_TEMP_NORMAL			0.5

#endif
#endif
