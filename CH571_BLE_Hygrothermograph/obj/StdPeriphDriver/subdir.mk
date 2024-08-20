################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../StdPeriphDriver/CH57x_adc.c \
../StdPeriphDriver/CH57x_clk.c \
../StdPeriphDriver/CH57x_flash.c \
../StdPeriphDriver/CH57x_gpio.c \
../StdPeriphDriver/CH57x_pwr.c \
../StdPeriphDriver/CH57x_sys.c \
../StdPeriphDriver/CH57x_uart1.c 

OBJS += \
./StdPeriphDriver/CH57x_adc.o \
./StdPeriphDriver/CH57x_clk.o \
./StdPeriphDriver/CH57x_flash.o \
./StdPeriphDriver/CH57x_gpio.o \
./StdPeriphDriver/CH57x_pwr.o \
./StdPeriphDriver/CH57x_sys.o \
./StdPeriphDriver/CH57x_uart1.o 

C_DEPS += \
./StdPeriphDriver/CH57x_adc.d \
./StdPeriphDriver/CH57x_clk.d \
./StdPeriphDriver/CH57x_flash.d \
./StdPeriphDriver/CH57x_gpio.d \
./StdPeriphDriver/CH57x_pwr.d \
./StdPeriphDriver/CH57x_sys.d \
./StdPeriphDriver/CH57x_uart1.d 


# Each subdirectory must supply rules for building sources it contributes
StdPeriphDriver/%.o: ../StdPeriphDriver/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Startup" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\APP\include" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Profile\include" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\StdPeriphDriver\inc" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\HAL\include" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Ld" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\LIB" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

