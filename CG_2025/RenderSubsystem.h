#pragma once

#include "GBuffer.h"

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
private:
	void drawDeferred(float deltaTime);

	RenderType renderType = RenderType::FORWARD;
	GBuffer gBuf;

	ComPtr<ID3D11VertexShader> deferredOpaqueVertexShader = nullptr;
	ComPtr<ID3D11PixelShader> deferredOpaquePixelShader = nullptr;
};