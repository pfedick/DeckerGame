#include "decker.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <SDL.h>
#include <ppl7-grafix.h>
#include <ppl7-tk.h>
#include "audio.h"
#include <math.h>
#include "objects.h"

#define DEBUGTIME
void help()
{
	printf("Usage:\n\n");
	printf("  -l LEVEL   load a level without presenting the start screen\n");
#ifdef DEBUGTIME
	printf("  -d         Starts the level compiled for debug\n");
#endif
	fflush(stdout);
}


void start(int argc, char** argv)
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

	if (ppl7::HaveArgv(argc, argv, "-h") || ppl7::HaveArgv(argc, argv, "--help")) {
		help();
		return;
	}


	SDL_Init(SDL_INIT_VIDEO);

	ppl7::grafix::Grafix gfx;
	ppl7::tk::WindowManager_SDL2 wm;
	Game game;


	game.init();
	game.init_grafix();


#ifdef DEBUGTIME
	if (ppl7::HaveArgv(argc, argv, "-d") && ppl7::File::exists("Makefile")) {
		//game.playIntroVideo();
		game.startLevel("level/tutorial.lvl");
		//game.startLevel("level/waynet.lvl");
		//game.startLevel("level/sound.lvl");
		//game.startLevel("level/desert.lvl");
		//game.startLevel("level/heaven.lvl");
		//game.startLevel("level/water.lvl");
		//game.startLevel("level/devel.lvl");
		//game.startLevel("level/test.lvl");
		/*
		game.startLevel("level/start.lvl");
		Decker::Objects::LevelEnd* leo=static_cast<Decker::Objects::LevelEnd*>(Decker::Objects::GetObjectSystem()->getObject(64));
		if (leo && leo->type() == Decker::Objects::Type::LevelEnd) {
			leo->state=Decker::Objects::LevelEnd::State::Inactive;

		}
		*/
		//game.startLevel("level/levelstats_test.lvl");
		//game.startLevel("level/particle.lvl");
		game.showUi(true);
		game.run();

		return;
	}
#endif
	if (ppl7::HaveArgv(argc, argv, "-l")) {
		ppl7::String level=ppl7::GetArgv(argc, argv, "-l");
		if (!ppl7::File::exists(level)) {
			level="level/" + level;
		}
		if (ppl7::File::exists(level)) {
			game.startLevel(level);
			game.showUi(true);
			game.run();
			return;
		}
		printf("ERROR: Level not found [%s]\n", (const char*)ppl7::GetArgv(argc, argv, "-l"));
		return;

	}

	//game.playIntroVideo();

	AudioStream GeorgeDeckerTheme("res/audio/PatrickF-George_Decker_Theme.mp3", AudioClass::Music);

	while (1) {
		GameState state=game.showStartScreen(GeorgeDeckerTheme);
		if (state == GameState::QuitGame) return;
		else if (state == GameState::StartGame) {
			game.showUi(false);
			game.resetPlayer();
			game.startLevel("level/heaven.lvl");
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
	start(__argc, __argv);
	return 0;
	try {
		start(__argc, __argv);
		return 0;
	} catch (const ppl7::Exception& ex) {
		ex.print();
		throw;
		return 1;
	}
	return 0;
}

int main(int argc, char** argv)
{

	start(argc, argv);
	return 0;
}
