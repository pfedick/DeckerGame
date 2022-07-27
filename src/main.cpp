#include "decker.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "audio.h"
#include <math.h>


void help()
{

}


void start()
{
#ifdef WIN32
	ppl7::String::setGlobalEncoding("UTF-8");
#endif

	if (setlocale(LC_CTYPE, "") == NULL) {
		printf("setlocale fehlgeschlagen\n");
		throw std::exception();
	}
	if (setlocale(LC_NUMERIC, "C") == NULL) {
		printf("setlocale fuer LC_NUMERIC fehlgeschlagen\n");
		throw std::exception();
	}
	SDL_Init(SDL_INIT_VIDEO);

	ppl7::grafix::Grafix gfx;
	ppl7::tk::WindowManager_SDL2 wm;
	Game game;
	game.init();
	game.init_grafix();


	//game.startLevel("level/test.lvl");
	game.startLevel("level/heaven.lvl");
	//game.startLevel("level/particle.lvl");
	game.showUi(true);
	game.run();
	return;





	//game.playIntroVideo();

	AudioStream GeorgeDeckerTheme("res/audio/PatrickF-George_Decker_Theme.mp3", AudioClass::Music);

	while (1) {
		GameState state=game.showStartScreen(GeorgeDeckerTheme);
		if (state == GameState::QuitGame) return;
		else if (state == GameState::StartGame) {
			game.showUi(false);
			game.resetPlayer();
			game.startLevel("level/test.lvl");
			game.enableControls(true);
			game.run();
		} else if (state == GameState::StartEditor) {
			game.showUi(true);
			LevelParameter default_params;
			game.createNewLevel(default_params);
			game.resetPlayer();
			game.openNewLevelDialog();
			game.run();
		} else if (state == GameState::ShowSettings) {
		}
	}
	fflush(stdout);
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
