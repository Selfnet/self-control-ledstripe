include ../Makefile.common

#OBJS+=startup.o
#OBJS+=io-helper.o
#OBJS+=main.o
#OBJS+=stm32f10x_it.o

#OBJS+=uiplib.o
#OBJS+=uip.o
#OBJS+=uip_arp.o
#OBJS+=timer.o
#OBJS+=stm32_eth.o
#OBJS+=psock.o
#OBJS+=ethernet.o
#OBJS+=clock-arch.o

#OBJS+=httpd.o

#OBJS+=syscalls.o


OBJS+=ethernet.o
OBJS+=ethernet_init.o
OBJS+=io-helper.o
OBJS+=tcp_app.o
OBJS+=main.o
OBJS+=startup.o
OBJS+=stm32f10x_it.o
OBJS+=syscalls.o
OBJS+=can.o



OBJS+=usb/usbd_cdc_vcp.o
OBJS+=usb/usbd_desc.o
OBJS+=usb/usbd_usr.o
OBJS+=usb/usb_bsp.o
OBJS+=usb/system_stm32f10x.o

OBJS+=led_pwm.o

all: src

src: app.a

app.a: $(OBJS)
		$(AR) cr app.a $(OBJS)

app2.a:
	@echo -n "Building $@ ..."
	$(CC) $(CFLAGS) \
		*.c \
		usb/*.c
	$(AR) cr app.a *.o usb/*.o
	@echo "done."


.PHONY: src clean
clean:
		rm -f *.o
		rm -f app.a
