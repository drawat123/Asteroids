#pragma once
#include <SDL.h>
class Debris {
public:
  float x;
  float y;
  float dx;
  float dy;
  int life;
  SDL_Rect rect;
  SDL_Texture *texture;
};
