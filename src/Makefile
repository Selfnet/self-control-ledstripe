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


OBJS+=clock-arch.o
OBJS+=ethernet.o
OBJS+=io-helper.o
OBJS+=tcp_app.o
OBJS+=main.o
OBJS+=psock.o
OBJS+=startup.o
OBJS+=stm32_eth.o
OBJS+=stm32f10x_it.o
OBJS+=syscalls.o
OBJS+=timer.o
OBJS+=uip_arp.o
OBJS+=uip.o
OBJS+=uiplib.o

all: src

src: app.a

app.a: $(OBJS)
		$(AR) cr app.a $(OBJS)

.PHONY: src clean
clean:
		rm -f app.a $(OBJS)
