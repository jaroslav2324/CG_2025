#include "RenderSubsystem.h"
#include "global.h"
#include "Game.h"
#include "AABB.h"

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
    {
        std::shared_ptr<ShaderManager> shaderManager = GE::getShaderManager();

        deferredOpaqueVertexShader = shaderManager->
            compileCreateVertexShader(L"./shaders/deferredOpaqueVertex.hlsl", "VSMain", "vs_5_0", nullptr);
        deferredOpaquePixelShader = shaderManager->
            compileCreatePixelShader(L"./shaders/deferredOpaquePixel.hlsl", "PSMain", "ps_5_0", nullptr);

        ComPtr<ID3DBlob> vertexByteCode;
        deferredLightingVertexShader = shaderManager->
            compileCreateVertexShader(L"./shaders/deferredLightingVertex.hlsl", "VSMain", "vs_5_0", nullptr, vertexByteCode);
        deferredLightingPixelShader = shaderManager->
            compileCreatePixelShader(L"./shaders/deferredLightingPixel.hlsl", "PSMain", "ps_5_0", nullptr);
        layoutPointSpot = GE::getBufferManager()->createInputLayout_PosF4(vertexByteCode);
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

	if (deferredOpaqueVertexShader)
		deviceContext->VSSetShader(deferredOpaqueVertexShader.Get(), nullptr, 0);
	if (deferredOpaquePixelShader)
		deviceContext->PSSetShader(deferredOpaquePixelShader.Get(), nullptr, 0);

	for (const auto gameComponent : GE::getGameSubsystem()->components) {
		gameComponent->draw(deltaTime);
	}
}

void RenderSubsystem::drawDeferredLighting(float deltaTime)
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

	if (deferredLightingVertexShader)
		deviceContext->VSSetShader(deferredLightingVertexShader.Get(), nullptr, 0);
	if (deferredLightingPixelShader)
		deviceContext->PSSetShader(deferredLightingPixelShader.Get(), nullptr, 0);
    for (LightSource& ls : GE::getGameSubsystem()->lightSources) {
        switch (ls.ls.sourceType)
        {
        case AMBIENT_LIGHT:
        case DIRECTIONAL_LIGHT:
            drawScreenAlignedQuad();
            break;

        case POINT_LIGHT:
        {
            AABB aabb = getAABBForPointLight(ls.ls);
            drawAABB(aabb, ls);
            break;
        }
        case SPOT_LIGHT:
        {
            AABB aabb = getAABBForSpotLight(ls.ls);
            drawAABB(aabb, ls);
            break;
        }
        default:
            break;
        }
    }
}

void RenderSubsystem::drawScreenAlignedQuad()
{
    auto context = GE::getGameSubsystem()->getDeviceContext();
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->Draw(4, 0);
}

void RenderSubsystem::drawAABB(const AABB& box, LightSource& lightSource)
{
    lightSource.addData.viewMatrix = GE::getCameraViewMatrix().Transpose();
    lightSource.addData.projectionMatrix = GE::getProjectionMatrix().Transpose();
    lightSource.mapAdditionalBuffer();

    auto context = GE::getGameSubsystem()->getDeviceContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetVertexBuffers(0, 1, &lightSource.getVertexBuffer(), lightSource.getStrides().data(), lightSource.getOffsets().data());
    context->IASetIndexBuffer(lightSource.getIndexBuffer().Get(), DXGI_FORMAT_R32G32B32A32_UINT, sizeof(int));
    context->VSSetConstantBuffers(0, 1, &lightSource.getAdditionalBuffer());

	context->DrawIndexed(36, 0, 0); 
}