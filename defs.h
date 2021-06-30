#pragma once
#include <algorithm>
using std::max;
using std::min;

constexpr auto PLAYER_SPEED = 4;
constexpr auto PLAYER_BULLET_SPEED = 16;
constexpr auto MAX_KEYBOARD_KEYS = 350;
constexpr auto FPS = 60;
constexpr auto ALIEN_BULLET_SPEED = 8;
constexpr auto MAX_SND_CHANNELS = 8;
enum class GameState { start, stop };
enum { CH_ANY = -1, CH_PLAYER, CH_ALIEN_FIRE };
enum {
  SND_PLAYER_FIRE,
  SND_ALIEN_FIRE,
  SND_PLAYER_DIE,
  SND_ALIEN_DIE,
  SND_MAX
};

static int collision(int x1, int y1, int w1, int h1, int x2, int y2, int w2,
                     int h2) {
  return (max(x1, x2) < min(x1 + w1, x2 + w2)) &&
         (max(y1, y2) < min(y1 + h1, y2 + h2));
}

static void calcSlope(int x1, int y1, int x2, int y2, float &dx, float &dy) {
  int steps = max(abs(x1 - x2), abs(y1 - y2));

  if (steps == 0) {
    dx = dy = 0;
    return;
  }

  dx = (x1 - x2);
  dx /= steps;

  dy = (y1 - y2);
  dy /= steps;
}