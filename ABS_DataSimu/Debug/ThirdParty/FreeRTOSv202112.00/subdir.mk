################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ThirdParty/FreeRTOSv202112.00/croutine.c \
../ThirdParty/FreeRTOSv202112.00/event_groups.c \
../ThirdParty/FreeRTOSv202112.00/list.c \
../ThirdParty/FreeRTOSv202112.00/queue.c \
../ThirdParty/FreeRTOSv202112.00/stream_buffer.c \
../ThirdParty/FreeRTOSv202112.00/tasks.c \
../ThirdParty/FreeRTOSv202112.00/timers.c 

OBJS += \
./ThirdParty/FreeRTOSv202112.00/croutine.o \
./ThirdParty/FreeRTOSv202112.00/event_groups.o \
./ThirdParty/FreeRTOSv202112.00/list.o \
./ThirdParty/FreeRTOSv202112.00/queue.o \
./ThirdParty/FreeRTOSv202112.00/stream_buffer.o \
./ThirdParty/FreeRTOSv202112.00/tasks.o \
./ThirdParty/FreeRTOSv202112.00/timers.o 

C_DEPS += \
./ThirdParty/FreeRTOSv202112.00/croutine.d \
./ThirdParty/FreeRTOSv202112.00/event_groups.d \
./ThirdParty/FreeRTOSv202112.00/list.d \
./ThirdParty/FreeRTOSv202112.00/queue.d \
./ThirdParty/FreeRTOSv202112.00/stream_buffer.d \
./ThirdParty/FreeRTOSv202112.00/tasks.d \
./ThirdParty/FreeRTOSv202112.00/timers.d 


# Each subdirectory must supply rules for building sources it contributes
ThirdParty/FreeRTOSv202112.00/%.o ThirdParty/FreeRTOSv202112.00/%.su: ../ThirdParty/FreeRTOSv202112.00/%.c ThirdParty/FreeRTOSv202112.00/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"/home/anhbt/PROJECT/Project/STM32/ABS_DataSimu/ThirdParty/FreeRTOSv202112.00" -I"/home/anhbt/PROJECT/Project/STM32/ABS_DataSimu/ThirdParty/FreeRTOSv202112.00/include" -I"/home/anhbt/PROJECT/Project/STM32/ABS_DataSimu/ThirdParty/FreeRTOSv202112.00/portable/GCC/ARM_CM3" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-ThirdParty-2f-FreeRTOSv202112-2e-00

clean-ThirdParty-2f-FreeRTOSv202112-2e-00:
	-$(RM) ./ThirdParty/FreeRTOSv202112.00/croutine.d ./ThirdParty/FreeRTOSv202112.00/croutine.o ./ThirdParty/FreeRTOSv202112.00/croutine.su ./ThirdParty/FreeRTOSv202112.00/event_groups.d ./ThirdParty/FreeRTOSv202112.00/event_groups.o ./ThirdParty/FreeRTOSv202112.00/event_groups.su ./ThirdParty/FreeRTOSv202112.00/list.d ./ThirdParty/FreeRTOSv202112.00/list.o ./ThirdParty/FreeRTOSv202112.00/list.su ./ThirdParty/FreeRTOSv202112.00/queue.d ./ThirdParty/FreeRTOSv202112.00/queue.o ./ThirdParty/FreeRTOSv202112.00/queue.su ./ThirdParty/FreeRTOSv202112.00/stream_buffer.d ./ThirdParty/FreeRTOSv202112.00/stream_buffer.o ./ThirdParty/FreeRTOSv202112.00/stream_buffer.su ./ThirdParty/FreeRTOSv202112.00/tasks.d ./ThirdParty/FreeRTOSv202112.00/tasks.o ./ThirdParty/FreeRTOSv202112.00/tasks.su ./ThirdParty/FreeRTOSv202112.00/timers.d ./ThirdParty/FreeRTOSv202112.00/timers.o ./ThirdParty/FreeRTOSv202112.00/timers.su

.PHONY: clean-ThirdParty-2f-FreeRTOSv202112-2e-00

