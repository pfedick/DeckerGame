#include "decker.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>

void help()
{

}

void start()
{
	ppl7::grafix::Grafix gfx;
	ppl7::tk::WindowManager_SDL2 wm;

	Game game;
	game.init();
	game.run();

}

int WinMain()
{
	try {
		start();
		return 0;
	} catch (const ppl7::Exception &ex) {
		ex.print();
		return 1;
	}
	return 0;
}

int main()
{
	return WinMain();
}
