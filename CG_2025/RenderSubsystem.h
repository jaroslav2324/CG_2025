#pragma once

#include "GBuffer.h"

class Game;

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

	RenderType renderType = RenderType::FORWARD;
	GBuffer gBuf;

	ComPtr<ID3D11VertexShader> deferredOpaqueVertexShader = nullptr;
	ComPtr<ID3D11PixelShader> deferredOpaquePixelShader = nullptr;
};