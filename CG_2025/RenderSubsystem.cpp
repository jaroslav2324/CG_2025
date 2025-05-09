#include "RenderSubsystem.h"
#include "global.h"
#include "Game.h"
#include "AABB.h"

RenderSubsystem::RenderSubsystem()
{
    gBuf.init();

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_RASTERIZER_DESC rastDesc;
    
    depthStencilDesc = {};
	depthStencilDesc.DepthEnable = false;                           
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;       
	depthStencilDesc.StencilEnable = false;                         

    auto device = GE::getGameSubsystem()->device;
	device->CreateDepthStencilState(&depthStencilDesc, &DSStateNoWriteNoCheck);

    depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;       
	depthStencilDesc.StencilEnable = false; 
    device->CreateDepthStencilState(&depthStencilDesc, &DSStateNoWriteLess);

    depthStencilDesc = {};
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER;
	depthStencilDesc.StencilEnable = false;
	device->CreateDepthStencilState(&depthStencilDesc, &DSStateNoWriteGreater);

    rastDesc = {};
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_BACK;
	device->CreateRasterizerState(&rastDesc, &rastStateCullBack);

    rastDesc = {};
	rastDesc.FillMode = D3D11_FILL_SOLID;
	rastDesc.CullMode = D3D11_CULL_FRONT;
	device->CreateRasterizerState(&rastDesc, &rastStateCullFront);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;       
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;          
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;   
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;

	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&blendDesc, &additiveBlendState);

    auto bufferManager = GE::getBufferManager();
    std::vector<Vector4> verts = { Vector4()};
    mockVertexBuffer = bufferManager->createVertexBuffer(verts);
    std::vector<int> idcs = { 0 };
    mockIndexBuffer = bufferManager->createIndexBuffer(idcs);

    D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	device->CreateSamplerState(&samplerDesc, &shadowSampler);
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
            compileCreateVertexShader(L"./shaders/deferredLightingVertex.hlsl", "VSMain", "vs_5_0", nullptr, &vertexByteCode);
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
    deviceContext->OMSetBlendState(nullptr, nullptr, 0xFFFFFFFF);

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
    auto device = GE::getGameSubsystem()->getDevice();
	auto deviceContext = GE::getGameSubsystem()->getDeviceContext();
	auto winHandler = GE::getWindowHandler();

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(winHandler->getWinWidth());
	viewport.Height = static_cast<float>(winHandler->getWinHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

    ID3D11RenderTargetView* rtv = GE::getGameSubsystem()->rtv;
    const float blackColor[4] = { 0.0, 0.0, 0.0, 0.0 };
    deviceContext->ClearRenderTargetView(rtv, blackColor);
    deviceContext->OMSetRenderTargets(1, &rtv, nullptr);
	deviceContext->RSSetViewports(1, &viewport);
    deviceContext->OMSetBlendState(additiveBlendState.Get(), nullptr, 0xFFFFFFFF);
    deviceContext->IASetInputLayout(layoutPointSpot.Get());

	if (deferredLightingVertexShader)
		deviceContext->VSSetShader(deferredLightingVertexShader.Get(), nullptr, 0);
	if (deferredLightingPixelShader)
		deviceContext->PSSetShader(deferredLightingPixelShader.Get(), nullptr, 0);
    gBuf.bindPixelShaderResourceViews(1);

	ID3D11SamplerState* rawShadowSampler = shadowSampler.Get();
    deviceContext->PSSetSamplers(1, 1, &rawShadowSampler);

    std::vector<Plane> frustumPlanes = getFrustumPlanes(getFrustumCornersWorldSpace(GE::getCameraViewMatrix(), GE::getProjectionMatrix()));
    AABB aabb;
    for (LightSource& ls : GE::getGameSubsystem()->lightSources) {

        if (ls.ls.sourceType == DIRECTIONAL_LIGHT) {
            ls.mapShadowMap();
			auto rawLightShadowMapDataBuffer = ls.getShadowMapDataBuffer().Get();
            deviceContext->PSSetConstantBuffers(2, 1, &rawLightShadowMapDataBuffer);

            ID3D11ShaderResourceView* shMapSrv[] = {
                ls.shMap.shaderResView.Get()
			};
            deviceContext->PSSetShaderResources(0, 1, shMapSrv);
        }

        ls.mapLightSourceDataConstBuffer();
        ls.addDeferredData.viewMatrix = GE::getCameraViewMatrix().Transpose();
        ls.addDeferredData.projectionMatrix = GE::getProjectionMatrix().Transpose();
        ls.addDeferredData.inverseViewMatrix = GE::getCameraViewMatrix().Invert().Transpose();
        ls.addDeferredData.inverseProjectionMatrix = GE::getProjectionMatrix().Invert().Transpose();
        ls.addDeferredData.camPos = Vector4(GE::getCameraPosition());
        ls.addDeferredData.camDir = Vector4(GE::getCameraForwardVector());
        ls.addDeferredData.flags = static_cast<int>(LightSourceAdditionalFlags::NONE);

        ID3D11Buffer* rawLightDataBuf = ls.getLightSourceDataConstBuffer().Get();
        deviceContext->VSSetConstantBuffers(1, 1, &rawLightDataBuf);
        deviceContext->PSSetConstantBuffers(1, 1, &rawLightDataBuf);
        if (ls.ls.sourceType == AMBIENT_LIGHT || ls.ls.sourceType == DIRECTIONAL_LIGHT) {
            ls.mapAdditionalConstBuffer();
            drawScreenAlignedQuad(ls);
        }
        else if (ls.ls.sourceType == POINT_LIGHT || ls.ls.sourceType == SPOT_LIGHT)
        {
            if (ls.ls.sourceType == POINT_LIGHT) aabb = getAABBForPointLight(ls.ls);
            if (ls.ls.sourceType == SPOT_LIGHT) aabb = getAABBForSpotLight(ls.ls);

            FrustumIntersection intersecRes = TestAABBFrustum(aabb, frustumPlanes);
            if (intersecRes == OUTSIDE_FRUSTUM) { 
                ls.addDeferredData.flags = static_cast<int>(LightSourceAdditionalFlags::FORCE_DRAW_SCREEN_ALIGNED_SQUAD);
                ls.mapAdditionalConstBuffer();
                deviceContext->OMSetDepthStencilState(DSStateNoWriteNoCheck.Get(), NULL);
                deviceContext->RSSetState(rastStateCullBack.Get());
                drawScreenAlignedQuad(ls);
            }
            else if (intersecRes == INTERSECTS_FAR_PLANE){ 
                ls.mapAdditionalConstBuffer();
                deviceContext->OMSetDepthStencilState(DSStateNoWriteLess.Get(), NULL);
                deviceContext->RSSetState(rastStateCullBack.Get());
                drawAABB(aabb, ls);
            }
            else {
                ls.mapAdditionalConstBuffer();
				deviceContext->OMSetDepthStencilState(DSStateNoWriteGreater.Get(), NULL);
				deviceContext->RSSetState(rastStateCullFront.Get());
				drawAABB(aabb, ls);
            }
        }
    }
}

