#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
class Entity
{
public:
	Entity();
	Entity(const Entity& source);
	Entity& operator=(const Entity& source);
	Entity(Entity&& source);
	Entity& operator=(Entity&& source);
	~Entity();

	enum class Type {
		SIDE_PLAYER, SIDE_ALIEN
	};
	int x;
	int y;
	int w;
	int h;
	float dx;
	float dy;
	int reload;
	int health;
	Type side;
	SDL_Texture* texture;
};

