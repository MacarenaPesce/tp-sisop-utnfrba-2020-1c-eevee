################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Broker.c \
../Cliente.c \
../ColaMensajes.c \
../Conexion.c \
../Contexto_broker.c \
../Herramientas_broker.c \
../Memoria.c \
../utils.c 

OBJS += \
./Broker.o \
./Cliente.o \
./ColaMensajes.o \
./Conexion.o \
./Contexto_broker.o \
./Herramientas_broker.o \
./Memoria.o \
./utils.o 

C_DEPS += \
./Broker.d \
./Cliente.d \
./ColaMensajes.d \
./Conexion.d \
./Contexto_broker.d \
./Herramientas_broker.d \
./Memoria.d \
./utils.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2020-1c-eevee/Pokebola" -IPokebola -Ipthread -Icommons -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


