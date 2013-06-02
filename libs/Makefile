include ../Makefile.common
LIBS+=libstm32.a
#LIBS+=libstm32-eth.a
#LIBS+=libusbfs.a
CFLAGS+=-c

all: libs

libs: $(LIBS)


libstm32-eth.a:
	@echo -n "Building $@ ..."
	#@rm $(LIBDIR)/libstm32-eth.a
	@cd $(LIBDIR)/Ethernet/src && \
		$(CC) $(CFLAGS) \
			-I../inc \
			*.c
	@$(AR) cr $(LIBDIR)/$@ \
		$(LIBDIR)/Ethernet/src/*.o
	@echo "done."


libusbfs.a:
	@echo -n "compiling source for $@ ..."
	cd $(STMLIB)/STM32_USB_Device_Library/Core/src && \
		$(CC) $(CFLAGS) \
			*.c
			
	cd $(STMLIB)/STM32_USB_Device_Library/Class/cdc/src/ &&\
		$(CC) $(CFLAGS) \
			usbd_cdc_core.c
			
	cd $(STMLIB)/STM32_USB_OTG_Driver/src && \
		$(CC) $(CFLAGS) \
			usb_core.c \
			usb_dcd.c \
			usb_dcd_int.c
		
	@echo -n "Building lib $@ ..."
	$(AR) cr $(LIBDIR)/$@ \
		$(STMLIB)/STM32_USB_Device_Library/Core/src/*.o \
		$(STMLIB)/STM32_USB_Device_Library/Class/cdc/src/usbd_cdc_core.o \
		$(STMLIB)/STM32_USB_OTG_Driver/src/*.o
	@echo "done."


libstm32.a:
	@echo -n "Building $@ ..."
	@cd $(STMLIB)/CMSIS/CM3/DeviceSupport/ST/STM32F10x && \
		$(CC) $(CFLAGS) \
			system_stm32f10x.c
	@cd $(STMLIB)/CMSIS/CM3/CoreSupport && \
		$(CC) $(CFLAGS) \
			core_cm3.c
	@cd $(STMLIB)/STM32F10x_StdPeriph_Driver/src && \
		$(CC) $(CFLAGS) \
			-D"assert_param(expr)=((void)0)" \
			-I../../CMSIS/CM3/CoreSupport \
			-I../../CMSIS/CM3/DeviceSupport/ST/STM32F10x \
			-I../inc \
			*.c


#@cd $(STMLIB)/STM32_USB-FS-Device_Driver/src && \
#	$(CC) $(CFLAGS) \
#		-D"assert_param(expr)=((void)0)" \
#		-I../../CMSIS/Include \
#		-I../../CMSIS/Device/ST/STM32F10x/Include \
#		-I../inc \
#		*.c
	@$(AR) cr $(LIBDIR)/$@ \
		$(STMLIB)/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.o \
		$(STMLIB)/CMSIS/CM3/CoreSupport/core_cm3.o \
		$(STMLIB)/STM32F10x_StdPeriph_Driver/src/*.o \
		#$(STMUTIL)/stm3210c_eval/*.o \
		#$(STMUTIL)/Common/fonts.o \
#		$(STMUTIL)/stm32_eval.o
#$(STMLIB)/STM32_USB-FS-Device_Driver/src/*.o
	@echo "done."

.PHONY: libs clean

clean:
	rm -f $(STMLIB)/CMSIS/CM3/DeviceSupport/ST/STM32F10x/system_stm32f10x.o
	rm -f $(STMLIB)/CMSIS/CM3/CoreSupport/core_cm3.o
	rm -f $(STMLIB)/STM32F10x_StdPeriph_Driver/src/*.o
	rm -f $(STMUTIL)/stm3210c_eval/*.o
	rm -f $(STMUTIL)/stm32_eval.o
	rm -f $(STMUTIL)/Common/fonts.o
	
	rm -f $(STMLIB)/STM32_USB_Device_Library/Core/src/*.o
	rm -f $(STMLIB)/STM32_USB_Device_Library/Class/cdc/src/*.o
	rm -f $(STMLIB)/STM32_USB_Device_Library/Class/audio/src/*.o
	rm -f $(STMLIB)/STM32_USB_Device_Library/Class/dfu/src/*.o
	rm -f $(STMLIB)/STM32_USB_Device_Library/Class/hid/src/*.o
	rm -f $(STMLIB)/STM32_USB_Device_Library/Class/msc/src/*.o
	rm -f $(STMLIB)/STM32_USB_OTG_Driver/src/*.o
	
	
	rm -f $(STMLIB)/STM32_ETH_Driver/src/*.o
#rm -f $(STMLIB)/STM32_USB-FS-Device_Driver/src/*.o
	rm -f $(LIBS)

