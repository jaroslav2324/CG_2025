#include <comdef.h>

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
	depthTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.SampleDesc = { 1, 0 };

	auto res = device->CreateTexture2D(&depthTexDesc, nullptr, &depth);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc = {};
	depthSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	depthSrvDesc.Texture2D.MipLevels = 1;
	depthSrvDesc.Texture2D.MostDetailedMip = 0;
	D3D11_RENDER_TARGET_VIEW_DESC depthRtvDesc = {};
	depthRtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	depthRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	depthRtvDesc.Texture2D.MipSlice = 0;
	res = device->CreateShaderResourceView(depth.Get(), &depthSrvDesc, &depthSRV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}
	res = device->CreateRenderTargetView(depth.Get(), &depthRtvDesc, &depthRTV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}

	D3D11_TEXTURE2D_DESC normalTexDesc = {};
	normalTexDesc.Width = 800;
	normalTexDesc.Height = 800;
	normalTexDesc.MipLevels = 1;
	normalTexDesc.ArraySize = 1;
	normalTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	normalTexDesc.Usage = D3D11_USAGE_DEFAULT;
	normalTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	normalTexDesc.CPUAccessFlags = 0;
	normalTexDesc.MiscFlags = 0;
	normalTexDesc.SampleDesc = { 1, 0 };

	res = device->CreateTexture2D(&normalTexDesc, nullptr, &normal);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC normalSrvDesc = {};
	normalSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	normalSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	normalSrvDesc.Texture2D.MipLevels = 1;
	normalSrvDesc.Texture2D.MostDetailedMip = 0;
	D3D11_RENDER_TARGET_VIEW_DESC normalRtvDesc = {};
	normalRtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	normalRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	normalRtvDesc.Texture2D.MipSlice = 0;
	res = device->CreateShaderResourceView(normal.Get(), &normalSrvDesc, &normalSRV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}
	res = device->CreateRenderTargetView(normal.Get(), &normalRtvDesc, &normalRTV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}

	D3D11_TEXTURE2D_DESC diffuseTexDesc = {};
	diffuseTexDesc.Width = 800;
	diffuseTexDesc.Height = 800;
	diffuseTexDesc.MipLevels = 1;
	diffuseTexDesc.ArraySize = 1;
	diffuseTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	diffuseTexDesc.Usage = D3D11_USAGE_DEFAULT;
	diffuseTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	diffuseTexDesc.CPUAccessFlags = 0;
	diffuseTexDesc.MiscFlags = 0;
	diffuseTexDesc.SampleDesc = { 1, 0 };

	res = device->CreateTexture2D(&diffuseTexDesc, nullptr, &diffuse);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC diffuseSrvDesc = {};
	diffuseSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	diffuseSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	diffuseSrvDesc.Texture2D.MipLevels = 1;
	diffuseSrvDesc.Texture2D.MostDetailedMip = 0;
	D3D11_RENDER_TARGET_VIEW_DESC diffuseRtvDesc = {};
	diffuseRtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	diffuseRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	diffuseRtvDesc.Texture2D.MipSlice = 0;
	res = device->CreateShaderResourceView(diffuse.Get(), &diffuseSrvDesc, &diffuseSRV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}
	res = device->CreateRenderTargetView(diffuse.Get(), &diffuseRtvDesc, &diffuseRTV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}

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
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC specExpSrvDesc = {};
	specExpSrvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	specExpSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	specExpSrvDesc.Texture2D.MipLevels = 1;
	specExpSrvDesc.Texture2D.MostDetailedMip = 0;
	D3D11_RENDER_TARGET_VIEW_DESC specExpRtvDesc = {};
	specExpRtvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	specExpRtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	specExpRtvDesc.Texture2D.MipSlice = 0;
	res = device->CreateShaderResourceView(specExp.Get(), &specExpSrvDesc, &specExpSRV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}
	res = device->CreateRenderTargetView(specExp.Get(), &specExpRtvDesc, &specExpRTV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}
}

void GBuffer::setGBufferRenderTargets() const
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11RenderTargetView* rtvs[] = {
		depthRTV.Get(),
		normalRTV.Get(),
		diffuseRTV.Get(),
		specExpRTV.Get()
	};
	context->OMSetRenderTargets(4, rtvs, GE::getGameSubsystem()->getDepthView().Get());
}

void GBuffer::clearRenderTargets() const
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(depthRTV.Get(), clearColor);
	context->ClearRenderTargetView(normalRTV.Get(), clearColor);
	context->ClearRenderTargetView(diffuseRTV.Get(), clearColor);
	context->ClearRenderTargetView(specExpRTV.Get(), clearColor);
}
