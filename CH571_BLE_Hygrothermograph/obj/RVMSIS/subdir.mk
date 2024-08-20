################################################################################
# �Զ����ɵ��ļ�����Ҫ�༭��
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../RVMSIS/core_riscv.c 

OBJS += \
./RVMSIS/core_riscv.o 

C_DEPS += \
./RVMSIS/core_riscv.d 


# Each subdirectory must supply rules for building sources it contributes
RVMSIS/%.o: ../RVMSIS/%.c
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -DDEBUG=1 -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Startup" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\APP\include" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Profile\include" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\StdPeriphDriver\inc" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\HAL\include" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Ld" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\LIB" -I"G:\��    ��Ƭ��\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\RVMSIS" -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@	@

