#include <comdef.h>

#include "global.h"
#include "GBuffer.h"

void GBuffer::init()
{
	auto device = GE::getGameSubsystem()->getDevice();

	D3D11_TEXTURE2D_DESC depthAmbientTexDesc = {};
	depthAmbientTexDesc.Width = 800;
	depthAmbientTexDesc.Height = 800;
	depthAmbientTexDesc.MipLevels = 1;
	depthAmbientTexDesc.ArraySize = 1;
	depthAmbientTexDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	depthAmbientTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthAmbientTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	depthAmbientTexDesc.CPUAccessFlags = 0;
	depthAmbientTexDesc.MiscFlags = 0;
	depthAmbientTexDesc.SampleDesc = { 1, 0 };

	auto res = device->CreateTexture2D(&depthAmbientTexDesc, nullptr, &depthAmbient);
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
	res = device->CreateShaderResourceView(depthAmbient.Get(), &depthSrvDesc, &depthAmbientSRV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}
	res = device->CreateRenderTargetView(depthAmbient.Get(), &depthRtvDesc, &depthAmbientRTV);
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

	res = device->CreateTexture2D(&diffuseTexDesc, nullptr, &diffuseNormal);
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
	res = device->CreateShaderResourceView(diffuseNormal.Get(), &diffuseSrvDesc, &diffuseNormalSRV);
	if (FAILED(res)) {
		_com_error err(res);
		std::wcerr << L"Îøèáêà: " << err.ErrorMessage() << std::endl;
	}
	res = device->CreateRenderTargetView(diffuseNormal.Get(), &diffuseRtvDesc, &diffuseNormalRTV);
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
		depthAmbientRTV.Get(),
		diffuseNormalRTV.Get(),
		specExpRTV.Get()
	};
	context->OMSetRenderTargets(3, rtvs, GE::getGameSubsystem()->getDepthView().Get());
}

void GBuffer::bindPixelShaderResourceViews(int startSlot) const
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11ShaderResourceView* rsvs[] = {
		depthAmbientSRV.Get(),
		diffuseNormalSRV.Get(),
		specExpSRV.Get(),
		GE::getGameSubsystem()->getDepthSrv().Get()
	};
	context->PSSetShaderResources(startSlot, 4, rsvs);
}

void GBuffer::clearRenderTargets() const
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	const float clearColorWhite[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const float clearColorBlack[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(depthAmbientRTV.Get(), clearColorWhite);
	context->ClearRenderTargetView(diffuseNormalRTV.Get(), clearColorBlack);
	context->ClearRenderTargetView(specExpRTV.Get(), clearColorBlack);
}
