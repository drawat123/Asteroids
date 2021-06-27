#include "game.h"
#include <iostream>
Game::Game()
    : SCREEN_WIDTH{640}, SCREEN_HEIGHT{480},
      renderer(SCREEN_WIDTH, SCREEN_HEIGHT),
      controller(event), running{true}, fps{""}, player{nullptr},
      frame_count{0}, enemySpawnTimer{0}, font{nullptr}, bgfont{nullptr},
      title_timestamp{SDL_GetTicks()}, stageResetTimer{0}, score{0}, highScore{
                                                                         0} {
  font = TTF_OpenFont("font\\font.ttf", 24);
  bgfont = TTF_OpenFont("font\\bgfont.ttf", 44);
  background_play_text = drawFont(
      "                     Asteriods\n\n        Press space to start",
      renderer.getScreenWidth() / 2, renderer.getScreenHeight() / 2, bgfont);
  background_restart_text = drawFont(
      "                GAME OVER!!!\n\n    Press space to restart",
      renderer.getScreenWidth() / 2, renderer.getScreenHeight() / 2, bgfont);
  background_text = background_play_text.get();
  playerTexture = IMG_LoadTexture(renderer.getRenderer(), "gfx\\player.png");

  resetStage();
}

Game::~Game() {
  TTF_CloseFont(font);
  TTF_CloseFont(bgfont);
  SDL_DestroyTexture(playerTexture);
  player = nullptr;
  background_text = nullptr;
}

string Game::calcFrameRate() {
  std::lock_guard<mutex> lk(m);
  if (frame_end - title_timestamp >= 1000) {
    int f = frame_count;
    frame_count = 0;
    title_timestamp = frame_end;
    return {"FPS: " + std::to_string(f)};
  }
  return fps;
}

void Game::Run() {
  gameLogic = make_unique<GameLogic>();
  gameDraw = make_unique<GameDraw>();
  long then = SDL_GetTicks();
  float remainder = 0;

  constexpr std::size_t target_frame_duration{1000 / 60};
  Uint32 frame_start;
  Uint32 frame_duration;

  while (running) {
    frame_start = SDL_GetTicks();

    renderer.prepareScene();
    controller.HandleInput(running);

    gameLogic->setGameState();
    if (gameLogic->gameState == GameState::start) {
      gameLogic->logic();
    }

    gameDraw->draw();

    frame_end = SDL_GetTicks();
    frame_count++;
    frame_duration = frame_end - frame_start;
    future<std::string> ftr =
        std::async(std::launch::async, &Game::calcFrameRate, this);
    fps = ftr.get();

    renderer.presentScene();

    if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }
  }
}

void Game::initPlayer() {
  unique_ptr<Entity> p = make_unique<Entity>();
  player = p.get();

  p->x = 100;
  p->y = 100;
  p->texture = playerTexture;
  p->side = Entity::Type::SIDE_PLAYER;
  SDL_QueryTexture(p->texture, NULL, NULL, &p->w, &p->h);
  fighters.push_back(std::move(p));
}

void Game::resetStage() {
  fighters.clear();

  bullets.clear();

  enemySpawnTimer = 0;

  stageResetTimer = FPS * 2;

  highScore = max(score, highScore);
  score = 0;
  renderer.UpdateWindowTitle(score, highScore);
}

unique_ptr<Entity> Game::drawFont(std::string textureText, int x, int y,
                                  TTF_Font *font) {
  SDL_Color textColor = {255, 255, 255};
  SDL_Surface *textSurface =
      TTF_RenderText_Blended_Wrapped(font, textureText.c_str(), textColor, 600);

  SDL_Texture *fontTexture =
      SDL_CreateTextureFromSurface(renderer.getRenderer(), textSurface);
  SDL_Rect dest;
  SDL_QueryTexture(fontTexture, NULL, NULL, &dest.w, &dest.h);

  unique_ptr<Entity> e = make_unique<Entity>();
  e->x = x - ((dest.w) / 2);
  e->y = y - ((dest.h) / 2);
  e->texture = fontTexture;

  SDL_FreeSurface(textSurface);

  return std::move(e);
}