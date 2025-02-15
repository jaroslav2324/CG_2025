#pragma once

class Game;

class GameComponent {
public:
	virtual ~GameComponent() = default;
	virtual int init(Game* game) = 0;
	virtual int update() = 0;
	virtual void destroyResources() = 0;
	virtual int draw() = 0;

protected: 
	Game* game = nullptr;
};