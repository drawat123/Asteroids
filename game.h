#pragma once
#include "SDL.h"
#include "controller.h"
#include "debris.h"
#include "entity.h"
#include "explosion.h"
#include "gamedraw.h"
#include "gamelogic.h"
#include "renderer.h"
#include <SDL_ttf.h>
#include <algorithm>
#include <future>
#include <list>
#include <memory>
#include <thread>

using std::future;
using std::list;
using std::make_unique;
using std::max_element;
using std::mutex;
using std::promise;
using std::string;
using std::thread;
using std::unique_ptr;

class Game {
private:
  Game();
  string calcFrameRate();
  static Game *obj_;
  unique_ptr<GameLogic> gameLogic;
  unique_ptr<GameDraw> gameDraw;

public:
  static Game *GetInstance() {
    if (obj_ == nullptr) {
      obj_ = new Game();
    }
    return obj_;
  }
  ~Game();
  void Run();
  void initPlayer();
  void resetStage();
  unique_ptr<Entity> drawFont(string textureText, int x, int y, TTF_Font *font);

  const size_t SCREEN_WIDTH;
  const size_t SCREEN_HEIGHT;
  Renderer renderer;
  Controller controller;
  Entity *player;
  string fps;
  list<unique_ptr<Entity>> bullets;
  list<unique_ptr<Entity>> fighters;
  SDL_Texture *playerTexture;
  Entity *background_text;
  unique_ptr<Entity> background_play_text;
  unique_ptr<Entity> background_restart_text;
  SDL_Event event;
  Uint32 title_timestamp;
  Uint32 frame_end;
  mutex m;
  TTF_Font *font;
  TTF_Font *bgfont;
  list<unique_ptr<Explosion>> explosion;
  list<unique_ptr<Debris>> debris;

  bool running;
  int enemySpawnTimer;
  int stageResetTimer;
  int score;
  int highScore;
  int frame_count;
};