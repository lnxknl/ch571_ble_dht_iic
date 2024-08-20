################################################################################
# �Զ����ɵ��ļ�����Ҫ�༭��
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../HAL/MCU.c \
../HAL/RTC.c \
../HAL/SLEEP.c 

OBJS += \
./HAL/MCU.o \
./HAL/RTC.o \
./HAL/SLEEP.o 

C_DEPS += \
./HAL/MCU.d \
./HAL/RTC.d \
./HAL/SLEEP.d 


# Each subdirectory must supply rules for building sources it contributes
HAL/%.o: ../HAL/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Startup" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\APP\include" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Profile\include" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\StdPeriphDriver\inc" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\HAL\include" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Ld" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\LIB" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

