#include "global.h"
#include "GBuffer.h"

void GBuffer::init()
{
	auto device = GE::getGameSubsystem()->getDevice();

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = 800;
	depthTexDesc.Height = 800;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.SampleDesc = { 1, 0 };

	auto res = device->CreateTexture2D(&depthTexDesc, nullptr, &depth);

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
	depthSrvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	res = device->CreateShaderResourceView(depth.Get(), &depthSrvDesc, &depthSRV);
	res = device->CreateRenderTargetView(depth.Get(), nullptr, &depthRTV);

	D3D11_TEXTURE2D_DESC normalTexDesc = {};
	normalTexDesc.Width = 800;
	normalTexDesc.Height = 800;
	normalTexDesc.MipLevels = 1;
	normalTexDesc.ArraySize = 1;
	normalTexDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	normalTexDesc.Usage = D3D11_USAGE_DEFAULT;
	normalTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	normalTexDesc.CPUAccessFlags = 0;
	normalTexDesc.MiscFlags = 0;
	normalTexDesc.SampleDesc = { 1, 0 };

	res = device->CreateTexture2D(&normalTexDesc, nullptr, &normal);

	D3D11_SHADER_RESOURCE_VIEW_DESC normalSrvDesc = {};
	normalSrvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	normalSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	res = device->CreateShaderResourceView(normal.Get(), &normalSrvDesc, &normalSRV);
	res = device->CreateRenderTargetView(normal.Get(), nullptr, &normalRTV);

	D3D11_TEXTURE2D_DESC diffuseTexDesc = {};
	diffuseTexDesc.Width = 800;
	diffuseTexDesc.Height = 800;
	diffuseTexDesc.MipLevels = 1;
	diffuseTexDesc.ArraySize = 1;
	diffuseTexDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	diffuseTexDesc.Usage = D3D11_USAGE_DEFAULT;
	diffuseTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	diffuseTexDesc.CPUAccessFlags = 0;
	diffuseTexDesc.MiscFlags = 0;
	diffuseTexDesc.SampleDesc = { 1, 0 };

	res = device->CreateTexture2D(&diffuseTexDesc, nullptr, &diffuse);

	D3D11_SHADER_RESOURCE_VIEW_DESC diffuseSpecExpSrvDesc = {};
	diffuseSpecExpSrvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	diffuseSpecExpSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	res = device->CreateShaderResourceView(diffuse.Get(), &diffuseSpecExpSrvDesc, &diffuseSRV);
	res = device->CreateRenderTargetView(diffuse.Get(), nullptr, &diffuseRTV);

	D3D11_TEXTURE2D_DESC specExpTexDesc = {};
	specExpTexDesc.Width = 800;
	specExpTexDesc.Height = 800;
	specExpTexDesc.MipLevels = 1;
	specExpTexDesc.ArraySize = 1;
	specExpTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	specExpTexDesc.Usage = D3D11_USAGE_DEFAULT;
	specExpTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	specExpTexDesc.CPUAccessFlags = 0;
	specExpTexDesc.MiscFlags = 0;
	specExpTexDesc.SampleDesc = { 1, 0 };

	res = device->CreateTexture2D(&specExpTexDesc, nullptr, &specExp);

	D3D11_SHADER_RESOURCE_VIEW_DESC specExpSrvDesc = {};
	specExpSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	specExpSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	res = device->CreateShaderResourceView(specExp.Get(), &specExpSrvDesc, &specExpSRV);
	res = device->CreateRenderTargetView(specExp.Get(), nullptr, &specExpRTV);
}

void GBuffer::setGBufferRenderTargets() const
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11RenderTargetView* rawDepthRTV = depthRTV.Get();
	context->OMSetRenderTargets(0, &rawDepthRTV, nullptr);
	ID3D11RenderTargetView* rawNormalRTV = normalRTV.Get();
	context->OMSetRenderTargets(1, &rawNormalRTV, nullptr);
	ID3D11RenderTargetView* rawDiffuseRTV = diffuseRTV.Get();
	context->OMSetRenderTargets(2, &rawDiffuseRTV, nullptr);
	ID3D11RenderTargetView* rawSpecExpRTV = specExpRTV.Get();
	context->OMSetRenderTargets(3, &rawSpecExpRTV, nullptr);
}
