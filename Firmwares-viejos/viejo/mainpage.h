/** \file	mainpage.h
*	\mainpage	Firmware de disposotivo de localizacion por GPS a traves de la red GSM.
*
*	\section sec1 Introduccion
*	Este documento detalla el desarrollo del firmeware. \n
* 	El mismo corre en un microcontrolador ATMEGA128. \n
*	El tabulado utilizado en el Notepad es de 4.
*
*	\b Configuracion \b de \b fusibles: \n
*	- Low Fuse = 0xAF	 	BODLEVEL=1,BODEN=0 ,SUT1=1 ,SUT0=0 ,CKSEL3=1,CKSEL2=1 ,CKSEL1=1 ,CKSEL0=1
*	- High Fuse = 0xC9	 	OCDEN=1   ,JTAGEN=1,SPIEN=0,CKOPT=0,EESAVE=1,BOOTSZ1=0,BOOTSZ0=0,BOOTRST=1
*	- Extendet Fuse = 0xFF	-         ,-       ,-      ,-      ,-       ,-        ,M103C=1  ,WDTON=1
*
*	- Lock Fuse = 0xFC	(bit 7 y 6 se leen como 0 por lo que se lee 3C)
*	- Calibration = 0xADADADAD
*
*	- avrdude -p m128 -c usbtiny -U efuse:w:0xFF:m -U hfuse:w:0xC9:m -U lfuse:w:0xaf:m
*	- avrdude -p m128 -c usbtiny flash:w:conductimetro.hex
*	- avrdude -p m128 -c usbtiny -U lock:w:0x3C:m
*
*	\subsection Revisiones de Firmware
*
*	-V1.00
*
*	\section sec2 Estructura del programa
*	- main.c
*	- mainpage.h
*	- macros.h
*	- eeprom.h
*	- eeprom.c
*	- medicion_ADC.h
*	- medicion_ADC.c
*	- inicioUc.h
*	- inicioUc.c
*	- USART0.c
*	- USART0.h
*	- alarmas.c
*	- alarmas.h
*	- custom_char.c
*	- discretas.c
*	- discretas.h
*	- display.c
*	- display.h
*	- lcd.c
*	- lcd.h
*	- PWM420mA.c
*	- PWM420mA.h
*	- terminal.c
*	- terminal.h
*	- utiles.c
*	- utiles.h
*
*	\section sec3 Descripcion general de funcionamiento
*
*	El equipoes un conductimetro de dos canales, canal 1 de 0-20 uS/cm y canal 2 de 20-2500 uS/cm.
*	Cada canal tiene asociada una salida 4-20 mA y a su vez, de conectarse un display de 2 lineas por
*	16 caracteres, muestra en cada renglon la midicion actual de cada conductimetro.
*	Cada canal tiene asociado una alarma, esta solo es configurable por terminal, TTL 38400kps 8N1.
*	Para activar la alarma se debe configurar su valor, de lo contrario se deja en 0 para desactivarla.
*	A cada alarma se le puede configurar el tiempo de permanencia en el estado para que la misma se active
*	o se desactive segun corresponda.
*	La calibracion de cada conductimetro se realiza por terminal, indicando por comando el valor real que
*	deberia estar midiendo el instrumento.
*	
*
*	\section sec4 Comandos por Terminal
*
*	-Comando:	"CAL COND x <valor>" 	Calibra el valor uS/cm que existe actualmente en el conductimetro para corregir la k de calibracion.
*				"CAL COND x?"			x: 1, 2.
*										<valor>: xxx.xx, valor en uS/cm. Canal 1 [0.0-20.0], Canal 2 [20.0-2500.0]
*										<k>: constante de calibracion.
*										<f>: valor de offset para correccion de cero.
*	Responde:	"CAL COND 1: <k> ; OFFSET: <f>"
*				"CAL COND 2: <k>"
*				"CAL COND x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"CAL PWM x <valor>" 	Calibra el valor mA que existe actualmente en el lazo 4-20mA para corregir la k de calibracion.
*				"CAL PWM x?"			x: 1, 2.
*										<valor>: xxx.xx, valor en mA. [4.0-20.0]
*										<k>: constante de calibracion.
*	Responde:	"CAL PWM x: <k>"
*				"CAL PWM x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"SET COND x <k>" 		Graba el valor de k de calibracion.
*				"SET COND x?"			x: 1, 2.
*										<k>: xx.xx constante de calibracion.
*	Responde:	"SET COND x: <k>"
*				"SET COND x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"SET PWM x <k>" 		Graba el valor de k de calibracion.
*				"SET PWM x?"			x: 1, 2.
*										<k>: xx.xx constante de calibracion.
*	Responde:	"SET PWM x: <k>"
*				"SET PWM x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"SET OFFSET 1 <valor>" 	Graba el valor uS/cm de offset para corregir el cero.
*				"SET OFFSET 1?"			<valor>: xxx.xx, valor en uS/cm.
*										<k>: constante de calibracion.
*										<f>: valor de offset para correccion de cero.
*	Responde:	"CAL COND 1: <k> ; OFFSET: <f>"
*				"SET OFFSET 1: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"ALARMA x <valor>" 	Valor en uS/cm para la activacion/desactivacion de la alarma, en 0 queda apagada.
*				"ALARMA x?"			x: 1, 2.
*									<valor>: xxxx.xx, valor en uS/cm. Canal 1 [0.0-20.0], Canal 2 [20.0-2500.0]
*	Responde:	"ALARMA x: <valor>"
*				"ALARMA x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"TIEMPO ALARMA x <valor>" 	Tiempo en mseg. de permanencia en la condicion para la activacion/desactivacion de la alarma.
*				"TIEMPO ALARMA x?"			x: 1, 2.
*											<valor>: xxxxx, valor en mseg. [1000-60000]
*	Responde:	"TIEMPO ALARMA x: <valor>"
*				"TIEMPO ALARMA x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"GANANCIA PWM x <valor>"	Ganancia proporcional en veces del lazo de control del PWM de la salida 4-20mA.
*				"GANANCIA PWM x?"			x: 1, 2.
*											<valor>: xxxx, valor de ganacia proporcional del lazo de control. [1-1000]
*	Responde:	"GANANCIA PWM x: <valor>"
*				"GANANCIA PWM x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"FILTRO COND x <valor>"	Ancho del filtro en cantidad de muestras de medicion de corriente del PWM de la salida 4-20mA.
*				"FILTRO COND x?"		x: 1, 2.
*										<valor>: xxxx, ancho del filtro en cantidad de muestras. [1-10000]
*	Responde:	"FILTRO COND x: <valor>"
*				"FILTRO COND x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"FILTRO PWM x <valor>"	Ancho del filtro de medicion de corriente del PWM de la salida 4-20mA.
*				"FILTRO PWM x?"			x: 1, 2.
*										<valor>: xxxx, ancho del filtro en cantidad de muestras. [1-10000]
*	Responde:	"FILTRO PWM x: <valor>"
*				"FILTRO PWM x: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"INFO" Consulta informacion sobre el equipo.
*				"INFO?"
*	Responde:	"INFO: <empresa \n\r eslogan \n\r nombre dispositivo \n\r version del firmware \n\r numero de serie>"
*
*
*	-Comando:	"PRINT <parametro>"	Imprime el parametro solicitado, se interrumpe la impresion presionando enter.
*									<parametro>: DISCRETAS, ADC.
*	Responde:	<comienza la impresion>
*				"PRINT: PARAMETRO DESCONOCIDO"
*
*
*	-Comando:	"FABRICA RESET"	Lleva todos los valores a fabrica, incluso las calibraciones.
*	Responde:	"FABRICA RESET: OK"
*
*
*	-Comando:	"RESET"	Reinicia el equipo.
*
*/
