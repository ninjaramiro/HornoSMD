/** \file	mainpage.h
 *	\mainpage	Firmware de control del calefon
 *
 *	\section sec1 Introducción
 *	Este documento detalla el desarrollo del firmeware. \n
 * 	El mismo corre en un microcontrolador ATMEGA328p. \n
 *	El desarrollo fué realizado con Eclipse AVR plugin. \n
 *
 *	\b Configuración \b de \b fusibles: \n
 *	HigByte = 11011001 = 0xD9	RSTDISBL 1, DWEN 1, SPIEN 0, WDTON 1, EESAVE 1, BOOTSZ1 0, BOOTSZ0 0, BOOTRST 1.
 *	LowByte = 11111111 = 0xFF	CKDIV8 1, CKOUT 1, SUT1 1, SUT0 1, CKSEL3 1, CKSEL2 1, CKSEL1 1, CKSEL0 1. => cristal 8MHz externo
 *
 *	- avrdude -p m328p -c usbtiny -U hfuse:w:0xd9:m -U lfuse:w:0xff:m
 *
 *
 */
