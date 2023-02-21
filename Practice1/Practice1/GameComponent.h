#pragma once

#include "Export.h"

class Game;

class GameComponent
{
public:

	Game *game;

	GameComponent(Game* _game)
	{
		game = _game;
	}

	virtual void Initialize() = 0;
	virtual void Draw(float deltaTime) = 0;
	virtual void Update(float deltaTime) = 0;
	virtual void DestroyResuorces() = 0;

};

