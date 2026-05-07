################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/Base/Error.c \
../Src/Base/System.c \
../Src/Base/Times.c 

OBJS += \
./Src/Base/Error.o \
./Src/Base/System.o \
./Src/Base/Times.o 

C_DEPS += \
./Src/Base/Error.d \
./Src/Base/System.d \
./Src/Base/Times.d 


# Each subdirectory must supply rules for building sources it contributes
Src/Base/%.o Src/Base/%.su Src/Base/%.cyclo: ../Src/Base/%.c Src/Base/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DSTM32 -DSTM32G4 -DSTM32G474RETx -c -I../Inc -I"/home/dawid/Desktop/SIROBOTICS/GIT/CanOpen-ECSS-Si-Robotics/examples/actuator/Drivers/CMSIS/Include" -I"/home/dawid/Desktop/SIROBOTICS/GIT/CanOpen-ECSS-Si-Robotics/examples/actuator/Drivers/CMSIS/Device/ST/STM32G4xx/Include" -I"/home/dawid/Desktop/SIROBOTICS/GIT/CanOpen-ECSS-Si-Robotics/examples/actuator/Inc/Base" -I"/home/dawid/Desktop/SIROBOTICS/GIT/CanOpen-ECSS-Si-Robotics/examples/actuator/Inc/Interface" -I"/home/dawid/Desktop/SIROBOTICS/GIT/CanOpen-ECSS-Si-Robotics/examples/actuator/Inc/Hardware" -O0 -ffunction-sections -fdata-sections -Wall -Wextra -Werror -pedantic -pedantic-errors -Wmissing-include-dirs -Wswitch-default -Wswitch-enum -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Src-2f-Base

clean-Src-2f-Base:
	-$(RM) ./Src/Base/Error.cyclo ./Src/Base/Error.d ./Src/Base/Error.o ./Src/Base/Error.su ./Src/Base/System.cyclo ./Src/Base/System.d ./Src/Base/System.o ./Src/Base/System.su ./Src/Base/Times.cyclo ./Src/Base/Times.d ./Src/Base/Times.o ./Src/Base/Times.su

.PHONY: clean-Src-2f-Base

