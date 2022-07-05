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
	Game game;
	game.init();
	/*
	game.startLevel("level/start.lvl");
	game.showUi(false);
	game.run();

	game.enableControls(false);
	*/

	game.init_grafix();
	while (1) {
		GameState state=game.showStartScreen();
		if (state == GameState::QuitGame) return;
		else if (state == GameState::StartGame) {
			game.showUi(false);
			game.resetPlayer();
			game.startLevel("level/test.lvl");
			game.enableControls(true);
			game.run();
		} else if (state == GameState::StartEditor) {
			game.showUi(true);
			game.clearLevel();

			game.resetPlayer();
			game.run();
		} else if (state == GameState::ShowSettings) {
		}

	}
}

int WinMain()
{
	start();
	return 0;
	try {
		start();
		return 0;
	}
	catch (const ppl7::Exception& ex) {
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
