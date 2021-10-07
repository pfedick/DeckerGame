#include "decker.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "audio.h"

void help()
{

}

void start()
{
	ppl7::grafix::Grafix gfx;
	ppl7::tk::WindowManager_SDL2 wm;

	/*
	AudioSystem audio;
	audio.test();
	return;
	*/
	Game game;
	game.init();
	game.startLevel("level/start.lvl");
	game.showUi(false);
	game.run();

	game.startLevel("level/test.lvl");
	game.showUi(false);

	game.run();

}

int WinMain()
{
	start();
	return 0;
	try {
		start();
		return 0;
	} catch (const ppl7::Exception &ex) {
		ex.print();
		throw;
		return 1;
	}
	return 0;
}

int main()
{
	return WinMain();
}
