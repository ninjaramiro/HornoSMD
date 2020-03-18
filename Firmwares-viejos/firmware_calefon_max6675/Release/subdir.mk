################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ADC.c \
../USART0.c \
../caudales.c \
../control.c \
../ds18b20.c \
../eeprom.c \
../inicio.c \
../lcd.c \
../main.c \
../max6675.c \
../motorPP.c \
../terminal.c \
../utiles.c 

OBJS += \
./ADC.o \
./USART0.o \
./caudales.o \
./control.o \
./ds18b20.o \
./eeprom.o \
./inicio.o \
./lcd.o \
./main.o \
./max6675.o \
./motorPP.o \
./terminal.o \
./utiles.o 

C_DEPS += \
./ADC.d \
./USART0.d \
./caudales.d \
./control.d \
./ds18b20.d \
./eeprom.d \
./inicio.d \
./lcd.d \
./main.d \
./max6675.d \
./motorPP.d \
./terminal.d \
./utiles.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega328p -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


