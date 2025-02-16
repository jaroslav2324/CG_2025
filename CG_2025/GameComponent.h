#pragma once

#include <string>

#include <wrl/client.h>

#include "ShaderManager.h"
#include "BufferManager.h"

class Game;

class GameComponent {
public:
	virtual ~GameComponent() = default;
	virtual int init(Game* game,
		std::shared_ptr<ShaderManager> shaderManager,
		std::shared_ptr<BufferManager> bufferManager,
		const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) = 0;
	virtual int update(float deltaTime) = 0;
	virtual void destroyResources() = 0;
	virtual int draw() = 0;

protected:
	Game* game = nullptr;
};