#include "ShadowMap.h"
#include "global.h"

using namespace DirectX::SimpleMath;

void ShadowMap::init()
{
	auto device = GE::getGameSubsystem()->getDevice();

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = textureSize;
	depthTexDesc.Height = textureSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 6;
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	depthTexDesc.SampleDesc = { 1, 0 };

	auto res = device->CreateTexture2D(&depthTexDesc, nullptr, &texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = 1;
	device->CreateShaderResourceView(texture.Get(), &srvDesc, &shaderResView);

	for (int i = 0; i < 6; ++i)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		dsvDesc.Texture2DArray.ArraySize = 1;
		dsvDesc.Flags = 0;

		device->CreateDepthStencilView(texture.Get(), &dsvDesc, &depthViews[i]);
	}
}
