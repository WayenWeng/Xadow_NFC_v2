################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Static_Code/System/PE_low_level_init.c 

OBJS += \
./Static_Code/System/PE_low_level_init.o 

C_DEPS += \
./Static_Code/System/PE_low_level_init.d 


# Each subdirectory must supply rules for building sources it contributes
Static_Code/System/%.o: ../Static_Code/System/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"FSL_OSA_BM_TIMER_CONFIG=2" -D"CPU_MKL02Z32VFG4" -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/hal/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/hal/src/sim/MKL02Z4 -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/system/src/clock/MKL02Z4 -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/system/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/osa/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/CMSIS/Include -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices/MKL02Z4/include -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices/MKL02Z4/startup -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Generated_Code/SDK/platform/devices/MKL02Z4/startup -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Sources -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Generated_Code -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/utilities/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/drivers/inc -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


