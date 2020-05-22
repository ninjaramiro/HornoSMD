/*
 * control.h
 *
 *  Created on: 7 may. 2020
 *      Author: matu
 */


#define ESTADO_INICIALIZACION 	1
#define ESTADO_STANDBY 			2
#define ESTADO_CALENTAR 		3
#define ESTADO_ENFRIAR 			4
#define ESTADO_AUTO 			5 // Estado automatico, es decir que se lleva a cabo la curva de temperatura predefinida.
#define ESTADO_RAMP_TO_SOAK 	6
#define ESTADO_PREHEAT 			7
#define ESTADO_RAMP_TO_PEAK 	8
#define ESTADO_REFLOW_UP		9
#define ESTADO_REFLOW_DOWN		10
#define ESTADO_COOLING			11
#define ESTADO_MENU				12
#define ESTADO_MANUAL			13
#define ESTADO_EXIT				14
#define ESTADO_MANUAL_CONTROL	15


#define TIEMPO_RAMP_TO_SOAK 	1000 // 1°C/seg
#define TIEMPO_PREHEAT 			60 // 60 segundos
#define TIEMPO_PREHEAT_DELAY	1000
#define TIEMPO_RAMP_TO_PEAK 	1000 // 1°C/seg
#define TIEMPO_REFLOW_UP		1000
#define TIEMPO_REFLOW_DOWN		500
#define TIEMPO_COOLING			500 // 2°C/seg, osea baja 1°C cada 1/2 seg
#define TIEMPO_PREGUNTA_MANUAL	1000

#define TEMP_PREHEAT 			150
#define TEMP_RAMP_TO_PEAK_MAX	217
#define TEMP_REFLOW_MAX			250
#define TEMP_COOLING_MIN		70
#define TEMP_MAX				300

//#define COOLING 				1
#define NOT_COOLING 			0

#define MODE_STANDBY 			0
#define MODE_AUTO				1
#define MODE_COOLING 			2
#define MODE_MENU 				3
#define MODE_MANUAL				4
#define MODE_MANUAL_CONTROL		5

#define MODE_AUX_AUTO 			0
#define MODE_AUX_MANUAL 		1
#define MODE_AUX_WAIT			3


void control_temperatura(void);
void control_init(void);
