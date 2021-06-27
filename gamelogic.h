#pragma once
#include "defs.h"
#include "entity.h"
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
  SDL_Texture *bulletTexture;
  SDL_Texture *enemyTexture;
  SDL_Texture *alienBulletTexture;
};
