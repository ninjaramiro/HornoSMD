/*
* max6675.h
*/

#include "macros.h"


/*****************************************************************************
* funciones
*****************************************************************************/
// read temp, maquina de estados
void Max6675_Get_Temp(void);

// funcion de inicio
void Spi_Max6675_Inicio(void);

// funcion de lectura escritura de spi
char Spi_Max6675_TxRx(char dataTx);

/*****************************************************************************
* estados
*****************************************************************************/
#define MAX6675_TX_RX				0
#define MAX6675_CALC_TEMP			1

/*****************************************************************************
* varios
*****************************************************************************/
#define PERIODO_MEDICION_MAX6675	250
#define OFFSET_MAX6675				-3.0 //°C
#define MUESTRAS_PROMEDIO_MAX6675	2.0

#define MAX_VAR_TEMP_NORMAL			0.5
