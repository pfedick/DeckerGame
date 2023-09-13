#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>

#include "gamecontroller.h"



int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);
	std::list<GameController::Device> device_list=GameController::enumerate();
	printf("we have %d controllers\n", (int)device_list.size());
	std::list<GameController::Device>::const_iterator it;
	for (it=device_list.begin();it != device_list.end();++it) {
		printf("   %d: %s\n", (*it).id, (const char*)(*it).name);
	}

	GameController::Device device=device_list.front();

	GameController gc;
	gc.open(device);

	SDL_Window* window=NULL;
	SDL_Renderer* renderer=NULL;

	SDL_CreateWindowAndRenderer(300, 200, SDL_WINDOW_SHOWN, &window, &renderer);

	SDL_Event event;

	printf("loop\n");
	SDL_KeyboardEvent* ev;
	while (1) {
		while (SDL_PollEvent(&event))
		{

			switch (event.type)
			{
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				ev=(SDL_KeyboardEvent*)&event;
				ppl7::PrintDebugTime("Key event: %d, key: %d, mod: %d\n", ev->type, ev->keysym.sym, ev->keysym.mod);
				break;
			case SDL_QUIT:
				return 0;

			}


		}
	}
	return 0;
}
