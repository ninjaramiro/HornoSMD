################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../USART.c \
../botones.c \
../display.c \
../inicioUc.c \
../lcd.c \
../main.c \
../medicion.c \
../spi.c 

OBJS += \
./USART.o \
./botones.o \
./display.o \
./inicioUc.o \
./lcd.o \
./main.o \
./medicion.o \
./spi.o 

C_DEPS += \
./USART.d \
./botones.d \
./display.d \
./inicioUc.d \
./lcd.d \
./main.d \
./medicion.d \
./spi.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


