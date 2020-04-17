################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Contexto_team.c \
../Herramientas_team.c \
../Team.c 

OBJS += \
./Contexto_team.o \
./Herramientas_team.o \
./Team.o 

C_DEPS += \
./Contexto_team.d \
./Herramientas_team.d \
./Team.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -Icommons -IPokebola -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


