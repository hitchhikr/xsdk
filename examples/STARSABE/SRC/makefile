ifeq ($(XSDK),)
$(error Environment variable "XSDK" is undefined)
endif
CC = $(XSDK)/bin/human68k-gcc
LD = $(XSDK)/bin/human68k-ld
AS = $(XSDK)/bin/vasm
RM = $(XSDK)/bin/rm
OBJCOPY	= $(XSDK)/bin/human68k-objcopy
STRIP = $(XSDK)/bin/stripx
CFLAGS = -g -Ofast -Wno-multichar -fomit-frame-pointer -fstrength-reduce -fforce-addr -fcall-used-d2 -fcall-used-a2
LIBS = -ldos -liocs -lbas -lc -lfloatfnc

.c.o:
	$(CC) $(CFLAGS) -o $@ -c $<
.s.o:
	$(AS) -I$(XSDK)/human68k/include -quiet -Felf -o $@ $<

files = main.o dim.o matrix.o getkey.o calc.o star.o model.o timelist.o enemy.o motion.o move.o \
        setbox.o spdisp.o grdisp.o txdisp.o sound.o interupt.o disp.o intrupt.o

all: RUN.X

RUN.X: $(files)
	$(CC) -Wl,--no-warn-rwx-segments -o ../$@ $(files) $(LIBS)
	$(OBJCOPY) -O xfile ../$@ ../$@
	$(STRIP) ../$@

clean:
	-$(RM) $(files)
