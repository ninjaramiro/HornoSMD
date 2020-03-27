#include "Blinky.h"



#include "macros.h"
#include "inicioUc.h"

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

volatile unsigned int uiContador_Blinky;
volatile unsigned char ucEstado_Blinky;

void blinky_init(void) //Configuro estado inicial
{
	ucEstado_Blinky = LED_TITILANDO_APAGADO;
}

void blinky(void)  // Maquina de estados  (los estados se definen en Blinky.h)
{

	switch (ucEstado_Blinky){

		case LED_FIJO_APAGADO:
			CLEAR_BIT(LEDS_PORT, LED1);
			CLEAR_BIT(LEDS_PORT, LED2);

			break;

		case LED_FIJO_ENCENDIDO:
			SET_BIT(LEDS_PORT, LED1);
			SET_BIT(LEDS_PORT, LED2);

			break;

		case LED_TITILANDO_APAGADO:
			CLEAR_BIT(LEDS_PORT, LED1);
			CLEAR_BIT(LEDS_PORT, LED2);

			if (uiContador_Blinky == 0){
				uiContador_Blinky = TIEMPO_PARPADEO;
				ucEstado_Blinky = LED_TITILANDO_ENCENDIDO;
			}

			break;

		case LED_TITILANDO_ENCENDIDO:
			SET_BIT(LEDS_PORT, LED1);
			SET_BIT(LEDS_PORT, LED2);


			if (uiContador_Blinky == 0){
				uiContador_Blinky = TIEMPO_PARPADEO;
				ucEstado_Blinky = LED_TITILANDO_APAGADO;
			}

			break;

		default:
			blinky_init();
			break;

	}


}
