CC=gcc
CFLAGS=-g -static -std=c11 -Wall -pedantic -I. -lm
LDFLAGS=-L/usr/lib
OBJS=color_quantization.o dithering.o

all: $(OBJS)

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

color_quantization_test: color_quantization_test.o color_quantization.o
	$(CC) -o $@ $^ $(CFLAGS)

dithering_test: dithering.o
	$(CC) -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	$(RM) *.o *.exe color_quantization_test
