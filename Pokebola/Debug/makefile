################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: libPokebola.so

dependents:
	-cd /home/utnso/so-commons-library/Debug && $(MAKE) clean all

# Tool invocations
libPokebola.so: $(OBJS) $(USER_OBJS) /home/utnso/so-commons-library/Debug/libcommons.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/so-commons-library/Debug" -shared -o "libPokebola.so" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(LIBRARIES)$(OBJS)$(C_DEPS) libPokebola.so
	-@echo ' '

.PHONY: all clean dependents
/home/utnso/so-commons-library/Debug/libcommons.so:

-include ../makefile.targets
