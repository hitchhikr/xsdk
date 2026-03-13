CC	= gcc
LD	= ld
CFLAGS	= -Os
LDFLAGS	= -s

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

files = x2bin.o

x2bin: $(files)
	$(CC) -o x2bin $(files) $(LDFLAGS)

clean:
	-rm $(files)
    
