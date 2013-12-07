
PROG = firmware
SOURCES = src/${PROG}.c
#LFUSE = 0xCE
#HFUSE = 0xD9
#EFUSE = 0xFF

CFLAGS  = -Os -ffast-math -mmcu=atmega328p -mcall-prologues #-combine
CFLAGS += -Wl,-static -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,-s
CFLAGS += -Wl,-Map,${PROG}.map
CFLAGS += -std=c99
#CFLAGS += -Wl,-u,vfprintf -lprintf_flt -lm

DUDE_FLAGS = -c dragon_isp -P usb -p m328p -F
#DUDE_FLAGS = -c arduino -b 57600 -P /dev/ttyUSB0 -p m328p

run_avr = echo
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
AVRDUDE = sudo avrdude

.PHONY: build write clean

build: ${PROG}.hex ${PROG}.lst

write: ${PROG}.hex
	${AVRDUDE} ${DUDE_FLAGS} -U flash:w:${PROG}.hex

#writefuses:
#	${AVRDUDE} ${DUDE_FLAGS} -U lfuse:w:${LFUSE}:m -U hfuse:w:${HFUSE}:m

clean:
	rm -f ${PROG} ${PROG}.hex ${PROG}.lst ${PROG}.map ${PROG}.elf

${PROG}.elf: $(SOURCES) Makefile
	${CC} ${CFLAGS} -o ${PROG}.elf ${SOURCES}

%.hex: %.elf
	${OBJCOPY} -j .text -j .data -O ihex $< $@
%.lst: %.elf
	${OBJDUMP} -S $< > $@

