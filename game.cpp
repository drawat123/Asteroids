#include "game.h"
#include <iostream>

Game::Game(Renderer& r, Controller& c) :renderer{ r }, controller{ c }, running{ true }, fps{ "" }, player{ nullptr }, frame_count{ 0 }
{
	bulletTexture = IMG_LoadTexture(renderer.getRenderer(), "gfx\\playerBullet.png");
	enemyTexture = IMG_LoadTexture(renderer.getRenderer(), "gfx\\enemy.png");
	alienBulletTexture = IMG_LoadTexture(renderer.getRenderer(), "gfx\\alienBullet.png");
	playerTexture = IMG_LoadTexture(renderer.getRenderer(), "gfx\\player.png");
	background_stop = IMG_LoadTexture(renderer.getRenderer(), "gfx\\background.jpg");
	background_play = IMG_LoadTexture(renderer.getRenderer(), "gfx\\background-play.jpg");
	background_restart = IMG_LoadTexture(renderer.getRenderer(), "gfx\\background-restart.jpg");
	gFont = TTF_OpenFont("font\\font.ttf", 24);
	background = background_stop;

	enemySpawnTimer = 0;

	resetStage();
}

Game::~Game()
{
	SDL_DestroyTexture(bulletTexture);
	SDL_DestroyTexture(enemyTexture);
	SDL_DestroyTexture(alienBulletTexture);
	SDL_DestroyTexture(playerTexture);
	SDL_DestroyTexture(background_stop);
	SDL_DestroyTexture(background_play);
	SDL_DestroyTexture(background_restart);
	TTF_CloseFont(gFont);
}

std::string Game::calcFrameRate()
{
	std::lock_guard<std::mutex> lk(m);
	if (frame_end - title_timestamp >= 1000) {
		int f = frame_count;
		frame_count = 0;
		title_timestamp = frame_end;
		return { "FPS: " + std::to_string(f) };
	}
	return fps;
}

void Game::Run()
{
	long then = SDL_GetTicks();
	float remainder = 0;

	constexpr std::size_t target_frame_duration{ 1000 / 60 };
	Uint32 frame_start;
	Uint32 frame_duration;

	while (running) {
		frame_start = SDL_GetTicks();

		renderer.prepareScene();
		controller.HandleInput(running);
		setGameState();
		if (gameState == GameState::start) {
			logic();
		}
		draw();

		frame_end = SDL_GetTicks();

		frame_count++;
		frame_duration = frame_end - frame_start;

		future<std::string> ftr = std::async(std::launch::async, &Game::calcFrameRate, this);
		fps = ftr.get();

		renderer.presentScene();

		if (frame_duration < target_frame_duration) {
			SDL_Delay(target_frame_duration - frame_duration);
		}
	}
}

void Game::setGameState(GameState g) {
	if (controller.keyboard[SDL_SCANCODE_SPACE] && gameState == GameState::stop)
	{
		gameState = GameState::start;
		background = background_play;
		initPlayer();
	}
	else if (g == GameState::stop)
	{
		gameState = g;
		background = background_restart;
		resetStage();
	}
}

void Game::initPlayer() {
	unique_ptr<Entity> p = make_unique<Entity>();
	player = p.get();

	p->x = 100;
	p->y = 100;
	p->texture = playerTexture;
	p->side = Entity::Type::SIDE_PLAYER;
	SDL_QueryTexture(p->texture, NULL, NULL, &p->w, &p->h);
	pHeight = p->h;
	fighters.push_back(std::move(p));
}

void Game::resetStage() {
	fighters.clear();

	bullets.clear();

	enemySpawnTimer = 0;

	stageResetTimer = FPS * 2;

	highScore = max(score, highScore);
	score = 0;
	renderer.UpdateWindowTitle(score, highScore);
}

void Game::logic()
{
	doPlayer();

	doFighters();

	doBullets();

	spawnEnemies();

	doEnemies();

	if (player == nullptr && --stageResetTimer <= 0)
	{
		setGameState(GameState::stop);
	}
}

void Game::doPlayer()
{
	if (player != nullptr)
	{
		player->dx = player->dy = 0;

		if (player->reload > 0)
		{
			player->reload--;
		}

		if (controller.keyboard[SDL_SCANCODE_UP] && (player->y - 4) > 0)
		{
			player->dy = -PLAYER_SPEED;
		}

		if (controller.keyboard[SDL_SCANCODE_DOWN] && (player->y + 4) < renderer.getScreenHeight() - player->h)
		{
			player->dy = PLAYER_SPEED;
		}

		if (controller.keyboard[SDL_SCANCODE_LEFT] && (player->x - 4) > 0)
		{
			player->dx = -PLAYER_SPEED;
		}

		if (controller.keyboard[SDL_SCANCODE_RIGHT] && (player->x + 4) < renderer.getScreenWidth() - player->w)
		{
			player->dx = PLAYER_SPEED;
		}

		if (controller.keyboard[SDL_SCANCODE_LCTRL] && player->reload == 0)
		{
			fireBullet();
		}

		player->x += player->dx;
		player->y += player->dy;
	}
}

