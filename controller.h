#pragma once
#include "defs.h"
#include "entity.h"
#include <SDL.h>

// Handles input
class Controller {
public:
  Controller(SDL_Event &e);
  void HandleInput(bool &running);
  int keyboard[MAX_KEYBOARD_KEYS];

private:
  void doKeyDown(SDL_KeyboardEvent *event);
  void doKeyUp(SDL_KeyboardEvent *event);
  SDL_Event &event;
};
