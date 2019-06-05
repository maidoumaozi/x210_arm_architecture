#################################################################
obj = led.o uart.o key_polling.o key_interrupt.o pwm.o wdt.o rtc.o i2c.o adc.o
#################################################################

dev.a : $(obj)
	$(AR) -r -o $@ $^

%.o : %.S
	$(CC) $(CFLAGS) $@ $<

%.o : %.c
	$(CC) $(CFLAGS) $@ $<

clean:
	rm -rf dev.a *.o
