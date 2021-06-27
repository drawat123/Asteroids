#include "renderer.h"
#include <iostream>
#include <string>

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height)
    : screen_width(screen_width), screen_height(screen_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow(
      "Asteroids          Score: ", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

  // Create renderer
  sdl_renderer = SDL_CreateRenderer(
      sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    std::cerr << "SDL_image could not initialize! SDL_Image Error: \n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Initialize SDL_ttf
  if (TTF_Init() == -1) {
    std::cerr << "SDL_ttf could not initialize! SDL_ttf Error: \n";
    std::cerr << "SDL_Error: " << TTF_GetError() << "\n";
  }
}

void Renderer::prepareScene() {
  SDL_SetRenderDrawColor(sdl_renderer, 32, 32, 32, 255);
  SDL_RenderClear(sdl_renderer);
}

void Renderer::presentScene() { SDL_RenderPresent(sdl_renderer); }

int Renderer::getScreenWidth() const { return screen_width; }

int Renderer::getScreenHeight() const { return screen_height; }

void Renderer::Render(Entity *e) {
  if (e == nullptr) {
    return;
  }
  blit(e->texture, e->x, e->y);
}

SDL_Renderer *Renderer::getRenderer() const { return sdl_renderer; }

void Renderer::blit(SDL_Texture *texture, int x, int y) {
  SDL_Rect dest;

  dest.x = x;
  dest.y = y;

  SDL_QueryTexture(texture, NULL, NULL, &dest.w, &dest.h);

  SDL_RenderCopy(sdl_renderer, texture, NULL, &dest);
}

void Renderer::UpdateWindowTitle(int score, int maxScore) {
  std::string title{"Asteroids          Score: " + std::to_string(score) +
                    "          High Score: " + std::to_string(maxScore)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}

Renderer::~Renderer() {
  SDL_DestroyRenderer(sdl_renderer);
  SDL_DestroyWindow(sdl_window);
  sdl_window = nullptr;
  sdl_renderer = nullptr;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}