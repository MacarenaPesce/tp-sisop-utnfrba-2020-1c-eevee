################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Broker/Broker.c \
../Broker/Contexto_broker.c \
../Broker/Herramientas_broker.c 

OBJS += \
./Broker/Broker.o \
./Broker/Contexto_broker.o \
./Broker/Herramientas_broker.o 

C_DEPS += \
./Broker/Broker.d \
./Broker/Contexto_broker.d \
./Broker/Herramientas_broker.d 


# Each subdirectory must supply rules for building sources it contributes
Broker/%.o: ../Broker/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


