#pragma once
#include "entity.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <memory>
#include <vector>

using std::make_shared;
using std::shared_ptr;

class Renderer {
public:
  Renderer(const std::size_t screen_width, const std::size_t screen_height);

  void Render(Entity *e);
  SDL_Renderer *getRenderer() const;
  void prepareScene();
  void presentScene();
  int getScreenWidth() const;
  int getScreenHeight() const;
  void UpdateWindowTitle(int score, int maxScore);

  ~Renderer();

private:
  void blit(SDL_Texture *texture, int x, int y);
  SDL_Window *sdl_window;
  SDL_Renderer *sdl_renderer;
  const std::size_t screen_width;
  const std::size_t screen_height;
};
