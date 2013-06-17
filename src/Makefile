include ../Makefile.common


OBJS+=io-helper.o
OBJS+=main.o
OBJS+=startup.o
OBJS+=stm32f10x_it.o
OBJS+=syscalls.o
OBJS+=can.o
OBJS+=clock.o
OBJS+=timer.o
OBJS+=game_dotcatching.o

OBJS+=flash_f1.o

#OBJS+=ps2.o
#OBJS+=ps2_init.o
#OBJS+=stm32f10x_misc.o


OBJS+=rgb_led.o

#OBJS+=led_pwm.o

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


appclean:
		rm -f *.o
		rm -f app.a

