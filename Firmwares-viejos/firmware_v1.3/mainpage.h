/** \file	mainpage.h
*	\mainpage	Firmware de conductimetro de dos canales.
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
*	- avrdude -p m128 -c usbtiny -Uflash:w:conductimetro.hex:a
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
*	El equipoes un conductimetro de dos canales, canal 1 y canal 2 de 0-20 uS/cm, 2-200 uS/cm y de 20-2500 uS/cm.
*	Cada canal tiene asociada una salida 4-20 mA y a su vez, de conectarse un display de 2 lineas por
*	16 caracteres, muestra en cada renglon la midicion actual de cada conductimetro.
*	Cada canal tiene asociado una alarma, esta solo es configurable por terminal, TTL 38400kps 8N1.
*	Para activar la alarma se debe configurar su valor, de lo contrario se deja en 0 para desactivarla.
*	A cada alarma se le puede configurar el tiempo de permanencia en el estado para que la misma se active
*	o se desactive segun corresponda.
*	La calibracion de cada conductimetro se realiza por terminal, indicando por comando el valor real que
*	deberia estar midiendo el instrumento.
*	
*/
