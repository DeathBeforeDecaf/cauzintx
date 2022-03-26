################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/strptx16/bitfld32.c \
../src/strptx16/command.c \
../src/strptx16/crc16.c \
../src/strptx16/editor.c \
../src/strptx16/filelist.c \
../src/strptx16/layout.c \
../src/strptx16/parseopt.c \
../src/strptx16/platform.c \
../src/strptx16/settings.c \
../src/strptx16/support.c \
../src/strptx16/txdirect.c \
../src/strptx16/txreduce.c \
../src/strptx16/txstrip.c 

C_DEPS += \
./src/strptx16/bitfld32.d \
./src/strptx16/command.d \
./src/strptx16/crc16.d \
./src/strptx16/editor.d \
./src/strptx16/filelist.d \
./src/strptx16/layout.d \
./src/strptx16/parseopt.d \
./src/strptx16/platform.d \
./src/strptx16/settings.d \
./src/strptx16/support.d \
./src/strptx16/txdirect.d \
./src/strptx16/txreduce.d \
./src/strptx16/txstrip.d 

OBJS += \
./src/strptx16/bitfld32.o \
./src/strptx16/command.o \
./src/strptx16/crc16.o \
./src/strptx16/editor.o \
./src/strptx16/filelist.o \
./src/strptx16/layout.o \
./src/strptx16/parseopt.o \
./src/strptx16/platform.o \
./src/strptx16/settings.o \
./src/strptx16/support.o \
./src/strptx16/txdirect.o \
./src/strptx16/txreduce.o \
./src/strptx16/txstrip.o 


# Each subdirectory must supply rules for building sources it contributes
src/strptx16/%.o: ../src/strptx16/%.c src/strptx16/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-strptx16

clean-src-2f-strptx16:
	-$(RM) ./src/strptx16/bitfld32.d ./src/strptx16/bitfld32.o ./src/strptx16/command.d ./src/strptx16/command.o ./src/strptx16/crc16.d ./src/strptx16/crc16.o ./src/strptx16/editor.d ./src/strptx16/editor.o ./src/strptx16/filelist.d ./src/strptx16/filelist.o ./src/strptx16/layout.d ./src/strptx16/layout.o ./src/strptx16/parseopt.d ./src/strptx16/parseopt.o ./src/strptx16/platform.d ./src/strptx16/platform.o ./src/strptx16/settings.d ./src/strptx16/settings.o ./src/strptx16/support.d ./src/strptx16/support.o ./src/strptx16/txdirect.d ./src/strptx16/txdirect.o ./src/strptx16/txreduce.d ./src/strptx16/txreduce.o ./src/strptx16/txstrip.d ./src/strptx16/txstrip.o

.PHONY: clean-src-2f-strptx16

