################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Pokebola/Pokebola.c 

OBJS += \
./Pokebola/Pokebola.o 

C_DEPS += \
./Pokebola/Pokebola.d 


# Each subdirectory must supply rules for building sources it contributes
Pokebola/%.o: ../Pokebola/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


