#pragma once

#include <memory>

#include "GBuffer.h"

class Game;
struct AABB;
class LightSource;
class FireParticleSystem;

enum class RenderType {
	FORWARD = 0,
	DEFERRED = 1
};

class RenderSubsystem {
public:
	RenderSubsystem();
	const GBuffer& getGBuffer() const;
	void render(float deltaTime);
	void setRenderType(RenderType type);
	void bindDefaultShaders();
private:
	void drawDeferred(float deltaTime);
	void drawDeferredOpaque(float deltaTime);
	void drawDeferredLighting(float deltaTime);
	void drawScreenAlignedQuad(LightSource& lightSource);
	void drawAABB(const AABB& box, LightSource& lightSource);
	void drawParticles(float deltaTime);

	RenderType renderType = RenderType::FORWARD;
	GBuffer gBuf;

	ComPtr<ID3D11InputLayout> layoutPointSpot = nullptr;
	ComPtr<ID3D11VertexShader> deferredOpaqueVertexShader = nullptr;
	ComPtr<ID3D11PixelShader> deferredOpaquePixelShader = nullptr;
	ComPtr<ID3D11VertexShader> deferredLightingVertexShader = nullptr;
	ComPtr<ID3D11PixelShader> deferredLightingPixelShader = nullptr;

	ComPtr<ID3D11DepthStencilState> DSStateNoWriteNoCheck = nullptr;
	ComPtr<ID3D11DepthStencilState> DSStateNoWriteGreater = nullptr;
	ComPtr<ID3D11DepthStencilState> DSStateNoWriteLess = nullptr;
	ComPtr<ID3D11DepthStencilState> DSStateBasic = nullptr;

	ComPtr<ID3D11RasterizerState> rastStateNoCull = nullptr;
	ComPtr<ID3D11RasterizerState> rastStateCullFront = nullptr;
	ComPtr<ID3D11RasterizerState> rastStateCullBack = nullptr;

	ComPtr<ID3D11BlendState> additiveBlendState = nullptr;

	ComPtr<ID3D11Buffer> mockVertexBuffer = nullptr;
	ComPtr<ID3D11Buffer> mockIndexBuffer = nullptr;
	std::vector<UINT> mockStrides = { 16 };
	std::vector<UINT> mockOffsets = { 0 };

	ComPtr<ID3D11SamplerState> shadowSampler;

	std::shared_ptr<FireParticleSystem> fireParticleSystem;
};