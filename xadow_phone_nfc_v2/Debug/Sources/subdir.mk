################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Sources/Events.c \
../Sources/delay.c \
../Sources/emulate_tag.c \
../Sources/gpio.c \
../Sources/i2c.c \
../Sources/main.c \
../Sources/mifare_classic.c \
../Sources/mifare_ultralight.c \
../Sources/ndef_message.c \
../Sources/ndef_record.c \
../Sources/nfc_adapter.c \
../Sources/nfc_tag.c \
../Sources/pn532.c \
../Sources/pn532_i2c.c 

OBJS += \
./Sources/Events.o \
./Sources/delay.o \
./Sources/emulate_tag.o \
./Sources/gpio.o \
./Sources/i2c.o \
./Sources/main.o \
./Sources/mifare_classic.o \
./Sources/mifare_ultralight.o \
./Sources/ndef_message.o \
./Sources/ndef_record.o \
./Sources/nfc_adapter.o \
./Sources/nfc_tag.o \
./Sources/pn532.o \
./Sources/pn532_i2c.o 

C_DEPS += \
./Sources/Events.d \
./Sources/delay.d \
./Sources/emulate_tag.d \
./Sources/gpio.d \
./Sources/i2c.d \
./Sources/main.d \
./Sources/mifare_classic.d \
./Sources/mifare_ultralight.d \
./Sources/ndef_message.d \
./Sources/ndef_record.d \
./Sources/nfc_adapter.d \
./Sources/nfc_tag.d \
./Sources/pn532.d \
./Sources/pn532_i2c.d 


# Each subdirectory must supply rules for building sources it contributes
Sources/%.o: ../Sources/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -D"FSL_OSA_BM_TIMER_CONFIG=2" -D"CPU_MKL02Z32VFG4" -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/hal/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/hal/src/sim/MKL02Z4 -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/system/src/clock/MKL02Z4 -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/system/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/osa/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/CMSIS/Include -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices/MKL02Z4/include -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/devices/MKL02Z4/startup -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Generated_Code/SDK/platform/devices/MKL02Z4/startup -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Sources -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/Generated_Code -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/utilities/inc -ID:/Freescale/WorkSpace/xadow_phone_nfc_v2/SDK/platform/drivers/inc -std=c99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


