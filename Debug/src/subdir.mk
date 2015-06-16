################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/RssiFilter.cpp \
../src/at_command.cpp \
../src/main.cpp \
../src/setup.cpp 

OBJS += \
./src/RssiFilter.o \
./src/at_command.o \
./src/main.o \
./src/setup.o 

CPP_DEPS += \
./src/RssiFilter.d \
./src/at_command.d \
./src/main.d \
./src/setup.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C++ Compiler'
	arm-none-eabi-c++ -D__NEWLIB__ -D__CODE_RED -DDEBUG -DNANO51822 -I"D:\LPCXpresso_7.7.2_379\workspace\BleThroughput\src" -I"D:\LPCXpresso_7.7.2_379\workspace\uCXpresso.NRF\inc" -O2 -Os -Wall -c -fmessage-length=0 -fno-builtin -ffunction-sections -fdata-sections -fno-rtti -fno-exceptions -mcpu=cortex-m0 -mthumb -D__NEWLIB__ -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


