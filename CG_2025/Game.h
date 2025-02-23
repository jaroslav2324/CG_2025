#pragma once

#include <vector>
#include <chrono>
#include <cmath>

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include "util.h"
#include "appExceptions.h"
#include "WindowHandler.h"
#include "ShaderManager.h"
#include "MeshComponent.h"
#include "RectComponent.h"
#include "BufferManager.h"

class Game {

public:
	Game();
	void init();
	void createPongScene();
	void run();
	int draw();
	~Game();
	void destroyResources();
	// TODO : create renderer and move there
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
private:
	void update(float deltaTime);

	HWND hWnd;

	// TODO: ComPtr
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Texture2D* backTex = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;

	// TODO: sahred ptr
	std::vector<GameComponent*> components;

	// TODO: remove
	std::shared_ptr<WindowHandler> winHandler = nullptr;

	int createMeshComponent(std::vector<DirectX::XMFLOAT4>&& points,
		std::vector<UINT>&& strides,
		std::vector<UINT>&& offsets,
		std::vector<int>&& indices);
	int createRectComponent(DirectX::SimpleMath::Vector2 centerPoint, float width, float height);
};