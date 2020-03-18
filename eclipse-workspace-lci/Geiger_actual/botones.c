/*
 * Maquina de estados de detección de botones
 *
 * */

#include "botones.h"
#include "macros.h"
#include "inicioUc.h"
#include "display.h"
#include "lcd.h"

#include <avr/io.h>
//#include <inttypes.h>
//#include <avr/sleep.h>

/* VARIABLES EXTERNAS*/
extern char 		chBuffer2[20];
struct_botones botones;


extern volatile uint16_t u16_intervalo_s; // Variable que define el intervalo de conteo.
extern volatile uint16_t u16_intervalo; // Variable que define el intervalo de conteo.
extern volatile uint16_t u16_tiempo_s; // Variable que define el tiempo transcurrido, cuenta regresiva
extern volatile uint8_t  u8_unidad;  // Unidad usada para definir el intervalo
extern volatile uint16_t u16_pulsos_en_intervalo; // Acumulacion de pulsos detectados
extern volatile uint16_t u16Cont_ms;
extern volatile uint8_t flag_stop;




void deteccion_botones (void) {	/* Funcion de inicio de los puertos de entrada/salida */

	switch (botones.estado){

	case esperando_boton:

		if ((BOT_PIN & (SET<<BOT_DISP_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_DISP;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_UP_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_UP;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_RESET_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_RESET;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_DOWN_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_DOWN;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_PARAM_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_PARAM;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_INJ_ERR_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_INJ;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		if ((BOT_PIN & (SET<<BOT_RUN_STOP_PIN)) == 0) {
			botones.estado = boton_en_deteccion;
			botones.boton_nro = boton_RUN_STOP;
			botones.timeout_debouncer = timeout_botones_ms;
		}

		break;

	case boton_en_deteccion:
		if(botones.timeout_debouncer == 0){
			// Si el botón sigue presionado
			if((BOT_PIN & (SET<<BOT_DISP_PIN)) == 0 || (BOT_PIN & (SET<<BOT_UP_PIN)) == 0 || (BOT_PIN & (SET<<BOT_RESET_PIN)) == 0 || (BOT_PIN & (SET<<BOT_DOWN_PIN)) == 0 || (BOT_PIN & (SET<<BOT_PARAM_PIN)) == 0 || (BOT_PIN & (SET<<BOT_INJ_ERR_PIN)) == 0 || (BOT_PIN & (SET<<BOT_RUN_STOP_PIN)) == 0){
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
			if((BOT_PIN & (SET<<BOT_DISP_PIN)) != 0 && (BOT_PIN & (SET<<BOT_UP_PIN)) != 0 && (BOT_PIN & (SET<<BOT_RESET_PIN)) != 0 && (BOT_PIN & (SET<<BOT_DOWN_PIN)) != 0 && (BOT_PIN & (SET<<BOT_PARAM_PIN)) != 0 && (BOT_PIN & (SET<<BOT_INJ_ERR_PIN)) != 0 && (BOT_PIN & (SET<<BOT_RUN_STOP_PIN)) != 0){
				botones.estado = boton_detectado;
				botones.timeout_debouncer = timeout_botones_ms;
			}

		}
		break;


	default:
		break;
	}
}

void atender_botones (void) {


	if (botones.estado == boton_detectado && botones.timeout_debouncer == 0){

					if (botones.boton_nro == boton_UP){
						if (u16_intervalo < 120){
							u16_intervalo++;

						}
					}

					if (botones.boton_nro == boton_DOWN){
						if (u16_intervalo > 1){
							u16_intervalo--;

						}
					}

					if (botones.boton_nro == boton_PARAM){
						if (u8_unidad == SEGUNDOS){
							u8_unidad = MINUTOS;
						}
						else if (u8_unidad == MINUTOS){
							u8_unidad = HORAS;
						}
						else{
							u8_unidad = SEGUNDOS;
						}
					}

					if (botones.boton_nro == boton_RUN_STOP){
						if (flag_stop == 0){
							flag_stop = 1;
						}
						else{
							flag_stop = 0;
						}
					}


					// Botones que resetean el conteo
					if(botones.boton_nro == boton_PARAM || botones.boton_nro == boton_DOWN || botones.boton_nro == boton_UP || botones.boton_nro == boton_RESET){
						if(u8_unidad == SEGUNDOS){
							u16_intervalo_s = u16_intervalo;
						}
						else if (u8_unidad == MINUTOS){
							u16_intervalo_s = u16_intervalo * 60;
						}
						else{
							if (u16_intervalo > 18){
								u16_intervalo = 18;
							}
							u16_intervalo_s = u16_intervalo * 3600;
						}
						u16_tiempo_s = u16_intervalo_s;  // Hubo cambios en el intervalo de conteo entonces reseteo el contador.
						u16Cont_ms = 1000;	// Para que resetee correctamente el primer segundo
						u16_pulsos_en_intervalo = 0;
					}
	LCDClear();
	init_botones(); // Reinicio la botonera
	}

	// Detengo el conteo si tengo el flag de stop
	if (flag_stop == 1){
		u16_tiempo_s = u16_intervalo_s;  // Hubo cambios en el intervalo de conteo entonces reseteo el contador.
		u16Cont_ms = 1000;	// Para que resetee correctamente el primer segundo
		u16_pulsos_en_intervalo = 0;
	}



}

	void init_botones (void){
		botones.estado = esperando_boton;
		botones.boton_nro = boton_NULL;
		botones.timeout_debouncer = 0;
}




