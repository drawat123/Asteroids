#include "gamedraw.h"
#include "game.h"

Game *gameInstance1 = Game::GetInstance();

GameDraw::GameDraw() : backgroundX{0} {
  background = IMG_LoadTexture(gameInstance1->renderer.getRenderer(),
                               "gfx\\background.jpg");
  explosionTexture = IMG_LoadTexture(gameInstance1->renderer.getRenderer(),
                                     "gfx\\explosion.png");
}

GameDraw::~GameDraw() { SDL_DestroyTexture(background); }

void GameDraw::draw() {
  drawBackground();

  drawPlayer();

  drawBullets();

  drawFighters();

  drawDebris();

  drawExplosions();

  updateFPS();
}

void GameDraw::drawBackground() {
  SDL_Rect dest;
  int x;

  for (x = backgroundX; x < gameInstance1->SCREEN_WIDTH;
       x += gameInstance1->SCREEN_WIDTH) {
    dest.x = x;
    dest.y = 0;
    dest.w = gameInstance1->SCREEN_WIDTH;
    dest.h = gameInstance1->SCREEN_HEIGHT;

    SDL_RenderCopy(gameInstance1->renderer.getRenderer(), background, NULL,
                   &dest);
  }

  gameInstance1->renderer.Render(gameInstance1->background_text);
}

void GameDraw::drawPlayer() {
  if (gameInstance1->player != nullptr) {
    gameInstance1->renderer.Render(gameInstance1->player);
  }
}

void GameDraw::drawBullets() {
  for (list<unique_ptr<Entity>>::iterator itr = gameInstance1->bullets.begin();
       itr != gameInstance1->bullets.end(); itr++) {
    gameInstance1->renderer.Render((*itr).get());
  }
}

void GameDraw::drawFighters() {
  for (list<unique_ptr<Entity>>::iterator itr = gameInstance1->fighters.begin();
       itr != gameInstance1->fighters.end(); itr++) {
    if ((*itr).get() != gameInstance1->player) {
      gameInstance1->renderer.Render((*itr).get());
    }
  }
}

void GameDraw::drawDebris() {
  Debris *d;
  for (list<unique_ptr<Debris>>::iterator itr = gameInstance1->debris.begin();
       itr != gameInstance1->debris.end(); itr++) {
    d = (*itr).get();
    gameInstance1->renderer.blit(d->texture, &d->rect, d->x, d->y);
  }
}

void GameDraw::drawExplosions() {
  Explosion *e;

  SDL_SetRenderDrawBlendMode(gameInstance1->renderer.getRenderer(),
                             SDL_BLENDMODE_ADD);
  SDL_SetTextureBlendMode(explosionTexture, SDL_BLENDMODE_ADD);

  for (list<unique_ptr<Explosion>>::iterator itr =
           gameInstance1->explosion.begin();
       itr != gameInstance1->explosion.end(); itr++) {
    e = (*itr).get();
    SDL_SetTextureColorMod(explosionTexture, e->r, e->g, e->b);
    SDL_SetTextureAlphaMod(explosionTexture, e->a);

    gameInstance1->renderer.blit(explosionTexture, e->x, e->y);
  }

  SDL_SetRenderDrawBlendMode(gameInstance1->renderer.getRenderer(),
                             SDL_BLENDMODE_NONE);
}

void GameDraw::updateFPS() {
  unique_ptr<Entity> f = gameInstance1->drawFont(
      gameInstance1->fps, gameInstance1->SCREEN_WIDTH - 80, 10,
      gameInstance1->font);
  gameInstance1->renderer.Render(f.get());

  SDL_DestroyTexture(f->texture);
}