################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Broker.c \
../Cliente.c \
../Conexion.c \
../Contexto_broker.c \
../Herramientas_broker.c \
../Memoria.c \
../utils.c 

OBJS += \
./Broker.o \
./Cliente.o \
./Conexion.o \
./Contexto_broker.o \
./Herramientas_broker.o \
./Memoria.o \
./utils.o 

C_DEPS += \
./Broker.d \
./Cliente.d \
./Conexion.d \
./Contexto_broker.d \
./Herramientas_broker.d \
./Memoria.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


