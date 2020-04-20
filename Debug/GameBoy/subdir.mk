################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../GameBoy/Contexto_gameboy.c \
../GameBoy/GameBoy.c \
../GameBoy/Herramientas_gameboy.c 

OBJS += \
./GameBoy/Contexto_gameboy.o \
./GameBoy/GameBoy.o \
./GameBoy/Herramientas_gameboy.o 

C_DEPS += \
./GameBoy/Contexto_gameboy.d \
./GameBoy/GameBoy.d \
./GameBoy/Herramientas_gameboy.d 


# Each subdirectory must supply rules for building sources it contributes
GameBoy/%.o: ../GameBoy/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


