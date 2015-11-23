################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Generated_Code/Cpu.c \
../Generated_Code/clockMan1.c \
../Generated_Code/hardware_init.c \
../Generated_Code/osa1.c \
../Generated_Code/pin_mux.c 

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/clockMan1.o \
./Generated_Code/hardware_init.o \
./Generated_Code/osa1.o \
./Generated_Code/pin_mux.o 

C_DEPS += \
./Generated_Code/Cpu.d \
./Generated_Code/clockMan1.d \
./Generated_Code/hardware_init.d \
./Generated_Code/osa1.d \
./Generated_Code/pin_mux.d 


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/%.o: ../Generated_Code/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"FSL_OSA_BM_TIMER_CONFIG=2" -D"CPU_MKL02Z32VFG4" -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/hal/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/hal/src/sim/MKL02Z4 -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/system/src/clock/MKL02Z4 -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/system/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/osa/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/CMSIS/Include -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices/MKL02Z4/include -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices/MKL02Z4/startup -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Generated_Code/SDK/platform/devices/MKL02Z4/startup -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Sources -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Generated_Code -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/utilities/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/drivers/inc -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


