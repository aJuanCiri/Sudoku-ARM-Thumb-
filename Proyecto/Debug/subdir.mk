################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../8led.c \
../Bmp.c \
../button.c \
../gestionExcepciones.c \
../gestorPantalla.c \
../gestorRebotes.c \
../gestorSudoku.c \
../itoa.c \
../lcd.c \
../led.c \
../listaCircularEstatica.c \
../main.c \
../pilaDepuracion.c \
../sudoku_2015.c \
../timer2.c \
../timer4.c 

ASM_SRCS += \
../data_abort_test.asm \
../sudoku_2015_ARM-THUMB.asm \
../sudoku_2015_ARM.asm 

OBJS += \
./8led.o \
./Bmp.o \
./button.o \
./data_abort_test.o \
./gestionExcepciones.o \
./gestorPantalla.o \
./gestorRebotes.o \
./gestorSudoku.o \
./itoa.o \
./lcd.o \
./led.o \
./listaCircularEstatica.o \
./main.o \
./pilaDepuracion.o \
./sudoku_2015.o \
./sudoku_2015_ARM-THUMB.o \
./sudoku_2015_ARM.o \
./timer2.o \
./timer4.o 

C_DEPS += \
./8led.d \
./Bmp.d \
./button.d \
./gestionExcepciones.d \
./gestorPantalla.d \
./gestorRebotes.d \
./gestorSudoku.d \
./itoa.d \
./lcd.d \
./led.d \
./listaCircularEstatica.d \
./main.d \
./pilaDepuracion.d \
./sudoku_2015.d \
./timer2.d \
./timer4.d 

ASM_DEPS += \
./data_abort_test.d \
./sudoku_2015_ARM-THUMB.d \
./sudoku_2015_ARM.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC C Compiler'
	arm-none-eabi-gcc -I"C:\Users\a680669\workspace\Practica3\common" -O0 -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -mapcs-frame -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

%.o: ../%.asm
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Sourcery Windows GCC Assembler'
	arm-none-eabi-gcc -x assembler-with-cpp -I"C:\Users\a680669\workspace\Practica3\common" -Wall -Wa,-adhlns="$@.lst" -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -mcpu=arm7tdmi -g3 -gdwarf-2 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


