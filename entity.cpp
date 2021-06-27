#include "entity.h"

Entity::Entity()
    : x{0}, y{0}, w{0}, h{0}, dx{0}, dy{0}, reload{0}, health{1},
      side{Type::SIDE_PLAYER}, texture{nullptr} {}

Entity::Entity(const Entity &source) {
  x = source.x;
  y = source.y;
  w = source.w;
  h = source.h;
  dx = source.dx;
  dy = source.dy;
  reload = source.reload;
  health = source.health;
  side = source.side;
  texture = source.texture;
}

Entity &Entity::operator=(const Entity &source) {
  if (this == &source) {
    return *this;
  }
  SDL_DestroyTexture(texture);

  x = source.x;
  y = source.y;
  w = source.w;
  h = source.h;
  dx = source.dx;
  dy = source.dy;
  reload = source.reload;
  health = source.health;
  side = source.side;
  texture = source.texture;

  return *this;
}

Entity::Entity(Entity &&source) {
  x = source.x;
  y = source.y;
  w = source.w;
  h = source.h;
  dx = source.dx;
  dy = source.dy;
  reload = source.reload;
  health = source.health;
  side = source.side;
  texture = source.texture;

  source.x = 0;
  source.y = 0;
  source.w = 0;
  source.h = 0;
  source.dx = 0;
  source.dy = 0;
  source.reload = 0;
  source.health = 0;
  source.side = Type::SIDE_PLAYER;
  source.texture = nullptr;
}

Entity &Entity::operator=(Entity &&source) {
  if (this == &source) {
    return *this;
  }
  SDL_DestroyTexture(texture);

  x = source.x;
  y = source.y;
  w = source.w;
  h = source.h;
  dx = source.dx;
  dy = source.dy;
  reload = source.reload;
  health = source.health;
  side = source.side;
  texture = source.texture;

  source.x = 0;
  source.y = 0;
  source.w = 0;
  source.h = 0;
  source.dx = 0;
  source.dy = 0;
  source.reload = 0;
  source.health = 0;
  source.side = Type::SIDE_PLAYER;
  source.texture = nullptr;

  return *this;
}

Entity::~Entity() { texture = nullptr; }