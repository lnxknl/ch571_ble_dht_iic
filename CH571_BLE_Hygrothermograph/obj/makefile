################################################################################
# 自动生成的文件。不要编辑！
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include StdPeriphDriver/subdir.mk
-include Startup/subdir.mk
-include RVMSIS/subdir.mk
-include Profile/subdir.mk
-include HAL/subdir.mk
-include APP/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
SECONDARY_FLASH += \
CH571_BLE_Hygrothermograph.hex \

SECONDARY_LIST += \
CH571_BLE_Hygrothermograph.lst \

SECONDARY_SIZE += \
CH571_BLE_Hygrothermograph.siz \


# 所有目标
all: CH571_BLE_Hygrothermograph.elf secondary-outputs

# 工具调用
CH571_BLE_Hygrothermograph.elf: $(OBJS) $(USER_OBJS)
	@	@	riscv-none-embed-gcc -march=rv32imac -mabi=ilp32 -mcmodel=medany -msmall-data-limit=8 -mno-save-restore -Os -fsigned-char -ffunction-sections -fno-common  -g -T "G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\Ld\Link.ld" -nostartfiles -Xlinker --gc-sections -L"../" -L"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\HAL\include" -L"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\LIB" -L"G:\╄    单片机\WCH\CH573\Projects\CH571_BLE_Hygrothermograph\StdPeriphDriver" -Xlinker --print-memory-usage -Wl,-Map,"CH571_BLE_Hygrothermograph.map" --specs=nano.specs --specs=nosys.specs -o "CH571_BLE_Hygrothermograph.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@	@
CH571_BLE_Hygrothermograph.hex: CH571_BLE_Hygrothermograph.elf
	@	riscv-none-embed-objcopy -O ihex "CH571_BLE_Hygrothermograph.elf"  "CH571_BLE_Hygrothermograph.hex"
	@	@
CH571_BLE_Hygrothermograph.lst: CH571_BLE_Hygrothermograph.elf
	@	riscv-none-embed-objdump --source --all-headers --demangle --line-numbers --wide "CH571_BLE_Hygrothermograph.elf" > "CH571_BLE_Hygrothermograph.lst"
	@	@
CH571_BLE_Hygrothermograph.siz: CH571_BLE_Hygrothermograph.elf
	@	riscv-none-embed-size --format=berkeley "CH571_BLE_Hygrothermograph.elf"
	@	@
# 其他目标
clean:
	-$(RM) $(ASM_UPPER_DEPS)$(OBJS)$(SECONDARY_FLASH)$(SECONDARY_LIST)$(SECONDARY_SIZE)$(ASM_DEPS)$(S_DEPS)$(S_UPPER_DEPS)$(C_DEPS) CH571_BLE_Hygrothermograph.elf
	-@
secondary-outputs: $(SECONDARY_FLASH) $(SECONDARY_LIST) $(SECONDARY_SIZE)

.PHONY: all clean dependents

-include ../makefile.targets
