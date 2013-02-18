TCPREFIX = ../gnu-gcc-arm/arm-toolchain/bin/arm-none-eabi-
CC      = $(TCPREFIX)gcc
LD      = $(TCPREFIX)ld -v
CP      = $(TCPREFIX)objcopy
OD      = $(TCPREFIX)objdump
GDBTUI  = $(TCPREFIX)gdbtui

STM32FLASH = ./stm32_flash.py
FLASHLOC = 0x08000000
LINKERSCRIPT = startup_src/stm32.ld

# -mfix-cortex-m3-ldrd should be enabled by default for Cortex M3.
CFLAGS  =  -I. -c -fno-common -O0 -g -mcpu=cortex-m3 -mthumb
LFLAGS  = -T$(LINKERSCRIPT) -nostartfiles
CPFLAGS = -Obinary
ODFLAGS = -S

all: run

clean:
	-rm -f src/main.lst src/*.o src/main.elf src/main.lst src/main.bin

run: src/main.bin
	$(STM32FLASH) src/main.bin $(FLASHLOC)

src/main.bin: src/main.elf
	@echo "...copying"
	$(CP) $(CPFLAGS) src/main.elf src/main.bin
	$(OD) $(ODFLAGS) src/main.elf > src/main.lst

src/main.elf: src/main.o $(LINKERSCRIPT)
	@echo "..linking"
	$(LD) $(LFLAGS) -o src/main.elf src/main.o

src/main.o: src/main.c
	@echo ".compiling"
	$(CC) $(CFLAGS) src/main.c -o src/main.o

debug:
	$(GDBTUI) -ex "target remote localhost:3333" \
	-ex "set remote hardware-breakpoint-limit 6" \
	-ex "set remote hardware-watchpoint-limit 4" src/main.elf
