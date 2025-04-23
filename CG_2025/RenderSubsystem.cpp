#include "RenderSubsystem.h"
#include "global.h"
#include "Game.h"

RenderSubsystem::RenderSubsystem()
{
    gBuf.init();
}

const GBuffer& RenderSubsystem::getGBuffer() const
{
    return gBuf;
}

void RenderSubsystem::render(float deltaTime)
{
    switch (renderType)
    {
    case RenderType::FORWARD:
        break;
    case RenderType::DEFERRED:
        drawDeferred(deltaTime);
        break;
    default:
        break;
    }
}

void RenderSubsystem::setRenderType(RenderType type)
{
    if (renderType == type) {
        return;
    }

    renderType = type;
    switch (renderType)
    {
    case RenderType::FORWARD:
        break;
    case RenderType::DEFERRED:
        if (!deferredOpaqueVertexShader || !deferredOpaquePixelShader) {
            std::shared_ptr<ShaderManager> shaderManager = GE::getShaderManager();
      
            deferredOpaqueVertexShader = shaderManager->
                compileCreateVertexShader(L"./shaders/deferredOpaqueVertex.hlsl", "VSMain", "vs_5_0", nullptr);
            deferredOpaquePixelShader = shaderManager->
                compileCreatePixelShader(L"./shaders/deferredOpaquePixel.hlsl", "PSMain", "ps_5_0", nullptr);
        }
        break;
    default:
        break;
    }
}

void RenderSubsystem::bindDefaultShaders()
{
    auto deviceContext = GE::getGameSubsystem()->getDeviceContext();

	switch (renderType)
	{
	case RenderType::FORWARD:
		break;
	case RenderType::DEFERRED:
        if (deferredOpaqueVertexShader)
		    deviceContext->VSSetShader(deferredOpaqueVertexShader.Get(), nullptr, 0);
        if (deferredOpaquePixelShader)
		    deviceContext->PSSetShader(deferredOpaquePixelShader.Get(), nullptr, 0);
		break;
	default:
		break;
	}
}


void RenderSubsystem::drawDeferred(float deltaTime)
{
    drawDeferredOpaque(deltaTime);
    drawDeferredLighting(deltaTime);
}

void RenderSubsystem::drawDeferredOpaque(float deltaTime)
{
    auto deviceContext = GE::getGameSubsystem()->getDeviceContext();
	auto winHandler = GE::getWindowHandler();

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(winHandler->getWinWidth());
	viewport.Height = static_cast<float>(winHandler->getWinHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

    deviceContext->RSSetViewports(1, &viewport);

    gBuf.setGBufferRenderTargets();
    gBuf.clearRenderTargets();

	for (const auto gameComponent : GE::getGameSubsystem()->components) {
		gameComponent->draw(deltaTime);
	}
}

void RenderSubsystem::drawDeferredLighting(float deltaTime)
{
}
