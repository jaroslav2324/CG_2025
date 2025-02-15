#pragma once

#include <string>

#include "ShaderManager.h"

class Game;

class GameComponent {
public:
	virtual ~GameComponent() = default;
	virtual int init(Game* game, std::shared_ptr<ShaderManager>,
		const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) = 0;
	virtual int update() = 0;
	virtual void destroyResources() = 0;
	virtual int draw() = 0;

protected: 
	Game* game = nullptr;
	ShaderManager* shaderManager = nullptr;
};