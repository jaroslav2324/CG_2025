#include "RenderSubsystem.h"
#include "global.h"

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


void RenderSubsystem::drawDeferred(float deltaTime)
{
    // Opaque pass
    // light pass
}
