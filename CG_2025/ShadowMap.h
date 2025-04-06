#pragma once

#include <array>

#include <wrl/client.h>
#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

using Microsoft::WRL::ComPtr;

struct ShadowMap 
{
	using Matrix = DirectX::SimpleMath::Matrix;

	void init();
	Matrix projectionMatrix;
	std::array<Matrix, 6> viewMatrices;
	int textureSize = 1024;

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResView;
	std::array<ComPtr<ID3D11DepthStencilView>, 6> depthViews;
};