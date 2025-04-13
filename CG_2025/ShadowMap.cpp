#include <limits>

#include "ShadowMap.h"
#include "global.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

//std::vector<DirectX::SimpleMath::Vector4> getFrustumCornersWorldSpace(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
std::vector<DirectX::SimpleMath::Vector4> getFrustumCornersWorldSpace(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	const auto viewProj = view * proj;
	const auto inv = viewProj.Invert();

	std::vector<Vector4> frustumCorners;
	frustumCorners.reserve(8);
	for (int z = 0; z < 2; ++z)
	{
		for (int x = 0; x < 2; ++x)
		{
			for (int y = 0; y < 2; ++y)
			{
				const Vector4 pt = Vector4::Transform(Vector4(
					2.0f * x - 1.0f,
					2.0f * y - 1.0f,
					z,
					1.0f
				), inv);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}


void ShadowMap::init()
{
	//data.projectionMatrix = Matrix::CreateOrthographic(15, 15, 0.1f, 10.0f);
	float nearPlane = 0.1f;
	float frustumLength = (10.0f - nearPlane);
	for (int i = 1; i <= cascadeCount; ++i) {
		data.cascadeSplits[i - 1] = nearPlane + i * frustumLength / cascadeCount;
	}

	auto device = GE::getGameSubsystem()->getDevice();

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = data.textureSize;
	depthTexDesc.Height = data.textureSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 4; 
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	device->CreateTexture2D(&depthTexDesc, nullptr, &texture);

	// Shader Resource View
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = 4;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;
	device->CreateShaderResourceView(texture.Get(), &srvDesc, &shaderResView);

	for (int i = 0; i < 4; ++i)
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
		dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		dsvDesc.Texture2DArray.MipSlice = 0;
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		dsvDesc.Texture2DArray.ArraySize = 1;

		device->CreateDepthStencilView(texture.Get(), &dsvDesc, &depthViews[i]);
	}
}


void ShadowMap::recalculateMatrices(Vector3 lsDirection)
{
	//data.viewMatrix = view;
	auto cameraFrustumCorners = getFrustumCornersWorldSpace(GE::getCameraViewMatrix(), GE::getProjectionMatrix());
	Vector3 center = Vector3::Zero;
	for (const auto& v: cameraFrustumCorners)
	{
		center += Vector3(v);
	}
	center /= cameraFrustumCorners.size();

	const auto lightView = Matrix::CreateLookAt(center, center + lsDirection, Vector3::Up);
	data.viewMatrix = lightView;

	std::vector<Vector4> slideVectors = {
	cameraFrustumCorners[4] - cameraFrustumCorners[0],
	cameraFrustumCorners[5] - cameraFrustumCorners[1],
	cameraFrustumCorners[6] - cameraFrustumCorners[2],
	cameraFrustumCorners[7] - cameraFrustumCorners[3]
	};

	for (int cascade = 1; cascade <= cascadeCount; ++cascade) {
		std::vector<Vector4> cascadeCorners;
		cascadeCorners.reserve(8);
		for (int i = 0; i < 4; i++)	cascadeCorners.push_back(cameraFrustumCorners[i] + (cascade - 1) * slideVectors[i] / cascadeCount);
		for (int i = 0; i < 4; i++)	cascadeCorners.push_back(cameraFrustumCorners[i] + cascade * slideVectors[i] / cascadeCount);
		
		float minX = (std::numeric_limits<float>::max)();
		float maxX = std::numeric_limits<float>::lowest();
		float minY = (std::numeric_limits<float>::max)();
		float maxY = std::numeric_limits<float>::lowest();
		float minZ = (std::numeric_limits<float>::max)();
		float maxZ = std::numeric_limits<float>::lowest();

		for (const auto& v : cascadeCorners) {
			const auto trf = Vector4::Transform(v, lightView);
			minX = (std::min)(minX, trf.x);
			maxX = (std::max)(maxX, trf.x);
			minY = (std::min)(minY, trf.y);
			maxY = (std::max)(maxY, trf.y);
			minZ = (std::min)(minZ, trf.z);
			maxZ = (std::max)(maxZ, trf.z);
		}

		constexpr float zMult = 15.0f;
		minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
		maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;
		auto lightProjection = Matrix::CreateOrthographicOffCenter(minX, maxX, minY, maxY, minZ, maxZ);
		data.projectionMatrices[cascade - 1] = lightProjection;
	}


}
