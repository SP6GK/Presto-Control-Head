################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/24LC_EEPROM.c \
../Core/Src/BASIC.c \
../Core/Src/F1956.c \
../Core/Src/GPIO_expander.c \
../Core/Src/LMX2572.c \
../Core/Src/MCP4725.c \
../Core/Src/MCP9800.c \
../Core/Src/NV_data.c \
../Core/Src/TCA9554.c \
../Core/Src/adc.c \
../Core/Src/calibration.c \
../Core/Src/crc.c \
../Core/Src/dma.c \
../Core/Src/error_handler.c \
../Core/Src/freertos.c \
../Core/Src/frequency_functions.c \
../Core/Src/globals.c \
../Core/Src/gpio.c \
../Core/Src/helper_functions.c \
../Core/Src/i2c.c \
../Core/Src/ili9341.c \
../Core/Src/ili9341_font.c \
../Core/Src/ili9341_gfx.c \
../Core/Src/main.c \
../Core/Src/pcf8574.c \
../Core/Src/spi.c \
../Core/Src/splash.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_hal_timebase_tim.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/24LC_EEPROM.o \
./Core/Src/BASIC.o \
./Core/Src/F1956.o \
./Core/Src/GPIO_expander.o \
./Core/Src/LMX2572.o \
./Core/Src/MCP4725.o \
./Core/Src/MCP9800.o \
./Core/Src/NV_data.o \
./Core/Src/TCA9554.o \
./Core/Src/adc.o \
./Core/Src/calibration.o \
./Core/Src/crc.o \
./Core/Src/dma.o \
./Core/Src/error_handler.o \
./Core/Src/freertos.o \
./Core/Src/frequency_functions.o \
./Core/Src/globals.o \
./Core/Src/gpio.o \
./Core/Src/helper_functions.o \
./Core/Src/i2c.o \
./Core/Src/ili9341.o \
./Core/Src/ili9341_font.o \
./Core/Src/ili9341_gfx.o \
./Core/Src/main.o \
./Core/Src/pcf8574.o \
./Core/Src/spi.o \
./Core/Src/splash.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_hal_timebase_tim.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/24LC_EEPROM.d \
./Core/Src/BASIC.d \
./Core/Src/F1956.d \
./Core/Src/GPIO_expander.d \
./Core/Src/LMX2572.d \
./Core/Src/MCP4725.d \
./Core/Src/MCP9800.d \
./Core/Src/NV_data.d \
./Core/Src/TCA9554.d \
./Core/Src/adc.d \
./Core/Src/calibration.d \
./Core/Src/crc.d \
./Core/Src/dma.d \
./Core/Src/error_handler.d \
./Core/Src/freertos.d \
./Core/Src/frequency_functions.d \
./Core/Src/globals.d \
./Core/Src/gpio.d \
./Core/Src/helper_functions.d \
./Core/Src/i2c.d \
./Core/Src/ili9341.d \
./Core/Src/ili9341_font.d \
./Core/Src/ili9341_gfx.d \
./Core/Src/main.d \
./Core/Src/pcf8574.d \
./Core/Src/spi.d \
./Core/Src/splash.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_hal_timebase_tim.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F411xE -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/24LC_EEPROM.cyclo ./Core/Src/24LC_EEPROM.d ./Core/Src/24LC_EEPROM.o ./Core/Src/24LC_EEPROM.su ./Core/Src/BASIC.cyclo ./Core/Src/BASIC.d ./Core/Src/BASIC.o ./Core/Src/BASIC.su ./Core/Src/F1956.cyclo ./Core/Src/F1956.d ./Core/Src/F1956.o ./Core/Src/F1956.su ./Core/Src/GPIO_expander.cyclo ./Core/Src/GPIO_expander.d ./Core/Src/GPIO_expander.o ./Core/Src/GPIO_expander.su ./Core/Src/LMX2572.cyclo ./Core/Src/LMX2572.d ./Core/Src/LMX2572.o ./Core/Src/LMX2572.su ./Core/Src/MCP4725.cyclo ./Core/Src/MCP4725.d ./Core/Src/MCP4725.o ./Core/Src/MCP4725.su ./Core/Src/MCP9800.cyclo ./Core/Src/MCP9800.d ./Core/Src/MCP9800.o ./Core/Src/MCP9800.su ./Core/Src/NV_data.cyclo ./Core/Src/NV_data.d ./Core/Src/NV_data.o ./Core/Src/NV_data.su ./Core/Src/TCA9554.cyclo ./Core/Src/TCA9554.d ./Core/Src/TCA9554.o ./Core/Src/TCA9554.su ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/calibration.cyclo ./Core/Src/calibration.d ./Core/Src/calibration.o ./Core/Src/calibration.su ./Core/Src/crc.cyclo ./Core/Src/crc.d ./Core/Src/crc.o ./Core/Src/crc.su ./Core/Src/dma.cyclo ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/dma.su ./Core/Src/error_handler.cyclo ./Core/Src/error_handler.d ./Core/Src/error_handler.o ./Core/Src/error_handler.su ./Core/Src/freertos.cyclo ./Core/Src/freertos.d ./Core/Src/freertos.o ./Core/Src/freertos.su ./Core/Src/frequency_functions.cyclo ./Core/Src/frequency_functions.d ./Core/Src/frequency_functions.o ./Core/Src/frequency_functions.su ./Core/Src/globals.cyclo ./Core/Src/globals.d ./Core/Src/globals.o ./Core/Src/globals.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/helper_functions.cyclo ./Core/Src/helper_functions.d ./Core/Src/helper_functions.o ./Core/Src/helper_functions.su ./Core/Src/i2c.cyclo ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/i2c.su ./Core/Src/ili9341.cyclo ./Core/Src/ili9341.d ./Core/Src/ili9341.o ./Core/Src/ili9341.su ./Core/Src/ili9341_font.cyclo ./Core/Src/ili9341_font.d ./Core/Src/ili9341_font.o ./Core/Src/ili9341_font.su ./Core/Src/ili9341_gfx.cyclo ./Core/Src/ili9341_gfx.d ./Core/Src/ili9341_gfx.o ./Core/Src/ili9341_gfx.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/pcf8574.cyclo ./Core/Src/pcf8574.d ./Core/Src/pcf8574.o ./Core/Src/pcf8574.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/splash.cyclo ./Core/Src/splash.d ./Core/Src/splash.o ./Core/Src/splash.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_hal_timebase_tim.cyclo ./Core/Src/stm32f4xx_hal_timebase_tim.d ./Core/Src/stm32f4xx_hal_timebase_tim.o ./Core/Src/stm32f4xx_hal_timebase_tim.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

