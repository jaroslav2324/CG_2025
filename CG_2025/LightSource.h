#pragma once

#include "d3d.h"
#include "d3d11.h"
#include "directxtk/SimpleMath.h"

#include "ShadowMap.h"
#include "addData.h"

class PlanetComponent;

enum LightSourceType {
	AMBIENT_LIGHT = 0,
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct LightSourceData {
	using Vector4 = DirectX::SimpleMath::Vector4;

	Vector4 position;
	Vector4 direction;
	Vector4 rgb;
	LightSourceType sourceType;
	float shineDistance;
	float angle;
	float intensity = 1.0f;
};

class LightSource {
public:
	void init();
	LightSourceData ls;
	ShadowMap shMap;
	PlanetComponent* mesh = nullptr;
	AdditionalData addData;

	ComPtr <ID3D11Buffer> getVertexBuffer() const { return vertexBufferPointSpot; };
	ComPtr <ID3D11Buffer> getIndexBuffer() const { return indexBufferPointSpot; };
	const std::vector<UINT>& getStrides() const { return stridesPointSpot; };
	const std::vector<UINT>& getOffsets() const { return offsetsPointSpot; };
	ComPtr<ID3D11Buffer> getLightSourceDataConstBuffer() const { return lightSourceDataConstBuffer; };
	ComPtr<ID3D11Buffer> getAdditionalConstBuffer() const { return additionalConstBuffer; };
	void mapAdditionalConstBuffer();
	void mapLightSourceDataConstBuffer();

private:
	ComPtr<ID3D11Buffer> lightSourceDataConstBuffer = nullptr;
	ComPtr<ID3D11Buffer> additionalConstBuffer = nullptr;

	ComPtr <ID3D11Buffer> vertexBufferPointSpot = nullptr;
	ComPtr <ID3D11Buffer> indexBufferPointSpot = nullptr;

	std::vector<UINT> stridesPointSpot = { 16 };
	std::vector<UINT> offsetsPointSpot = {0};
};