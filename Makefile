include Makefile.common

STARTUP=startup.c
LINKERSCRIPT=linker.ld

LDFLAGS=$(COMMONFLAGS) -fno-exceptions -ffunction-sections -fdata-sections -L$(LIBDIR) -nostartfiles -Wl,--gc-sections,-T$(LINKERSCRIPT)

LDLIBS+=-lm
LDLIBS+=-lstm32

all: libs src
		$(CC) -o $(BUILDDIR)/$(PROGRAM).elf $(LDFLAGS) \
				-Wl,--whole-archive \
						src/app.a \
				-Wl,--no-whole-archive \
						$(LDLIBS)
		$(OBJCOPY) -O ihex $(BUILDDIR)/$(PROGRAM).elf $(BUILDDIR)/$(PROGRAM).hex
		$(OBJCOPY) -O binary $(BUILDDIR)/$(PROGRAM).elf $(BUILDDIR)/$(PROGRAM).bin
#Extract info contained in ELF to readable text-files:
		$(RE) -a $(BUILDDIR)/$(PROGRAM).elf > $(BUILDDIR)/$(PROGRAM).info_elf
		$(SIZE) -d -B -t $(BUILDDIR)/$(PROGRAM).elf > $(BUILDDIR)/$(PROGRAM).info_size
		$(OD) -S $(BUILDDIR)/$(PROGRAM).elf > $(BUILDDIR)/$(PROGRAM).info_code
		$(NM) -t d -S --size-sort -s $(BUILDDIR)/$(PROGRAM).elf > $(BUILDDIR)/$(PROGRAM).info_symbol

.PHONY: libs src clean
libs:
		$(MAKE) -C libs $@

src:
		$(MAKE) -C src $@

usbprogram: all
		dfu-util -R -a 0 -s 0x08000000 -D $(BUILDDIR)/$(PROGRAM).bin 

program: all
		$(STM32FLASH) $(BUILDDIR)/$(PROGRAM).bin $(FLASHLOC)

allclean:
		$(MAKE) -C src $@
		$(MAKE) -C libs $@
		rm -f $(BUILDDIR)/$(PROGRAM).elf $(BUILDDIR)/$(PROGRAM).hex $(BUILDDIR)/$(PROGRAM).bin \
				$(BUILDDIR)/$(PROGRAM).info_elf $(BUILDDIR)/$(PROGRAM).info_size
		rm -f $(BUILDDIR)/$(PROGRAM).info_code
		rm -f $(BUILDDIR)/$(PROGRAM).info_symbol

clean:
		$(MAKE) -C src $@
		rm -f $(BUILDDIR)/$(PROGRAM).elf $(BUILDDIR)/$(PROGRAM).hex $(BUILDDIR)/$(PROGRAM).bin \
				$(BUILDDIR)/$(PROGRAM).info_elf $(BUILDDIR)/$(PROGRAM).info_size
		rm -f $(BUILDDIR)/$(PROGRAM).info_code
		rm -f $(BUILDDIR)/$(PROGRAM).info_symbol
