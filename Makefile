CC=gcc
CFLAGS=-g -static -std=c99 -Wall -pedantic -Iinclude/ -lm
LDFLAGS=-L/usr/lib
OBJS=src/color_quantization.o src/scale.o src/tga.o src/img.o

all: libafbeelding.a

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

libafbeelding.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

color_quantization_test: color_quantization_test.o libafbeelding.a
	$(CC) -o $@ $< libafbeelding.a $(CFLAGS)
	
libafbeelding_test: libafbeelding_test.o libafbeelding.a
	$(CC) -o $@ $< libafbeelding.a

scale_test: scale_test.o libafbeelding.a
	$(CC) -o $@ $< libafbeelding.a

.PHONY: clean color_quantization_test

clean:
	$(RM) src/*.o *.o *.exe color_quantization_test libafbeelding_test
