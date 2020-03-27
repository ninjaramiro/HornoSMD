
#ifndef _BOTONES_H_
#define _BOTONES_H_

void botones_init(void);
void botones(void);



// Definición de estados de la máquina de estados
#define ESPERANDO_BOTON 0
#define ESPERANDO_DEBOUNCER 1
#define DETECCION_BOTON 2

#define TIEMPO_DEBOUNCER		1000


#endif
