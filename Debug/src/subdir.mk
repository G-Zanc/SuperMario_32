################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/commands.c \
../src/deathMusic.c \
../src/diskio.c \
../src/ff.c \
../src/flagMusic.c \
../src/font.c \
../src/gameObjects.c \
../src/game_tileset.c \
../src/gpio.c \
../src/lcd.c \
../src/main.c \
../src/midi.c \
../src/midiPlayer.c \
../src/overworld1.c \
../src/overworldMusic.c \
../src/sd.c \
../src/syscalls.c \
../src/system_stm32f0xx.c \
../src/title_screen.c 

OBJS += \
./src/commands.o \
./src/deathMusic.o \
./src/diskio.o \
./src/ff.o \
./src/flagMusic.o \
./src/font.o \
./src/gameObjects.o \
./src/game_tileset.o \
./src/gpio.o \
./src/lcd.o \
./src/main.o \
./src/midi.o \
./src/midiPlayer.o \
./src/overworld1.o \
./src/overworldMusic.o \
./src/sd.o \
./src/syscalls.o \
./src/system_stm32f0xx.o \
./src/title_screen.o 

C_DEPS += \
./src/commands.d \
./src/deathMusic.d \
./src/diskio.d \
./src/ff.d \
./src/flagMusic.d \
./src/font.d \
./src/gameObjects.d \
./src/game_tileset.d \
./src/gpio.d \
./src/lcd.d \
./src/main.d \
./src/midi.d \
./src/midiPlayer.d \
./src/overworld1.d \
./src/overworldMusic.d \
./src/sd.d \
./src/syscalls.d \
./src/system_stm32f0xx.d \
./src/title_screen.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"/home/shay/a/jmielke/ece362/ece362workplace/workspace/final_combined_project/StdPeriph_Driver/inc" -I"/home/shay/a/jmielke/ece362/ece362workplace/workspace/final_combined_project/inc" -I"/home/shay/a/jmielke/ece362/ece362workplace/workspace/final_combined_project/CMSIS/device" -I"/home/shay/a/jmielke/ece362/ece362workplace/workspace/final_combined_project/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


