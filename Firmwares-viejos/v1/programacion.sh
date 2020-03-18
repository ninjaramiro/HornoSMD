#!/bin/sh

# sudo apt-get install avr-libc gcc-avr avrdude

#avr-gcc -mmcu=atmega8 -Wall main.c USART.c inicio.c  estados.c control.c configuraciones.c Botones.c otras.c -o main.bin
avr-gcc -mmcu=atmega8 -Wall inicio.c Botones.c control.c USART.c comandos.c main.c -o main.bin
avr-objcopy -j .text -O ihex main.bin main.hex
avrdude -p m8 -V -c usbtiny -U f:w:main.hex


# Fuses para el proyecto tripode
# avrdude -p m8 -c usbtiny  -U lfuse:w:0xff:m -U hfuse:w:0xd9:m
