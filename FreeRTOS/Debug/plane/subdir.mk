################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../plane/nclink.c \
../plane/transform.c 

OBJS += \
./plane/nclink.o \
./plane/transform.o 

C_DEPS += \
./plane/nclink.d \
./plane/transform.d 


# Each subdirectory must supply rules for building sources it contributes
plane/%.o plane/%.su plane/%.cyclo: ../plane/%.c plane/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../LCD -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../BSP -I"F:/ygh20/workspace/FreeRTOS/plane" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-plane

clean-plane:
	-$(RM) ./plane/nclink.cyclo ./plane/nclink.d ./plane/nclink.o ./plane/nclink.su ./plane/transform.cyclo ./plane/transform.d ./plane/transform.o ./plane/transform.su

.PHONY: clean-plane

