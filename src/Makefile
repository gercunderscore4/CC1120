CC=gcc
CFLAGS=-Wall -Werror -g -Wextra -Wno-unused-parameter

all: bit_bang.o Arduino.o main

main: bit_bang.o Arduino.o
	$(CC) bit_bang.o Arduino.o main.c -o main

Arduino.o: Arduino.h Arduino.c
	$(CC) $(CFLAGS) -c Arduino.c

bits.o: bits.h bits.c
	$(CC) $(CFLAGS) -c bits.c

spi.o: gpio.h bits.h spi.h spi.c
	$(CC) $(CFLAGS) -c spi.c

bit_bang.o: bit_bang.h Arduino.h bit_bang.h Arduino.c
	$(CC) $(CFLAGS) -c bit_bang.c

clean:
	rm -rf bit_bang.o Arduino.o main
