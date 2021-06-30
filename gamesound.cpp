#include "gameSound.h"

GameSound::GameSound() : sounds(4, nullptr) {
  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
    printf("Couldn't initialize SDL Mixer\n");
    exit(1);
  }
  Mix_AllocateChannels(MAX_SND_CHANNELS);

  initSounds();
}

void GameSound::initSounds() {
  music = nullptr;
  loadSounds();
}

void GameSound::loadSounds() {
  sounds[SND_PLAYER_FIRE] =
      Mix_LoadWAV("sound\\334227__jradcoolness__laser.ogg");
  sounds[SND_ALIEN_FIRE] =
      Mix_LoadWAV("sound\\196914__dpoggioli__laser-gun.ogg");
  sounds[SND_PLAYER_DIE] =
      Mix_LoadWAV("sound\\245372__quaker540__hq-explosion.ogg");
  sounds[SND_ALIEN_DIE] =
      Mix_LoadWAV("sound\\10 Guage Shotgun-SoundBible.com-74120584.ogg");
  loadMusic("sound\\Mercury.ogg");
}

void GameSound::loadMusic(const char *filename) {
  if (music != NULL) {
    Mix_HaltMusic();
    Mix_FreeMusic(music);
    music = NULL;
  }
  music = Mix_LoadMUS(filename);
  playMusic(-1);
}

void GameSound::playMusic(int loop) { Mix_PlayMusic(music, (loop) ? -1 : 0); }

void GameSound::playSound(int id, int channel) {
  Mix_PlayChannel(channel, sounds[id], 0);
}