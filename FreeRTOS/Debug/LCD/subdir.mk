################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (11.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LCD/gui.c \
../LCD/lcd.c 

OBJS += \
./LCD/gui.o \
./LCD/lcd.o 

C_DEPS += \
./LCD/gui.d \
./LCD/lcd.d 


# Each subdirectory must supply rules for building sources it contributes
LCD/%.o LCD/%.su LCD/%.cyclo: ../LCD/%.c LCD/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xE -c -I../Core/Inc -I../LCD -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM3 -I../BSP -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-LCD

clean-LCD:
	-$(RM) ./LCD/gui.cyclo ./LCD/gui.d ./LCD/gui.o ./LCD/gui.su ./LCD/lcd.cyclo ./LCD/lcd.d ./LCD/lcd.o ./LCD/lcd.su

.PHONY: clean-LCD

