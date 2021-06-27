#include "controller.h"
#include "renderer.h"
#include "game.h"
#include <iostream>

Game* Game::obj_ = nullptr;

int main(int argc, char* argv[])
{
	Game::GetInstance();
	Game::GetInstance()->Run();
	return 0;
}