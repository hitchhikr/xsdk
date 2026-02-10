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

all: SPLOAD.X SPSAVE.X

spload.o : main.c
	$(CC) $(CFLAGS) -o $@ -c $<
spsave.o : main.c
	$(CC) $(CFLAGS) -DSPSAVE -o $@ -c $<
.s.o:
	$(AS) -I$(XSDK)/human68k/include -quiet -Felf -o $@ $<

SPLOAD.X: spload.o
	$(CC) -Wl,--no-warn-rwx-segments -o $@ $< $(LIBS)
	$(OBJCOPY) -O xfile $@ $@
	$(STRIP) $@

SPSAVE.X: spsave.o
	$(CC) -Wl,--no-warn-rwx-segments -o $@ $< $(LIBS)
	$(OBJCOPY) -O xfile $@ $@
	$(STRIP) $@

clean:
	-$(RM) spload.o spsave.o
