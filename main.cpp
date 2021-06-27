#include "controller.h"
#include "renderer.h"
#include "game.h"
#include <iostream>
int main(int argc, char* argv[])
{
	const size_t SCREEN_WIDTH = 640;
	const size_t SCREEN_HEIGHT = 480;
	SDL_Event event;
	Renderer renderer(SCREEN_WIDTH, SCREEN_HEIGHT);
	Controller controller(event);
	Game game(renderer, controller);
	game.Run();
	return 0;
}