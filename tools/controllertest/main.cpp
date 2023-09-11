#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ppl7.h>
#include <ppl7-grafix.h>

#include "gamecontroller.h"



int main(int argc, char** argv)
{
	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK);
	std::list<GameController::Device> device_list=GameController::enumerate();
	printf("we have %zd controllers\n", device_list.size());

	GameController::Device device=device_list.front();

	GameController gc;
	gc.open(device);

	return 0;
}
