[![Ubuntu](https://github.com/pfedick/DeckerGame/actions/workflows/ubuntu.yml/badge.svg)](https://github.com/pfedick/DeckerGame/blob/main/.github/workflows/ubuntu.yml)
[![Windows](https://github.com/pfedick/DeckerGame/actions/workflows/msys2.yml/badge.svg)](https://github.com/pfedick/DeckerGame/blob/main/.github/workflows/msys2.yml)

# George Decker - The Game
<p align="center">
  <img src="https://github.com/pfedick/DeckerGame/raw/main/res/screenshots/decker_0_17_0b.jpg" />
</p>

## What is the game about?
I always wanted to program a game, but never got very far with it. But I didn't give up and this is the result!

"George Decker" is a classic Jump'n'Run game like Super Mario Brothers. You have to collect coins and other things,
avoid enemies and traps, find keys to open doors and solve other small puzzles.

It features George Decker, one of the main characters of my Lego(tm) stopmotion film [The Adventures Of Charlie and George Decker](https://youtu.be/-aJDTe_obKI), which I made a few years ago and is available on YouTube.


## What does it cost?
Nothing!

This game is completely free and open source! I provide finished binaries for Windows for download on my [github page](https://github.com/pfedick/DeckerGame/releases), but you can always download the sources and compile them on your favorite operating system, including Linux and FreeBSD! (see below on [how to compile](#How-to-compile))

You can even fork the sourcecode and make your own modifications!

## Where can I download it?
- Latest Release for Windows: [GeorgeDecker-0.17.1-Setup.exe](https://github.com/pfedick/DeckerGame/releases/download/0.17.0/GeorgeDecker-0.17.1-Setup.exe) (ca. 270 MB)
- Sourcecode of latest release: [DeckerGame-0.17.1.tar.gz](https://github.com/pfedick/DeckerGame/archive/refs/tags/0.17.1.tar.gz) (ca. 829 MB)
- GitHub: https://github.com/pfedick/DeckerGame


I provide binaries for previous releases on my [github page](https://github.com/pfedick/DeckerGame/releases).


## Where can I find more information about the game?
You can find a lot of videos about the game on my channel on YouTube and Twitch, I'm also on mastodon and discord:

* Mastodon: https://mastodon.de/@patrickf
* YouTube: https://www.youtube.com/@PatrickFedick
* Twitch: https://www.twitch.tv/pfedick
* Discord: https://discord.gg/QZXPNFKNKP

## What's the status of the game?
The game is in development since summer 2021 and all game mechanics are implemented. This includes
the graphic and sound engine, physics and particles, which can be used to generate smoke, fire, rain and water.
And of course, next to the playable character, there are a lot of enemies and traps.
There is a tutorial and several other playable levels, and it got a nice little render intro, too.

## How to control the player

### Keyboard
The player can be controlled by the arrow keys or alternatively with "wasd" or "ijkl"
- cursor up / w / i: climb up a ladder or jump up in the air
- cursor down / s / k: climb down a ladder
- cursor left / a / j: walk to the left
- cursor right / d / l: walk to the right
- shift left / shift right: walk faster, jump higher
- e / o: action key, it activates switches or open doors
- f: activates the flashlight, if the player has collected it (version 0.15.0 and above)
- space: skip the intro video

When walking an pressing the key for up, the player will jump in the direction of walking.

### Game Controller
Since version 0.14.0 the game supports game controllers. The controller is detected and used automatically (plug it in or switch it on). The mapping of sticks and buttons can be changed in the games settings.
- left stick left or right: walk or run in the direction of the stick
- left stick up: jump or climb up a ladder
- left stick down: climb down a ladder
- Button "A": action key, it activates switches or open doors. In a Menu it selects the highlighted element
- Button "B": alternative for jump
- Button "X": go back in a menu
- Button "Y": activates the flashlight, if the player has collected it (version 0.15.0 and above)
- Button "Menu": go to the settings
- Pad: navigate in a Menu, also alternative for player movement

If possible, the controller rumbles, when the player gets demage. You can disable this in the settings

## On what platforms will the game be available
My target platforms are:

- Linux (development is done on Fedora, it compiles without issues on Ubuntu, too)
- Windows 10 and 11
- The Windows version is known to run on the Steamdeck!

The game depends on the [Simple DirectMedia Layer](https://www.libsdl.org/)
library, which is available for many platforms. So in theory it should be possible to
compile it on any platform with a c++ compiler and the SDL library available.
See below on [how to compile](#How-to-compile).

## Can you make a release for iOS, Android, Debian, Ubuntu, Fedora,...
Sorry, but at this moment, that goes above my knowledge and time.

But, hey! It's open source! If you have the knowledge and time, please feel free to make a port.


## In which language is the game written?

It is written in C++.

I use  the [Simple DirectMedia Layer](https://www.libsdl.org/) library for low level grafix and sound output, but everything else is written by me, including the game engine.


## When will it come out?
It's already some kind of "out" :-)

The game is still in development, but I make regular preview releases (see my [GitHub](https://github.com/pfedick/DeckerGame/releases) page). You can also compile the latest version on your own (see below).

## When is it finished?

Good question, honest answer: I don't know. This is one of my hobbies, but I have others, too :-)
Maybe I will never finish it or decide to do something else. We will see...

## Known issues and limitations
I have three different systems available for developing and testing, but they all have in common, that display resoltion is around 1920 x 1080 pixel with a framerate of 60. I give my best to support other resoltions and frame rates, but I have limited possibilities to test.
- on systems with framerate other than 60, strange things can happen
- on systems with framerates lower than 30, everything will get slower and the physics engine will start to fail
- on monitors with resolutions other than 1920 x 1080, the output will be scaled up or down


How to avoid issues with different resolutions:
- when playing in a window, resize the window to a size of 1920 x 1080 pixel
- play in fullscreen (not window mode) with a fixed resolution of 1920 x 1080 pixel
- if framerate is higher than 60 and you run into issues, please report to me and try to configure your graphic card to limit the game at 60 Hz

## Bugreport
You can open an issue in my [github project](https://github.com/pfedick/DeckerGame/issues) or contact me on [discord](https://discord.gg/QZXPNFKNKP).

## What's missing, what's done?
### Things which are ongoing

I created a [project board](https://github.com/users/pfedick/projects/3) on github to track all things I'm working on and which are in my backlog.

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
- dav1d
- libvorbis
- lobogg

#### Fedora
```bash
sudo dnf install -y git gcc gcc-c++ make pcre-devel SDL2-devel zlib-devel \
    bzip2-devel libpng-devel mpg123-devel libjpeg-turbo-devel libdav1d-devel \
    libvorbis-devel libogg-devel
```

#### Ubuntu
```bash
sudo apt-get install -y git gcc g++ libpcre3-dev libbz2-dev zlib1g-dev \
    libsdl2-dev libpng-dev libmpg123-dev libdav1d-dev \
    libvorbis-dev libogg-dev
```

#### Windows 10 with mingw64/msys

For installing msys2 and mingw64, please see: [https://www.msys2.org/](https://www.msys2.org/)

```bash
pacman -S git make mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-zlib \
    mingw-w64-ucrt-x86_64-libiconv mingw-w64-ucrt-x86_64-bzip2 \
    mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-mpg123 \
    mingw-w64-ucrt-x86_64-pcre mingw-w64-ucrt-x86_64-libpng \
    mingw-w64-ucrt-x86_64-libjpeg-turbo mingw-w64-ucrt-x86_64-dav1d \
    mingw-w64-ucrt-x86_64-libvorbis mingw-w64-ucrt-x86_64-libogg
```

### Checkout and compile
You can download a release version [here](https://github.com/pfedick/DeckerGame/releases). If you want the newest version, you can check out the sources from github:

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


## Release History
* [0.17.1](https://github.com/pfedick/DeckerGame/releases/tag/0.17.1) [2024-08-25]: Companion & Pyramid Update
* [0.16.0](https://github.com/pfedick/DeckerGame/releases/tag/0.16.0) [2024-07-11]: UI overhaul
* [0.15.1](https://github.com/pfedick/DeckerGame/releases/tag/0.15.1) [2024-03-10]: Skullmaster update
* [0.15.0](https://github.com/pfedick/DeckerGame/releases/tag/0.15.0) [2024-03-04]: Light and Dark update
* [0.14.0](https://github.com/pfedick/DeckerGame/releases/tag/0.14.0) [2023-09-17]: Play it with controller
* [0.13.0](https://github.com/pfedick/DeckerGame/releases/tag/0.13.0) [2023-09-01]: Intro
* [0.12.0](https://github.com/pfedick/DeckerGame/releases/tag/0.12.0) [2023-08-19]: Tutorial and Speech
* [0.11.0](https://github.com/pfedick/DeckerGame/releases/tag/0.11.0) [2023-08-10]: Pathfinding and Blood Update
* [0.10.0](https://github.com/pfedick/DeckerGame/releases/tag/0.10.0) [2023-07-30]: Water update
* [0.9.0](https://github.com/pfedick/DeckerGame/releases/tag/0.9.0) [2023-07-24]: Tropical update
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
