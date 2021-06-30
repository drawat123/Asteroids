#pragma once
#include "defs.h"
#include <SDL_mixer.h>
#include <vector>

using std::vector;

class GameSound {
public:
  GameSound();
  void initSounds();
  void loadSounds();
  void loadMusic(const char *filename);
  void playMusic(int loop);
  void playSound(int id, int channel);
  vector<Mix_Chunk *> sounds;
  Mix_Music *music;
};
