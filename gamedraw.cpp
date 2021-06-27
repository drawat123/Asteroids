#include "gamedraw.h"
#include "game.h"

Game *gameInstance1 = Game::GetInstance();

GameDraw::GameDraw() : backgroundX{0} {
  background = IMG_LoadTexture(gameInstance1->renderer.getRenderer(),
                               "gfx\\background.jpg");
}

GameDraw::~GameDraw() { SDL_DestroyTexture(background); }

void GameDraw::draw() {
  drawBackground();

  drawPlayer();

  drawBullets();

  drawFighters();

  updateFPS();
}

void GameDraw::drawBackground() {
  SDL_Rect dest;
  int x;

  for (x = backgroundX; x < gameInstance1->renderer.getScreenWidth();
       x += gameInstance1->renderer.getScreenWidth()) {
    dest.x = x;
    dest.y = 0;
    dest.w = gameInstance1->renderer.getScreenWidth();
    dest.h = gameInstance1->renderer.getScreenHeight();

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

void GameDraw::updateFPS() {
  unique_ptr<Entity> f = gameInstance1->drawFont(
      gameInstance1->fps, gameInstance1->renderer.getScreenWidth() - 80, 10,
      gameInstance1->font);
  gameInstance1->renderer.Render(f.get());

  SDL_DestroyTexture(f->texture);
}