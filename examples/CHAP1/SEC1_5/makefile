ifeq ($(XSDK),)
$(error Environment variable "XSDK" is undefined)
endif
CC = $(XSDK)/bin/human68k-gcc
AS = $(XSDK)/bin/vasm
RM = $(XSDK)/bin/rm
OBJCOPY	= $(XSDK)/bin/human68k-objcopy
STRIP = $(XSDK)/bin/stripx
CFLAGS = -Ofast -Wno-multichar -fomit-frame-pointer -fstrength-reduce -fforce-addr -fcall-used-d2 -fcall-used-a2
LIBS = -ldos -liocs -lbas -lm

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
.s.o:
	$(AS) -I$(XSDK)/human68k/include -quiet -Felf -o $@ $<

files = main.o atari.o enemy.o ship.o bomb.o enedat.o sub.o

all: MAIN.X

MAIN.X: $(files)
	$(CC) -Wl,--no-warn-rwx-segments -o $@ $(files) $(LIBS)
	$(OBJCOPY) -O xfile $@ $@
	$(STRIP) $@

clean:
	-$(RM) $(files)
