#include "Botones.h"



#include "macros.h"
#include "inicioUc.h"
#include "Blinky.h"


#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/eeprom.h>
#include <avr/pgmspace.h>

// Defino un struct para manejar todos los datos de los botones
typedef struct {
	unsigned int  uiContador_Debouncer;
	unsigned char ucEstado_Botones;
	unsigned char ucFlag_Boton_detectado;
	unsigned char ucNumero_Boton_detectado;

}botones_t;

volatile botones_t str_botones;

// Variable externa para probar
extern volatile unsigned char ucEstado_Blinky;


void botones_init(void) //Configuro estado inicial
{
	str_botones.uiContador_Debouncer = 0;
	str_botones.ucEstado_Botones = ESPERANDO_BOTON;
	str_botones.ucFlag_Boton_detectado = 0;
	str_botones.ucNumero_Boton_detectado = 0;

}

void botones(void)  // Maquina de estados  (los estados se definen en Blinky.h)
{
	if ((BOT_PIN & (SET<<BOT_DISP_PIN)) == 0){
//	IF_BIT_CLEAR(AUX_PORT, AUX1_PIN){

		ucEstado_Blinky = LED_FIJO_ENCENDIDO;


	}


//	switch (str_botones.ucEstado_Botones){
//
//		case ESPERANDO_BOTON:
//			if(str_botones.ucFlag_Boton_detectado == 0){ // Solo detecto si leyeron el boton anteriormente
//
//				// Ver definiciones en macros.h
//				IF_BIT_CLEAR(AUX_PORT, AUX1_PIN){  //deteccion en pin1
//					str_botones.ucNumero_Boton_detectado = 1;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//				IF_BIT_CLEAR(AUX_PORT, AUX2_PIN){  //deteccion en pin2
//					str_botones.ucNumero_Boton_detectado = 2;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//				IF_BIT_CLEAR(AUX_PORT, AUX3_PIN){  //deteccion en pin3
//					str_botones.ucNumero_Boton_detectado = 3;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//				IF_BIT_CLEAR(AUX_PORT, AUX4_PIN){  //deteccion en pin4
//					str_botones.ucNumero_Boton_detectado = 4;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//				IF_BIT_CLEAR(AUX_PORT, AUX5_PIN){  //deteccion en pin5
//					str_botones.ucNumero_Boton_detectado = 5;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//				IF_BIT_CLEAR(AUX_PORT, AUX6_PIN){  //deteccion en pin6
//					str_botones.ucNumero_Boton_detectado = 6;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//				IF_BIT_CLEAR(AUX_PORT, AUX7_PIN){  //deteccion en pin7
//					str_botones.ucNumero_Boton_detectado = 7;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//				IF_BIT_CLEAR(AUX_PORT, AUX8_PIN){  //deteccion en pin8
//					str_botones.ucNumero_Boton_detectado = 8;
//					str_botones.ucEstado_Botones = DETECCION_BOTON;
//					str_botones.uiContador_Debouncer = TIEMPO_DEBOUNCER;
//				}
//			}
//
//
//			break;
//
//		case ESPERANDO_DEBOUNCER:
//
//			if (str_botones.uiContador_Debouncer == 0){
//				str_botones.ucEstado_Botones = DETECCION_BOTON;
//			}
//
//			break;
//
//		case DETECCION_BOTON:  // Vuelvo a chequear el estado de los botones
//
//			IF_BIT_CLEAR(AUX_PORT, AUX1_PIN){  //deteccion en pin1
//				if (str_botones.ucNumero_Boton_detectado == 1){
//					str_botones.ucNumero_Boton_detectado = 1;
//					str_botones.ucFlag_Boton_detectado = 1; // Levanto el flag de deteccion de boton
//
//				}
//				else
//				{
//					str_botones.ucNumero_Boton_detectado = 0;
//				}
//
//
//
//
//
//
//
//			break;
//
//
//
//		default:
//			botones_init();
//			break;
//
//	}


}
