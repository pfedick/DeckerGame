[![C/C++ CI](https://github.com/pfedick/DeckerGame/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/pfedick/DeckerGame/actions/workflows/c-cpp.yml)

# Decker - The Game

<p align="center">
  <img src="https://github.com/pfedick/DeckerGame/raw/main/lightwave/Render/george/frame_0027.png" />
</p>


This is my attempt of writing a Jump'n'Run game in C++. It uses the [Simple DirectMedia Layer](https://www.libsdl.org/) library for graphics and my own library [pplib](https://github.com/pfedick/pplib) for graphics and ui.

It features George Decker, one of the main characters of my Lego(tm) stopmotion film [The Adventures Of Charlie and George Decker](https://youtu.be/-aJDTe_obKI), which I made a few years ago and is available on YouTube.


## What's the status of the game?
The game is in very early development, which means I know a few things I want to build into it,
but I have to figure out how to do it.

### Things which are ongoing
- create graphics
- ui
- world editor

### Things I have to do or figure out
- which resolution should the game have
- what is the best size for tiles and sprites?
- how to implement interactive objects/enemies
- how to control the main character?
- physics
- collision detection
- game mechanics (hallways, doors, traps, ladders, walk, run, jump, fall) 
- how does sound work with SDL?
- how to integrate sound track and sound effects
- compose soundtrack
- create sound effects
- build the world/levels
- story?
There is probably more, which I'm not aware of, yet ;-)

### Things I have finished
- basic ui for editing and testing the world
- how to build and render the tile-based world (tiles, layers)
- how to move in the world (side scrolling, paralax)
- how to implement the sprites?


## On what platforms will the game be available
My target platforms are:

- Linux (development is done on Fedora 34)
- Windows 10

The game will be open source and uses the [Simple DirectMedia Layer](https://www.libsdl.org/) library, which is available on many platforms. So in therory it should be possible to compile it on any platform with a c++ compiler and the SDL library.

## When will it come out?
I don't know. This is one of my hobbies, but I have others, too :-)
Maybe I will never finish it or decide to do something else. We will see...

## How to compile
### Requirements
The following Tools/Libraries are required:
- gcc or clang with c++ support (C++11 or greater)
- gnu make
- pcre
- sdl2
- iconv
- zlib
- bz2
- png

#### Fedora
    sudo dnf install -y git gcc gcc-c++ make pcre-devel SDL2-devel zlib-devel \
        bzip2-devel libpng-devel
        
#### Ubuntu
    sudo apt-get install -y git gcc g++ libpcre3-dev libbz2-dev zlib1g-dev libsdl2-dev \
        libpng-dev libfreetype-dev
        
#### Windows 10 with mingw64
TODO

### Checkout and compile

    git clone --recurse-submodules https://github.com/pfedick/DeckerGame.git
    cd DeckerGame
    ./configure
    make -j
    
    # run the game:
    ./decker
    
If you already cloned the project and want to update to the newest version:

    git pull
    git submodule update --init --recursive
    ./configure
    make -j
    
    
    






