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
Geiger_actual.lss \

FLASH_IMAGE += \
Geiger_actual.hex \

EEPROM_IMAGE += \
Geiger_actual.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: Geiger_actual.elf secondary-outputs

# Tool invocations
Geiger_actual.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,Geiger_actual.map -mmcu=atmega32 -o "Geiger_actual.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

Geiger_actual.lss: Geiger_actual.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S Geiger_actual.elf  >"Geiger_actual.lss"
	@echo 'Finished building: $@'
	@echo ' '

Geiger_actual.hex: Geiger_actual.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex Geiger_actual.elf  "Geiger_actual.hex"
	@echo 'Finished building: $@'
	@echo ' '

Geiger_actual.eep: Geiger_actual.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex Geiger_actual.elf  "Geiger_actual.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: Geiger_actual.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 Geiger_actual.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(FLASH_IMAGE)$(ELFS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(S_DEPS)$(SIZEDUMMY)$(S_UPPER_DEPS)$(LSS)$(C_DEPS) Geiger_actual.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents

-include ../makefile.targets
