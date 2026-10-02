CC = g++
LD = ld
CPPFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS	= -s

.cpp.o:
	$(CC) $(CPPFLAGS) -o $@ -c $<

files = elf2x68k.o

elf2x68k: $(files)
	$(CC) -o elf2x68k $(files) $(LDFLAGS)

clean:
	-rm $(files)
    
