/*
 * Maquina de estados de detección de botones
 *
 * */

#include "botones.h"
#include "macros.h"
#include "inicioUc.h"
#include "display.h"
#include "lcd.h"
#include "control.h"

#include <avr/io.h>

/* VARIABLES EXTERNAS*/

struct_botones botones;
extern float flTempMedida;
extern unsigned char uchEstadoControl;
extern int mode;
extern int mode_aux;
extern unsigned int t_seg;

int boton_up=1;
int boton_down=0;
int boton_exit=0;


void deteccion_botones (void) {	/* Funcion de inicio de los puertos de entrada/salida */

	switch (botones.estado){

	case esperando_boton:

		if ((BOT_PIN & (SET<<BOT_ON_OFF_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_ON_OFF;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_RUN_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_RUN;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_UP_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_UP;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_DOWN_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_DOWN;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_EXIT_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_EXIT;
			botones.timeout_debouncer = timeout_botones_ms;
		}



		break;

	case boton_en_deteccion:
		if(botones.timeout_debouncer == 0){
			// Si el botón sigue presionado
			if((BOT_PIN & (SET<<BOT_ON_OFF_PIN)) == 0 || (BOT_PIN & (SET<<BOT_RUN_PIN)) == 0 || (BOT_PIN & (SET<<BOT_UP_PIN)) == 0 || (BOT_PIN & (SET<<BOT_DOWN_PIN)) == 0 || (BOT_PIN & (SET<<BOT_EXIT_PIN)) == 0){
				botones.estado = boton_en_deteccion2;
			}
			else{
				botones.estado = esperando_boton;
				init_botones ();
			}
		}
		break;

	case boton_en_deteccion2:
		if(botones.timeout_debouncer == 0){
			// Espero a que levante el dedo para confirmar la detección
			if((BOT_PIN & (SET<<BOT_ON_OFF_PIN)) != 0 && (BOT_PIN & (SET<<BOT_RUN_PIN)) != 0 && (BOT_PIN & (SET<<BOT_UP_PIN)) != 0 && (BOT_PIN & (SET<<BOT_DOWN_PIN)) != 0 && (BOT_PIN & (SET<<BOT_EXIT_PIN)) != 0){
				botones.estado = boton_detectado;
				botones.timeout_debouncer = timeout_botones_ms;
			}

		}
		break;


	default:
		break;
	}
}


void atender_botones (void)
{
	if (botones.estado == boton_detectado && botones.timeout_debouncer == 0)
	{
		if (botones.boton_nro == boton_ON_OFF){
			flTempMedida=25;
			uchEstadoControl=ESTADO_MENU;
			mode=MODE_MENU;
		}

		if (botones.boton_nro == boton_RUN && mode!=MODE_STANDBY && mode!=MODE_AUTO){
			// levantar el estado correspondiente, para que lo tome control.c
			if(mode==MODE_MANUAL)
			{
				uchEstadoControl=ESTADO_MANUAL_CONTROL;
			}

			if(mode_aux==MODE_AUX_AUTO)
			{
				uchEstadoControl=ESTADO_AUTO;
			}
			if(mode_aux==MODE_AUX_MANUAL)
			{
				uchEstadoControl=ESTADO_MANUAL;
			}

		}


		if (botones.boton_nro == boton_UP && mode!=MODE_STANDBY && mode!=MODE_AUTO){
			boton_up=1;
		}

		if (botones.boton_nro == boton_DOWN && mode!=MODE_STANDBY && mode!=MODE_AUTO){
			boton_down=1;
		}


		if (botones.boton_nro == boton_EXIT){
			if(mode==MODE_MENU)
			{
				uchEstadoControl=ESTADO_STANDBY;
				mode=MODE_STANDBY;
			}
			if(mode==MODE_AUTO || mode==MODE_MANUAL || mode==MODE_MANUAL_CONTROL)
			{
				uchEstadoControl=ESTADO_MENU;
				mode=MODE_MENU;
				mode_aux=MODE_AUX_AUTO;
				CLEAR_BIT(SALIDA_DIG_PORT, SALIDA_DIG_1); // Apago el horno, es decir dejo el rele abierto
			}
			boton_up=1;
			t_seg=0;
		}


	init_botones(); // Reinicio la botonera
	}
}



void init_botones (void){
	botones.estado = esperando_boton;
	botones.boton_nro = boton_NULL;
	botones.timeout_debouncer = 0;
}




