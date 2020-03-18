################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../PWM420mA.c \
../USART0.c \
../USART1.c \
../alarmas.c \
../discretas.c \
../display.c \
../eeprom.c \
../inicioUc.c \
../lcd.c \
../main.c \
../medicion_ADC.c \
../terminal.c \
../utiles.c 

OBJS += \
./PWM420mA.o \
./USART0.o \
./USART1.o \
./alarmas.o \
./discretas.o \
./display.o \
./eeprom.o \
./inicioUc.o \
./lcd.o \
./main.o \
./medicion_ADC.o \
./terminal.o \
./utiles.o 

C_DEPS += \
./PWM420mA.d \
./USART0.d \
./USART1.d \
./alarmas.d \
./discretas.d \
./display.d \
./eeprom.d \
./inicioUc.d \
./lcd.d \
./main.d \
./medicion_ADC.d \
./terminal.d \
./utiles.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega128 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


