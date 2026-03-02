CC	= gcc
LD	= ld
CFLAGS	= -Os
LDFLAGS	= -s

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<

files = stripx.o

stripx: $(files)
	$(CC) -o stripx $(files) $(LDFLAGS)

clean:
	-rm $(files)
    
