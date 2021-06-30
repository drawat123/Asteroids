#include "gamelogic.h"
#include "game.h"

Game *gameInstance = Game::GetInstance();

GameLogic::GameLogic() : gameState{GameState::stop} {
  bulletTexture = IMG_LoadTexture(gameInstance->renderer.getRenderer(),
                                  "gfx\\playerBullet.png");
  enemyTexture =
      IMG_LoadTexture(gameInstance->renderer.getRenderer(), "gfx\\enemy.png");
  alienBulletTexture = IMG_LoadTexture(gameInstance->renderer.getRenderer(),
                                       "gfx\\alienBullet.png");
}

GameLogic::~GameLogic() {
  SDL_DestroyTexture(bulletTexture);
  SDL_DestroyTexture(enemyTexture);
  SDL_DestroyTexture(alienBulletTexture);
}

void GameLogic::logic() {
  doPlayer();

  doFighters();

  doBullets();

  spawnEnemies();

  doEnemies();

  if (gameInstance->player == nullptr && --gameInstance->stageResetTimer <= 0) {
    setGameState(GameState::stop);
  }
}

void GameLogic::doPlayer() {
  if (gameInstance->player != nullptr) {
    gameInstance->player->dx = gameInstance->player->dy = 0;

    if (gameInstance->player->reload > 0) {
      gameInstance->player->reload--;
    }

    if (gameInstance->controller.keyboard[SDL_SCANCODE_UP] &&
        (gameInstance->player->y - 4) > 0) {
      gameInstance->player->dy = -PLAYER_SPEED;
    }

    if (gameInstance->controller.keyboard[SDL_SCANCODE_DOWN] &&
        (gameInstance->player->y + 4) <
            gameInstance->SCREEN_HEIGHT - gameInstance->player->h) {
      gameInstance->player->dy = PLAYER_SPEED;
    }

    if (gameInstance->controller.keyboard[SDL_SCANCODE_LEFT] &&
        (gameInstance->player->x - 4) > 0) {
      gameInstance->player->dx = -PLAYER_SPEED;
    }

    if (gameInstance->controller.keyboard[SDL_SCANCODE_RIGHT] &&
        (gameInstance->player->x + 4) <
            gameInstance->SCREEN_WIDTH - gameInstance->player->w) {
      gameInstance->player->dx = PLAYER_SPEED;
    }

    if (gameInstance->controller.keyboard[SDL_SCANCODE_LCTRL] &&
        gameInstance->player->reload == 0) {
      fireBullet();
    }

    gameInstance->player->x += gameInstance->player->dx;
    gameInstance->player->y += gameInstance->player->dy;
  }
}

void GameLogic::fireBullet() {
  unique_ptr<Entity> bullet;

  bullet = make_unique<Entity>();

  bullet->x = gameInstance->player->x;
  bullet->y = gameInstance->player->y;
  bullet->dx = PLAYER_BULLET_SPEED;
  bullet->health = 1;
  bullet->side = Entity::Type::SIDE_PLAYER;
  bullet->texture = bulletTexture;
  SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

  bullet->y += (gameInstance->player->h / 2) - (bullet->h / 2);

  gameInstance->bullets.push_back(std::move(bullet));

  gameInstance->player->reload = 8;
}

void GameLogic::doFighters() {
  Entity *e;
  for (list<unique_ptr<Entity>>::iterator itr = gameInstance->fighters.begin();
       itr != gameInstance->fighters.end();) {
    e = (*itr).get();
    e->x += e->dx;
    e->y += e->dy;
    if (e != gameInstance->player && (e->x < -e->w || playerHitFighter(e))) {
      e->health = 0;
    }
    if (e->health == 0) {
      if (e == gameInstance->player) {
        gameInstance->player = nullptr;
      }
      itr = gameInstance->fighters.erase(itr);
    } else if (itr != gameInstance->fighters.end()) {
      itr++;
    }
  }
}

