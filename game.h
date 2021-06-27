#pragma once
#include "SDL.h"
#include "controller.h"
#include "entity.h"
#include "renderer.h"
#include"defs.h"
#include <list>
#include <vector>
#include <algorithm>
#include <memory>
#include <thread>
#include <future>
#include <SDL_ttf.h>

using std::list;
using std::vector;
using std::max_element;
using std::unique_ptr;
using std::make_unique;
using std::thread;
using std::future;
using std::promise;

class Game {
public:
	enum class GameState {
		start, stop
	};
	Game(Renderer& r, Controller& c);
	~Game();
	void Run();
	GameState gameState = GameState::stop;

private:
	void initPlayer();
	void resetStage();
	std::string calcFrameRate();
	void logic();
	void doPlayer();
	void doFighters();
	void setGameState(GameState g = GameState::start);
	int playerHitFighter(Entity* b);
	void doBullets();
	int bulletHitFighter(Entity* b);
	void doEnemies();
	void fireAlienBullet(Entity* e);
	void spawnEnemies();
	void fireBullet();
	void draw();
	void drawBackground();
	void drawPlayer();
	void drawBullets();
	void drawFighters();
	void updateFPS(std::string textureText);
	Entity* player;
	std::string fps;
	list<unique_ptr<Entity>> bullets;
	list<unique_ptr<Entity>> fighters;
	SDL_Texture* bulletTexture;
	SDL_Texture* enemyTexture;
	SDL_Texture* playerTexture;
	SDL_Texture* alienBulletTexture;
	SDL_Texture* background;
	SDL_Texture* background_play;
	SDL_Texture* background_stop;
	SDL_Texture* background_restart;
	Renderer& renderer;
	Controller& controller;
	bool running;
	int enemySpawnTimer = 0;
	int stageResetTimer = 0;
	int score = 0;
	int highScore = 0;
	int pHeight = 0;
	int backgroundX = 0;

	Uint32 title_timestamp = SDL_GetTicks();
	Uint32 frame_end;

	int frame_count;

	int frame_count = 0;


	std::mutex m;
	TTF_Font* gFont = NULL;
};
