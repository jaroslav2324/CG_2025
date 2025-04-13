#pragma once

#include <array>

#include <wrl/client.h>
#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

constexpr int cascadeCount = 4;

struct LightSourceShadowMapData {
	
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrices[cascadeCount];
	float cascadeSplits[cascadeCount];
	int textureSize = 1024;
	int padding[3];	
};

using Microsoft::WRL::ComPtr;

struct ShadowMap
{
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Matrix = DirectX::SimpleMath::Matrix;

	void init();
	void recalculateMatrices(Vector3 lsDirection);
	LightSourceShadowMapData data;

	ComPtr<ID3D11Texture2D> texture;
	ComPtr<ID3D11ShaderResourceView> shaderResView;
	ComPtr<ID3D11DepthStencilView> depthViews[cascadeCount];
};