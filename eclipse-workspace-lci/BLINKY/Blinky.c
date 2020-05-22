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
	//ucEstado_Blinky = LED_TITILANDO_APAGADO; // me prende los dos leds de las puntas, sin titilar
	//ucEstado_Blinky = LED_TITILANDO_ENCENDIDO;// me apaga los dos leds de las puntas, sin titilar
	//ucEstado_Blinky = LED_FIJO_ENCENDIDO; // me prende los dos leds de las puntas
	//ucEstado_Blinky = LED_FIJO_APAGADO; // me apaga los dos leds de las puntas
	ucEstado_Blinky = LEDS_APAGADOS; // Para prender los leds secuencialmente
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

// Para prender los leds 4 secuencialmente ####################################################3

		case LEDS_APAGADOS:
			CLEAR_BIT(LEDS_PORT, PD4);
			CLEAR_BIT(LEDS_PORT, PD5);
			CLEAR_BIT(LEDS_PORT, PD6);
			CLEAR_BIT(LEDS_PORT, PD7);
			ucEstado_Blinky=LED1_ON;
			break;


		case LED1_ON:
			SET_BIT(LEDS_PORT, PD4);
			if (uiContador_Blinky == 0){
				uiContador_Blinky = TIEMPO_PARPADEO;
				CLEAR_BIT(LEDS_PORT, PD4);
				ucEstado_Blinky = LED2_ON;
			}
			break;

		case LED2_ON:
			SET_BIT(LEDS_PORT, PD5);
			if (uiContador_Blinky == 0){
				uiContador_Blinky = TIEMPO_PARPADEO;
				CLEAR_BIT(LEDS_PORT, PD5);
				ucEstado_Blinky = LED3_ON;
			}
			break;

		case LED3_ON:
			SET_BIT(LEDS_PORT, PD6);
			if (uiContador_Blinky == 0){
				uiContador_Blinky = TIEMPO_PARPADEO;
				CLEAR_BIT(LEDS_PORT, PD6);
				ucEstado_Blinky = LED4_ON;
			}
			break;

		case LED4_ON:
			SET_BIT(LEDS_PORT, PD7);
			if (uiContador_Blinky == 0){
				uiContador_Blinky = TIEMPO_PARPADEO;
				CLEAR_BIT(LEDS_PORT, PD7);
				ucEstado_Blinky = LED1_ON;
			}
			break;

		default:
			blinky_init();
			break;

	}


}