void RenderSubsystem::drawScreenAlignedQuad(LightSource& lightSource)
{
    auto context = GE::getGameSubsystem()->getDeviceContext();

    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetVertexBuffers(0, 1, &mockVertexBuffer, mockStrides.data(), mockOffsets.data());
	context->IASetIndexBuffer(mockIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
    ID3D11Buffer* rawConstBuf = lightSource.getAdditionalConstBuffer().Get();
	context->VSSetConstantBuffers(0, 1, &rawConstBuf);
	context->PSSetConstantBuffers(0, 1, &rawConstBuf);

	context->Draw(4, 0);
}

void RenderSubsystem::drawAABB(const AABB& box, LightSource& lightSource)
{
    auto context = GE::getGameSubsystem()->getDeviceContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    ID3D11Buffer* rawVertexBuffer = lightSource.getVertexBuffer().Get();
    context->IASetVertexBuffers(0, 1, &rawVertexBuffer, lightSource.getStrides().data(), lightSource.getOffsets().data());
    context->IASetIndexBuffer(lightSource.getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
    ID3D11Buffer* rawConstBuf = lightSource.getAdditionalConstBuffer().Get();
    context->VSSetConstantBuffers(0, 1, &rawConstBuf);
    context->PSSetConstantBuffers(0, 1, &rawConstBuf);

	context->DrawIndexed(36, 0, 0); 
}