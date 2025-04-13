#pragma once

#include "d3d.h"
#include "d3d11.h"
#include "directxtk/SimpleMath.h"

#include "ShadowMap.h"

class PlanetComponent;

enum LightSourceType {
	DIRECTIONAL_LIGHT = 0,
	POINT_LIGHT = 1,
	SPOT_LIGHT = 2
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
};