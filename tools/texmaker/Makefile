CC		= gcc
EXTRA_CFLAGS =
INCLUDE	= -I.
CFLAGS	= -ggdb -Wall  $(INCLUDE) $(EXTRA_CFLAGS) `ppl6-config --cflags release`
LIB		= -L/usr/local/lib `ppl6-config --libs release`  -lstdc++
PROGRAM	= texmaker

OBJECTS=main.o texture.o texturefile.o
	

LIBDEP	:= $(shell ppl6-config --ppllib release)
	
$(PROGRAM): $(OBJECTS) $(LIBDEP)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(CFLAGS) $(LIB)
	-chmod 755 $(PROGRAM)

all: $(PROGRAM)

clean:
	-rm -f *.o $(PROGRAM) *.core
	
install: $(PROGRAM)
	-cp $(PROGRAM) /usr/local/bin

	
main.o: main.cpp Makefile texmaker.h
	$(CC) -o main.o -c main.cpp $(CFLAGS)
	
texture.o: texture.cpp Makefile texmaker.h
	$(CC) -o texture.o -c texture.cpp $(CFLAGS)

texturefile.o: texturefile.cpp Makefile texmaker.h
	$(CC) -o texturefile.o -c texturefile.cpp $(CFLAGS)
	