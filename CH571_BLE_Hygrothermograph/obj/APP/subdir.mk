################################################################################
# 自动生成的文件。不要编辑！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../APP/I2C.c \
../APP/SHT4X.c \
../APP/broadcaster.c \
../APP/broadcaster_main.c 

OBJS += \
./APP/I2C.o \
./APP/SHT4X.o \
./APP/broadcaster.o \
./APP/broadcaster_main.o 

C_DEPS += \
./APP/I2C.d \
./APP/SHT4X.d \
./APP/broadcaster.d \
./APP/broadcaster_main.d 


# Each subdirectory must supply rules for building sources it contributes
APP/%.o: ../APP/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Startup" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\APP\include" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Profile\include" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\StdPeriphDriver\inc" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\HAL\include" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Ld" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\LIB" -I"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