int GameLogic::playerHitFighter(Entity *e) {
  if (gameInstance->player != nullptr &&
      gameInstance->player->side != e->side &&
      collision(e->x, e->y, e->w, e->h, gameInstance->player->x,
                gameInstance->player->y, gameInstance->player->w,
                gameInstance->player->h)) {
    e->health = 0;
    return 1;
  }

  return 0;
}

void GameLogic::doBullets() {
  Entity *b{nullptr};
  for (list<unique_ptr<Entity>>::iterator itr = gameInstance->bullets.begin();
       itr != gameInstance->bullets.end();) {
    b = (*itr).get();
    b->x += b->dx;
    b->y += b->dy;
    if (bulletHitFighter(b) || b->x < -b->w || b->y < -b->h ||
        b->x > gameInstance->SCREEN_WIDTH ||
        b->y > gameInstance->SCREEN_HEIGHT) {
      itr = gameInstance->bullets.erase(itr);
    } else if (itr != gameInstance->bullets.end()) {
      itr++;
    }
  }
}

int GameLogic::bulletHitFighter(Entity *b) {
  Entity *e;
  for (list<unique_ptr<Entity>>::iterator itr = gameInstance->fighters.begin();
       itr != gameInstance->fighters.end(); itr++) {
    e = (*itr).get();
    if (e->side != b->side &&
        collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h)) {
      b->health = 0;
      e->health = 0;
      if (e != gameInstance->player) {
        gameInstance->score++;
        gameInstance->renderer.UpdateWindowTitle(gameInstance->score,
                                                 gameInstance->highScore);
      }
      return 1;
    }
  }
  return 0;
}

void GameLogic::spawnEnemies() {
  unique_ptr<Entity> enemy;

  if (--gameInstance->enemySpawnTimer <= 0) {
    enemy = make_unique<Entity>();

    enemy->texture = enemyTexture;
    SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);
    enemy->x = gameInstance->SCREEN_WIDTH;
    enemy->y = rand() % (gameInstance->SCREEN_HEIGHT - enemy->h);
    enemy->side = Entity::Type::SIDE_ALIEN;
    enemy->health = 1;
    enemy->dx = -(2 + (rand() % 4));
    enemy->reload = FPS; /* * (1 + (rand() % 3));*/

    gameInstance->fighters.push_back(std::move(enemy));

    gameInstance->enemySpawnTimer = 10 + (rand() % 30);
  }
}

void GameLogic::doEnemies() {
  Entity *e;

  for (list<unique_ptr<Entity>>::iterator itr = gameInstance->fighters.begin();
       itr != gameInstance->fighters.end(); itr++) {
    e = (*itr).get();
    if (e != gameInstance->player && gameInstance->player != nullptr &&
        --e->reload <= 0) {
      fireAlienBullet(e);
    }
  }
}

void GameLogic::fireAlienBullet(Entity *e) {
  unique_ptr<Entity> bullet;

  bullet = make_unique<Entity>();

  bullet->x = e->x;
  bullet->y = e->y;
  bullet->health = 1;
  bullet->texture = alienBulletTexture;
  bullet->side = e->side;
  SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

  bullet->x += (e->w / 2) - (bullet->w / 2);
  bullet->y += (e->h / 2) - (bullet->h / 2);

  calcSlope(gameInstance->player->x + (gameInstance->player->w / 2),
            gameInstance->player->y + (gameInstance->player->h / 2), e->x, e->y,
            bullet->dx, bullet->dy);

  bullet->dx *= ALIEN_BULLET_SPEED;
  bullet->dy *= ALIEN_BULLET_SPEED;

  bullet->side = Entity::Type::SIDE_ALIEN;

  gameInstance->bullets.push_back(std::move(bullet));

  e->reload = (rand() % FPS * 2);
}

void GameLogic::setGameState(GameState g) {
  if (gameInstance->controller.keyboard[SDL_SCANCODE_SPACE] &&
      gameState == GameState::stop) {
    gameState = GameState::start;
    gameInstance->background_text = nullptr;
    gameInstance->initPlayer();
  } else if (g == GameState::stop) {
    gameState = g;
    gameInstance->background_text = gameInstance->background_restart_text.get();
    gameInstance->resetStage();
  }
}