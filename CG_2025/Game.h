#pragma once

#include <vector>
#include <chrono>

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include "util.h"
#include "appExceptions.h"
#include "WindowHandler.h"
#include "ShaderManager.h"
#include "MeshComponent.h"
#include "BufferManager.h"

class Game {

public:
	void init();
	void run();
	int draw();
	int createTriangleComponent(std::vector<DirectX::XMFLOAT4>&& points,
		std::vector<UINT>&& strides,
		std::vector<UINT>&& offsets,
		std::vector<int>&& indices);
	~Game();
	void destroyResources();
	ID3D11Device* getDevice();
	ID3D11DeviceContext* getDeviceContext();
private:
	WindowHandler winHandler;
	std::shared_ptr<ShaderManager> shaderManager = nullptr;
	std::shared_ptr<BufferManager> bufferManger = nullptr;
	HWND hWnd;

	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Texture2D* backTex = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;

	std::vector<GameComponent*> components;

	const int screenWidth = 800;
	const int screenHeight = 800;
};