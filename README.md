[![C/C++ CI](https://github.com/pfedick/DeckerGame/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/pfedick/DeckerGame/actions/workflows/c-cpp.yml)

# Decker - The Game

<p align="center">
  <img src="https://github.com/pfedick/DeckerGame/raw/main/res/screenshots/Screenshot_20210724_193500.jpg" />
</p>


#Test


This is my attempt of writing a Jump'n'Run game in C++. It uses the [Simple DirectMedia Layer](https://www.libsdl.org/) library for graphics and my own library [pplib](https://github.com/pfedick/pplib) for graphics and ui.

It features George Decker, one of the main characters of my Lego(tm) stopmotion film [The Adventures Of Charlie and George Decker](https://youtu.be/-aJDTe_obKI), which I made a few years ago and is available on YouTube.


## What's the status of the game?
The game is in development and I already implemented a lot of the features I
had in mind, when I started this project. The graphic engine is complete,
player-physics is done and collision detection between the player, the world
and the objects inside it is working. The test level is fully playable. The
sound engine is finished, too.

### Things which are ongoing

I created a [kanban board](https://github.com/pfedick/DeckerGame/projects/1) on
github to track my progress.


### Things I have to do or figure out
- create interactive objects / enemies:
    - scorpion
    - huge vent
    - stamper
    - Wallenstein + Helena? (from the movie)
    - some kind of angry bird?
    - side wind
- compose sound track
- create sound effects
- support mono audio files
- build the world/levels
- story?
- save and load of progress in a level
- create start screen
- end the game, when player dies and has no lifes rest


There is probably more, which I'm not aware of, yet ;-)

### Things I have finished
- basic ui for editing and testing the world
- how to build and render the tile-based world (tiles, layers)
- how to move in the world (side scrolling, paralax)
- how to implement the sprites?
- which resolution should the game have
- what is the best size for tiles and sprites?
- how to implement interactive objects/enemies
- how to control the main character?
- physics
- collision detection
- create interactive objects / enemies:
    - spears from ground
    - gems (can be collected => points)
    - coins (can be collected  => points)
    - medikit
    - savepoint
    - rat
    - spider
    - skeleton
    - mummy
    - dissolving ground
    - floater
    - lasers
    - arrows from wall
    - locked door
    - key (used to unlock a locked door)
    - mushroom (white minifig with big red hat and white dots)
- how does sound work with SDL?
- write the audio engine
- how to integrate sound track and sound effects into the game
- game mechanics (hallways, doors, traps, ladders, walk, run, jump, fall)
- world editor
- death animation for objects (player, skeleton)
- positional sound: objects in the world shot be able to emit sound effects.
  The loudness and direction (left/right) should depend on the distance to
  the player.
- ui for special objects
- refine physics for jumping & falling

## How to control the player

The player can be controlled by the arrow keys or alternatively with "wasd" or "ijkl"
- cursor up / w / i: climb up a ladder or jump up in the air
- cursor down / s / k: climb down a ladder
- cursor left / a / j: walk to the left
- cursor right / d / l: walk to the right
- shift left / shift right: walk faster, jump higher

When walking an pressing the key for up, the player will jump in the direction of walking.


## On what platforms will the game be available
My target platforms are:

- Linux (development is done on Fedora 34)
- Windows 10

The game will be open source and uses the [Simple DirectMedia Layer](https://www.libsdl.org/)
library, which is available on many platforms. So in theory it should be possible to
compile it on any platform with a c++ compiler and the SDL library.

## In which language is the game written?

It is written in my favorite language: C++


## When will it come out?
I don't know. This is one of my hobbies, but I have others, too :-)
Maybe I will never finish it or decide to do something else. We will see...

But hey, you can already play the game! See below, how to compile it your
own, or download a preview version on [GitHub](https://github.com/pfedick/DeckerGame/releases).



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
- mpg123
- ffmpeg-libs

#### Fedora
    sudo dnf install -y git gcc gcc-c++ make pcre-devel SDL2-devel zlib-devel \
        bzip2-devel libpng-devel mpg123-devel ffmpeg-devel

#### Ubuntu
    sudo apt-get install -y git gcc g++ libpcre3-dev libbz2-dev zlib1g-dev libsdl2-dev \
        libpng-dev libmpg123-dev libavcodec-dev libavformat-dev libswscale-dev libavutil-dev

#### Windows 10 with mingw64/msys

For installing msys2 and mingw64, please see: [https://www.msys2.org/](https://www.msys2.org/)

    pacman -S git make mingw-w64-x86_64-gcc mingw-w64-x86_64-zlib mingw-w64-x86_64-libiconv \
         mingw-w64-x86_64-bzip2 mingw-w64-x86_64-SDL2 mingw-w64-x86_64-mpg123 \
         mingw-w64-x86_64-pcre mingw-w64-x86_64-libpng


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
