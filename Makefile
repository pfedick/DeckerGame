CC		= gcc
EXTRA_CFLAGS =
INCLUDE	= -Iinclude -I/usr/include/SDL2 -I/usr/local/include/SDL2 -I/mingw64/include/SDL2
CFLAGS	= -ggdb -Wall  $(INCLUDE) $(EXTRA_CFLAGS) `ppl7-config --cflags debug core gfx`

LIB		= -L/usr/local/lib `ppl7-config --libs debug core gfx` `sdl2-config --static-libs` -lstdc++
PROGRAM	= decker

OBJECTS=compile/main.o \
	compile/sdl.o \
	compile/game.o \
	compile/sprite.o \
	compile/level.o \
	compile/fps.o \
	compile/ui.o \
	compile/plane.o \
	compile/player.o
	
	
TEXMAKER_CFLAGS	= -ggdb -Wall  -Itools/texmaker $(EXTRA_CFLAGS) `ppl7-config --cflags release` `ppl7-config --cflags debug core gfx`
TEXMAKER_LIB	= -L/usr/local/lib `ppl7-config --libs release` -lstdc++
TEXMAKER_OBJECTS	= compile/texm_main.o compile/texm_texture.o compile/texm_texturefile.o


LIBDEP	:= $(shell ppl7-config --ppllib debug)
	
$(PROGRAM): $(OBJECTS) $(LIBDEP)
	$(CC) -o $(PROGRAM) $(OBJECTS) $(CFLAGS) $(LIB)
	-chmod 755 $(PROGRAM)

all: $(PROGRAM)

texmaker: $(TEXMAKER_OBJECTS) $(LIBDEP)
	$(CC) -o texmaker $(TEXMAKER_OBJECTS) $(TEXMAKER_CFLAGS) $(TEXMAKER_LIB)
	-chmod 755 texmaker

clean:
	-rm -f compile/*.o $(PROGRAM) texmaker *.core
	
sprites:
	./texmaker -s lightwave/Render/george/*.png -t res/george.tex -w 1024 -h 1024 -px 128 -py 127 -x res/george
	./texmaker -s lightwave/Render/tiles/*.png -t res/tiles.tex -w 512 -h 512 --pivot_detection bricks -x res/tiles
	./texmaker -f res/cursor/cursor.lst -t res/cursor.tex -w 128 -h 128 -px 0 -py 0
	
compile/main.o: src/main.cpp Makefile include/decker.h
	-mkdir -p compile 
	$(CC) -o compile/main.o -c src/main.cpp $(CFLAGS)
	
compile/sdl.o: src/sdl.cpp Makefile include/decker.h
	-mkdir -p compile 
	$(CC) -o compile/sdl.o -c src/sdl.cpp $(CFLAGS)

compile/game.o: src/game.cpp Makefile include/decker.h include/ui.h
	-mkdir -p compile 
	$(CC) -o compile/game.o -c src/game.cpp $(CFLAGS)

compile/sprite.o: src/sprite.cpp Makefile include/decker.h
	-mkdir -p compile 
	$(CC) -o compile/sprite.o -c src/sprite.cpp $(CFLAGS)

compile/level.o: src/level.cpp Makefile include/decker.h
	-mkdir -p compile 
	$(CC) -o compile/level.o -c src/level.cpp $(CFLAGS)

compile/fps.o: src/fps.cpp Makefile include/decker.h
	-mkdir -p compile 
	$(CC) -o compile/fps.o -c src/fps.cpp $(CFLAGS)

compile/ui.o: src/ui.cpp Makefile include/ui.h
	-mkdir -p compile 
	$(CC) -o compile/ui.o -c src/ui.cpp $(CFLAGS)

compile/plane.o: src/plane.cpp Makefile include/decker.h
	-mkdir -p compile 
	$(CC) -o compile/plane.o -c src/plane.cpp $(CFLAGS)

compile/player.o: src/player.cpp Makefile include/player.h
	-mkdir -p compile 
	$(CC) -o compile/player.o -c src/player.cpp $(CFLAGS)

	
compile/texm_main.o: tools/texmaker/main.cpp Makefile tools/texmaker/texmaker.h
	-mkdir -p compile 
	$(CC) -o compile/texm_main.o -c tools/texmaker/main.cpp $(TEXMAKER_CFLAGS)
	
compile/texm_texture.o: tools/texmaker/texture.cpp Makefile tools/texmaker/texmaker.h
	-mkdir -p compile 
	$(CC) -o compile/texm_texture.o -c tools/texmaker/texture.cpp $(TEXMAKER_CFLAGS)
	
compile/texm_texturefile.o: tools/texmaker/texturefile.cpp Makefile tools/texmaker/texmaker.h
	-mkdir -p compile 
	$(CC) -o compile/texm_texturefile.o -c tools/texmaker/texturefile.cpp $(TEXMAKER_CFLAGS)
	