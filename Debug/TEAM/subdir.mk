################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../TEAM/Contexto_team.c \
../TEAM/Herramientas_team.c \
../TEAM/Team.c 

OBJS += \
./TEAM/Contexto_team.o \
./TEAM/Herramientas_team.o \
./TEAM/Team.o 

C_DEPS += \
./TEAM/Contexto_team.d \
./TEAM/Herramientas_team.d \
./TEAM/Team.d 


# Each subdirectory must supply rules for building sources it contributes
TEAM/%.o: ../TEAM/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


