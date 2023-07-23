[![C/C++ CI](https://github.com/pfedick/DeckerGame/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/pfedick/DeckerGame/actions/workflows/c-cpp.yml)

# Decker - The Game

<p align="center">
  <img src="https://github.com/pfedick/DeckerGame/raw/main/res/screenshots/Screenshot_20210724_193500.jpg" />
</p>


This is my attempt of writing a Jump'n'Run game in C++. It uses the [Simple DirectMedia Layer](https://www.libsdl.org/) library for graphics and my own library [pplib](https://github.com/pfedick/pplib) for graphics and ui. And it is open source!

It features George Decker, one of the main characters of my Lego(tm) stopmotion film [The Adventures Of Charlie and George Decker](https://youtu.be/-aJDTe_obKI), which I made a few years ago and is available on YouTube.


## What's the status of the game?
The game is in development and I already implemented a lot of the features I
had in mind, when I started this project. The graphics and sound engine is complete,
we have physics and a particle system, where we can generate smoke, fire, rain and water.
A test level is complete and fully playable and I'm working on more levels with different themes.

### Things which are ongoing

I created a [project board](https://github.com/users/pfedick/projects/3) on github to track all things I'm working on and which are in my backlog.


### Things I still have in mind
- create more interactive objects / enemies:
    - huge vent
    - give Helena (enemy from my movie) a weappon
    - side wind
    - swimming
    - underwater sections with limited air
- create more sound effects
- build more levels
- story?
- save and load of progress in a level?

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
    - scorpion
    - ostrich
    - Wallenstein
    - Helena (but she make no damage yet)
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
- particle effects

## How to control the player

The player can be controlled by the arrow keys or alternatively with "wasd" or "ijkl"
- cursor up / w / i: climb up a ladder or jump up in the air
- cursor down / s / k: climb down a ladder
- cursor left / a / j: walk to the left
- cursor right / d / l: walk to the right
- shift left / shift right: walk faster, jump higher
- e: action key, it activates switches or open doors

When walking an pressing the key for up, the player will jump in the direction of walking.


## On what platforms will the game be available
My target platforms are:

- Linux (development is done on Fedora)
- Windows 10

The game will be open source and uses the [Simple DirectMedia Layer](https://www.libsdl.org/)
library, which is available on many platforms. So in theory it should be possible to
compile it on any platform with a c++ compiler and the SDL library available.

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

#### Fedora
```bash
sudo dnf install -y git gcc gcc-c++ make pcre-devel SDL2-devel zlib-devel \
    bzip2-devel libpng-devel mpg123-devel libjpeg-turbo-devel
```

#### Ubuntu
```bash
sudo apt-get install -y git gcc g++ libpcre3-dev libbz2-dev zlib1g-dev \
    libsdl2-dev libpng-dev libmpg123-dev
```

#### Windows 10 with mingw64/msys

For installing msys2 and mingw64, please see: [https://www.msys2.org/](https://www.msys2.org/)

```bash
pacman -S git make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-zlib \
    mingw-w64-ucrt-x86_64-libiconv mingw-w64-ucrt-x86_64-bzip2 \
    mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-mpg123 \
    mingw-w64-ucrt-x86_64-pcre mingw-w64-ucrt-x86_64-libpng \
    mingw-w64-ucrt-x86_64-libjpeg-turbo
```

### Checkout and compile

```bash
git clone --recurse-submodules https://github.com/pfedick/DeckerGame.git
cd DeckerGame
./configure
make
# for faster compile you can add the paraneter "-j CORES", where CORES is the number
# of cores on you cpu:
make -j 8

# run the game:
./decker
```

If you already cloned the project and want to update to the newest version:

```bash
git pull
git submodule update --init --recursive
./configure
make
```

## Social media
You can find a lot of videos about the game on my channel on YouTube and Twitch:

* YouTube: https://www.youtube.com/@PatrickFedick
* Twitch: https://www.twitch.tv/pfedick
* Mastodon: https://mastodon.de/@patrickf


## Release History

* 0.9.0 [coming soon!]
* [0.8.2](https://github.com/pfedick/DeckerGame/releases/tag/0.8.2) [2023-07-14]: Bugfix release
* [0.8.0](https://github.com/pfedick/DeckerGame/releases/tag/0.8.0) [2023-07-13]: Level consistency
* [0.7.0](https://github.com/pfedick/DeckerGame/releases/tag/0.7.0) [2022-07-24]: Particle Update
* [0.6.1](https://github.com/pfedick/DeckerGame/releases/tag/0.6.1) [2022-07-15]: Holiday update - Summer 2022
* [0.5.0](https://github.com/pfedick/DeckerGame/releases/tag/0.5.0) [2022-06-28]: One-year-wrap-up
* [0.4.0](https://github.com/pfedick/DeckerGame/releases/tag/0.4.0) [2021-08-08]: Refactoring of game mechanics
* [0.3.0](https://github.com/pfedick/DeckerGame/releases/tag/0.3.0) [2021-08-01]: We have sound!
* [0.2.1](https://github.com/pfedick/DeckerGame/releases/tag/0.2.1) [2021-07-25]: Lots of new traps and enemies
* [0.2.0](https://github.com/pfedick/DeckerGame/releases/tag/0.2.0) [2021-07-21]: More paralax planes, graphics and objects
* [0.1.0](https://github.com/pfedick/DeckerGame/releases/tag/0.1.0) [2021-07-14]: It's a brick world
* [0.0.5](https://github.com/pfedick/DeckerGame/releases/tag/0.0.5) [2021-07-12]: Sprite system is working
* [0.0.2](https://github.com/pfedick/DeckerGame/releases/tag/0.0.2) [2021-07-08]: Tile system is working
* [0.0.1](https://github.com/pfedick/DeckerGame/releases/tag/0.0.1) [2021-07-04]: And so it begins