void Game::doFighters() {
	Entity* e;
	for (list <unique_ptr<Entity>> ::iterator itr = fighters.begin(); itr != fighters.end();)
	{
		e = (*itr).get();
		e->x += e->dx;
		e->y += e->dy;
		if (e != player && (e->x < -e->w || playerHitFighter(e)))
		{
			e->health = 0;
		}
		if (e->health == 0) {
			if (e == player)
			{
				player = nullptr;
			}
			itr = fighters.erase(itr);
		}
		else if (itr != fighters.end()) {
			itr++;
		}
	}
}

int Game::playerHitFighter(Entity* e)
{
	if (player != nullptr && player->side != e->side && collision(e->x, e->y, e->w, e->h, player->x, player->y, player->w, player->h))
	{
		e->health = 0;
		return 1;
	}

	return 0;
}

void Game::doBullets()
{
	Entity* b{ nullptr };
	for (list <unique_ptr<Entity>> ::iterator itr = bullets.begin(); itr != bullets.end(); )
	{
		b = (*itr).get();
		b->x += b->dx;
		b->y += b->dy;
		if (bulletHitFighter(b) || b->x < -b->w || b->y < -b->h || b->x > renderer.getScreenWidth() || b->y > renderer.getScreenHeight())
		{
			itr = bullets.erase(itr);
		}
		else if (itr != bullets.end()) {
			itr++;
		}
	}
}

int Game::bulletHitFighter(Entity* b)
{
	Entity* e;
	for (list <unique_ptr<Entity>> ::iterator itr = fighters.begin(); itr != fighters.end(); itr++)
	{
		e = (*itr).get();
		if (e->side != b->side && collision(b->x, b->y, b->w, b->h, e->x, e->y, e->w, e->h))
		{
			b->health = 0;
			e->health = 0;
			if (e != player) {
				score++;
				renderer.UpdateWindowTitle(score, highScore);
			}
			return 1;
		}
	}
	return 0;
}

void Game::doEnemies()
{
	Entity* e;

	for (list <unique_ptr<Entity>> ::iterator itr = fighters.begin(); itr != fighters.end(); itr++)
	{
		e = (*itr).get();
		if (e != player && player != nullptr && --e->reload <= 0)
		{
			fireAlienBullet(e);
		}
	}
}

void Game::fireAlienBullet(Entity* e)
{
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

	calcSlope(player->x + (player->w / 2), player->y + (player->h / 2), e->x, e->y, bullet->dx, bullet->dy);

	bullet->dx *= ALIEN_BULLET_SPEED;
	bullet->dy *= ALIEN_BULLET_SPEED;

	bullet->side = Entity::Type::SIDE_ALIEN;

	bullets.push_back(std::move(bullet));

	e->reload = (rand() % FPS * 2);
}

void Game::spawnEnemies() {
	unique_ptr<Entity> enemy;

	if (--enemySpawnTimer <= 0)
	{
		enemy = make_unique<Entity>();

		enemy->x = renderer.getScreenWidth();
		enemy->y = rand() % (renderer.getScreenHeight() - pHeight);
		enemy->texture = enemyTexture;
		enemy->side = Entity::Type::SIDE_ALIEN;
		enemy->health = 1;
		SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

		enemy->dx = -(2 + (rand() % 4));
		enemy->reload = FPS * (1 + (rand() % 3));

		fighters.push_back(std::move(enemy));

		enemySpawnTimer = 30 + (rand() % 60);
	}
}

void Game::fireBullet()
{
	unique_ptr<Entity> bullet;

	bullet = make_unique<Entity>();

	bullet->x = player->x;
	bullet->y = player->y;
	bullet->dx = PLAYER_BULLET_SPEED;
	bullet->health = 1;
	bullet->side = Entity::Type::SIDE_PLAYER;
	bullet->texture = bulletTexture;
	SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

	bullet->y += (player->h / 2) - (bullet->h / 2);

	bullets.push_back(std::move(bullet));

	player->reload = 8;
}

void Game::draw()
{
	drawBackground();

	drawPlayer();

	drawBullets();

	drawFighters();

	updateFPS(fps);
}

void Game::drawBackground() {
	SDL_Rect dest;
	int x;

	for (x = backgroundX; x < renderer.getScreenWidth(); x += renderer.getScreenWidth())
	{
		dest.x = x;
		dest.y = 0;
		dest.w = renderer.getScreenWidth();
		dest.h = renderer.getScreenHeight();

		SDL_RenderCopy(renderer.getRenderer(), background, NULL, &dest);
	}
}

void Game::drawPlayer()
{
	if (player != nullptr) {
		renderer.Render(player);
	}
}

void Game::drawBullets()
{
	for (list <unique_ptr<Entity>> ::iterator itr = bullets.begin(); itr != bullets.end(); itr++)
	{
		renderer.Render((*itr).get());
	}
}

void Game::drawFighters()
{
	for (list <unique_ptr<Entity>> ::iterator itr = fighters.begin(); itr != fighters.end(); itr++)
	{
		if ((*itr).get() != player) {
			renderer.Render((*itr).get());
		}
	}
}

void Game::updateFPS(std::string textureText)
{
	SDL_Color textColor = { 255, 255, 255 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);

	SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer.getRenderer(), textSurface);

	Entity fps;
	fps.x = renderer.getScreenWidth() - 80;
	fps.y = 10;
	fps.texture = fontTexture;

	renderer.Render(&fps);

	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(fontTexture);
}