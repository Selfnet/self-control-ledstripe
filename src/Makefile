include ../Makefile.common

OBJS+=startup.o
OBJS+=io-helper.o
OBJS+=main.o
OBJS+=stm32f10x_it.o

OBJS+=uiplib.o
OBJS+=uip.o
OBJS+=uip_arp.o
OBJS+=timer.o
OBJS+=stm32_eth.o
OBJS+=psock.o
OBJS+=ethernet.o
OBJS+=clock-arch.o

OBJS+=httpd.o

OBJS+=syscalls.o


all: src

src: app.a

app.a: $(OBJS)
		$(AR) cr app.a $(OBJS)

.PHONY: src clean
clean:
		rm -f app.a $(OBJS)
