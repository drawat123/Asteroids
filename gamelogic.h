#pragma once
#include "defs.h"
#include "entity.h"
#include "gamesound.h"
#include <memory>

using std::make_unique;
using std::unique_ptr;

class GameLogic {
public:
  GameLogic();
  ~GameLogic();
  GameState gameState = GameState::stop;
  void logic();
  void setGameState(GameState g = GameState::start);

private:
  void doPlayer();
  void fireBullet();
  void doFighters();
  int playerHitFighter(Entity *b);
  void doBullets();
  int bulletHitFighter(Entity *b);
  void spawnEnemies();
  void doEnemies();
  void fireAlienBullet(Entity *e);
  void addExplosions(int x, int y, int num);
  void doExplosions();
  void addDebris(Entity *e);
  void doDebris();

  GameSound gameSound;
  SDL_Texture *bulletTexture;
  SDL_Texture *enemyTexture;
  SDL_Texture *alienBulletTexture;
};
