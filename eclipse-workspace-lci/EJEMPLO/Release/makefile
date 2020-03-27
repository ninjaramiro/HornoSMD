################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Debug/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
EJEMPLO.lss \

FLASH_IMAGE += \
EJEMPLO.hex \

EEPROM_IMAGE += \
EJEMPLO.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: EJEMPLO.elf secondary-outputs

# Tool invocations
EJEMPLO.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,EJEMPLO.map -mmcu=atmega32 -o "EJEMPLO.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

EJEMPLO.lss: EJEMPLO.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S EJEMPLO.elf  >"EJEMPLO.lss"
	@echo 'Finished building: $@'
	@echo ' '

EJEMPLO.hex: EJEMPLO.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex EJEMPLO.elf  "EJEMPLO.hex"
	@echo 'Finished building: $@'
	@echo ' '

EJEMPLO.eep: EJEMPLO.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex EJEMPLO.elf  "EJEMPLO.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: EJEMPLO.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 EJEMPLO.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) EJEMPLO.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents

-include ../makefile.targets
