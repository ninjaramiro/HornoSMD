
#ifndef _BLINKY_H_
#define _BLINKY_H_

void blinky_init(void);
void blinky(void);



// Definición de estados de la máquina de estados
#define LED_FIJO_APAGADO 0
#define LED_FIJO_ENCENDIDO 1
#define LED_TITILANDO_APAGADO 2
#define LED_TITILANDO_ENCENDIDO 3

#define TIEMPO_PARPADEO		1000


#endif
