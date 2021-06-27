#include "controller.h"
#include <iostream>

Controller::Controller(SDL_Event& e) :event(e), keyboard{ 0 } {}

void Controller::HandleInput(bool& running) {

	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			running = false;
		}
		else if (event.type == SDL_KEYDOWN) {
			doKeyDown(&event.key);
		}
		else if (event.type == SDL_KEYUP) {
			doKeyUp(&event.key);
		}
	}
}

void Controller::doKeyDown(SDL_KeyboardEvent* event) {
	if (event->repeat == 0 && event->keysym.scancode < 350) {
		keyboard[event->keysym.scancode] = 1;
	}
}

void Controller::doKeyUp(SDL_KeyboardEvent* event) {
	if (event->repeat == 0 && event->keysym.scancode < 350) {
		keyboard[event->keysym.scancode] = 0;
	}
}