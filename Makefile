PREFIX=/usr/local

CC=gcc
CFLAGS=-g -static -std=c99 -Wall -pedantic -Iinclude/ -lm
LDFLAGS=-L/usr/lib

OBJS= \
	src/color_quantization.o \
	src/dither.o \
	src/format-tga.o \
	src/img.o \
	src/scale.o \

all: libafbeelding.a 

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

libafbeelding.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

color_quantization_test: color_quantization_test.o libafbeelding.a
	$(CC) -o $@ $< libafbeelding.a $(CFLAGS)

dithering_test: dithering_test.o libafbeelding.a
	$(CC) -o $@ $< libafbeelding.a $(CFLAGS)
	
libafbeelding_test: libafbeelding_test.o libafbeelding.a
	$(CC) -o $@ $< libafbeelding.a

scale_test: scale_test.o libafbeelding.a
	$(CC) -o $@ $< libafbeelding.a

.PHONY: clean install uninstall color_quantization_test

install: libafbeelding.a
	mkdir -p $(DESTDIR)$(PREFIX)/lib
	cp libafbeelding.a $(DESTDIR)$(PREFIX)/lib
	mkdir -p $(DESTDIR)$(PREFIX)/include/libafbeelding
	cp include/*.h $(DESTDIR)$(PREFIX)/include/libafbeelding/

uninstall:
	cd $(DESTDIR)$(PREFIX)/lib/ && rm libafbeelding.a
	cd $(DESTDIR)$(PREFIX)/include && rm -r libafbeelding/

clean:
	$(RM) src/*.o *.o *.exe color_quantization_test libafbeelding_test
