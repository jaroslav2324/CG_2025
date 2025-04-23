#pragma once

#include <vector>
#include <chrono>
#include <cmath>
#include <random>

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include "util.h"
#include "addData.h"
#include "appExceptions.h"
#include "WindowHandler.h"
#include "ShaderManager.h"
#include "MeshComponent.h"
#include "RectComponent.h"
#include "BufferManager.h"
#include "CatamariBall.h"
#include "CatamariBox.h"
#include "LightSource.h"

class PlanetComponent;

class Game {
private:
	using Matrix = DirectX::SimpleMath::Matrix;

public:
	Game();
	void init();
	void run();
	int draw(float deltaTime);
	~Game();
	void destroyResources();
	float getTotalTime();
	ID3D11Device* getDevice();
	ComPtr<ID3D11DepthStencilView> getDepthView();
	ID3D11DeviceContext* getDeviceContext();
	const ComPtr<ID3D11SamplerState> getSamplerState();
private:
	void update(float deltaTime);
	void shadowPass();
	void setShadowViewProj(Matrix lightView, Matrix lightProj, const LightSourceData& lightData);

	void createPongScene();
	void updatePongScene(float deltaTime);
	void cretePlanetsScene();
	void updatePlanetsScene(float deltaTime);
	void createKatamariScene();
	void updateKatamariScene(float deltaTime);
	HWND hWnd;

	// TODO: ComPtr
	ID3D11Device* device = nullptr;
	ID3D11DeviceContext* context = nullptr;
	IDXGISwapChain* swapChain = nullptr;
	ID3D11Texture2D* backTex = nullptr;
	ID3D11RenderTargetView* rtv = nullptr;
	ComPtr<ID3D11Texture2D> depthBuffer;
	ComPtr<ID3D11DepthStencilView> depthView;
	ComPtr<ID3D11SamplerState> sampler;
	ComPtr<ID3D11SamplerState> shadowSampler;

	std::vector<GameComponent*> components;

	int createMeshComponent(std::vector<DirectX::XMFLOAT4>&& points,
		std::vector<UINT>&& strides,
		std::vector<UINT>&& offsets,
		std::vector<int>&& indices);
	int createRectComponent(DirectX::SimpleMath::Vector2 centerPoint, float width, float height);
	int createPlanetComponent(DirectX::SimpleMath::Vector3 position, float radius);
	CatamariBall* createCatamariBallComponent(DirectX::SimpleMath::Vector3 position, float radius, int stacks = 16, int slices = 16);
	CatamariBox* createCatamariBoxComponent(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size);
	PlanetComponent* createLightSourceComponent(DirectX::SimpleMath::Vector3 position, float radius);

	int leftPongScore = 0;
	int rightPongScore = 0;
	DirectX::SimpleMath::Vector2 coordsStartLeftRocket = { 100, 400 };
	DirectX::SimpleMath::Vector2 coordsStartRightRocket = { 700, 400 };
	float startBallVelocity = 250;
	bool reflectedFromLeftRocket = true;

	DirectX::SimpleMath::Vector2 prevMousePosition = { 0, 0 };

	DirectX::SimpleMath::Vector2 generateRandomBallDirection();

	float totalTime = 0.0f;

	int countLightSources = 0;
	std::vector<LightSource> lightSources;
	std::vector<LightSourceData> lightSourcesMainPass;
	ComPtr<ID3D11Buffer> lightSourcesBuffer = nullptr;
	std::vector<LightSourceShadowMapData> shadowMapsData;
	ComPtr<ID3D11Buffer> shadowMapsDataBuffer = nullptr;

	ShadowPassLightAddData shp_lightAddData;
	ComPtr<ID3D11Buffer> shp_lightAddDataBuffer = nullptr;

	friend class RenderSubsystem;
};