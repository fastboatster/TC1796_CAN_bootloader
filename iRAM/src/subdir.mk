################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lz4.c \
../src/main.c

S_UPPER_SRCS += \
../src/crt0.S 

OBJS += \
./src/crt0.o \
./src/lz4.o \
./src/main.o

C_DEPS += \
./src/lz4.d \
./src/main.d


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.S
	@echo 'Building file: $<'
	@echo 'Invoking: TriCore Assembler'
	"$(TRICORE_TOOLS)/bin/tricore-elf-gcc" -c -I"../h" -Wa,--gdwarf-2 -mcpu=tc1796 -Wa,--insn32-preferred -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: TriCore C Compiler'
	"$(TRICORE_TOOLS)/bin/tricore-elf-gcc" -c -I"../h" -fno-common -Os -g3 -W -Wall -Wextra -Wdiv-by-zero -Warray-bounds -Wcast-align -Wignored-qualifiers -Wformat -Wformat-security -pipe -DPHYCORE_TC1796 -fshort-double -mcpu=tc1796 -mversion-info -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


