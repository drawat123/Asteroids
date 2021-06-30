#pragma once
#include "entity.h"
#include <SDL_ttf.h>
#include <memory>
using std::make_unique;
using std::unique_ptr;

class GameDraw {
public:
  GameDraw();
  ~GameDraw();
  void draw();

private:
  void drawBackground();
  void drawPlayer();
  void drawBullets();
  void drawFighters();
  void drawDebris();
  void drawExplosions();
  void updateFPS();
  int backgroundX;
  SDL_Texture *background;
  SDL_Texture *explosionTexture;
};
