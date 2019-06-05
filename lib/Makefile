#################################################################
obj := div64.o lib1funcs.o ctype.o muldi3.o printf.o string.o vsprintf.o
#################################################################

libc.a : $(obj)
	$(AR) -r -o $@ $^
	
%.o : %.c
	$(CC) $(CFLAGS) $@ $<

%.o : %.S
	$(CC) $(CFLAGS) $@ $<

clean:
	rm -rf libc.a *.o
