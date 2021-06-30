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

  doExplosions();

  doDebris();

  if (gameInstance->player == nullptr && --gameInstance->stageResetTimer <= 0) {
    setGameState(GameState::stop);
    gameInstance->resetStage();
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
      gameSound.playSound(SND_PLAYER_FIRE, CH_PLAYER);

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
    addDebris(e);
    addExplosions(e->x, e->w, 10);
    gameSound.playSound(SND_ALIEN_DIE, CH_ANY);
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
      if (e == gameInstance->player) {
        gameSound.playSound(SND_PLAYER_DIE, CH_PLAYER);
      } else if (e != gameInstance->player) {
        gameInstance->score++;
        gameInstance->renderer.UpdateWindowTitle(gameInstance->score,
                                                 gameInstance->highScore);
        gameSound.playSound(SND_ALIEN_DIE, CH_ANY);
      }
      addDebris(e);
      addExplosions(e->x, e->w, 10);
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

      gameSound.playSound(SND_ALIEN_FIRE, CH_ALIEN_FIRE);
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

void GameLogic::addExplosions(int x, int y, int num) {
  unique_ptr<Explosion> e;
  int i;

  for (i = 0; i < num; i++) {
    e = make_unique<Explosion>();

    e->x = x + (rand() % 32) - (rand() % 32);
    e->y = y + (rand() % 32) - (rand() % 32);
    e->dx = (rand() % 10) - (rand() % 10);
    e->dy = (rand() % 10) - (rand() % 10);

    e->dx /= 10;
    e->dy /= 10;

    switch (rand() % 4) {
    case 0:
      e->r = 255;
      break;

    case 1:
      e->r = 255;
      e->g = 128;
      break;

    case 2:
      e->r = 255;
      e->g = 255;
      break;

    default:
      e->r = 255;
      e->g = 255;
      e->b = 255;
      break;
    }

    e->a = rand() % FPS * 3;

    gameInstance->explosion.push_back(std::move(e));
  }
}

void GameLogic::doExplosions() {
  Explosion *e;

  for (list<unique_ptr<Explosion>>::iterator itr =
           gameInstance->explosion.begin();
       itr != gameInstance->explosion.end();) {
    e = (*itr).get();
    e->x += e->dx;
    e->y += e->dy;
    if (--e->a <= 0) {
      itr = gameInstance->explosion.erase(itr);
    } else if (itr != gameInstance->explosion.end()) {
      itr++;
    }
  }
}

void GameLogic::addDebris(Entity *e) {
  unique_ptr<Debris> d;
  int x, y, w, h;

  w = e->w / 2;
  h = e->h / 2;

  for (y = 0; y <= h; y += h) {
    for (x = 0; x <= w; x += w) {
      d = make_unique<Debris>();

      d->x = e->x + e->w / 2;
      d->y = e->y + e->h / 2;
      d->dx = (rand() % 5) - (rand() % 5);
      d->dy = -(5 + (rand() % 12));
      d->life = FPS * 2;
      d->texture = e->texture;

      d->rect.x = x;
      d->rect.y = y;
      d->rect.w = w;
      d->rect.h = h;

      gameInstance->debris.push_back(std::move(d));
    }
  }
}

void GameLogic::doDebris() {
  Debris *d;

  for (list<unique_ptr<Debris>>::iterator itr = gameInstance->debris.begin();
       itr != gameInstance->debris.end();) {
    d = (*itr).get();
    d->x += d->dx;
    d->y += d->dy;
    d->dy += 0.5;
    if (--d->life <= 0) {
      itr = gameInstance->debris.erase(itr);
    } else if (itr != gameInstance->debris.end()) {
      itr++;
    }
  }
}

void GameLogic::setGameState(GameState g) {
  if (gameInstance->controller.keyboard[SDL_SCANCODE_SPACE] &&
      gameState == GameState::stop) {
    gameState = GameState::start;
    gameInstance->background_text = nullptr;
    gameInstance->initPlayer();
    gameSound.playSound(SND_PLAYER_FIRE, CH_PLAYER);
  } else if (g == GameState::stop) {
    gameState = g;
    gameInstance->background_text = gameInstance->background_restart_text.get();
    gameInstance->resetStage();
  }
}