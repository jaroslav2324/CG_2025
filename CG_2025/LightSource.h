#pragma once

#include "d3d.h"
#include "d3d11.h"
#include "directxtk/SimpleMath.h"

enum LightSourceType {
	GLOBAL_LIGHT = 0,
	POINT_LIGHT = 1,
	SPOT_LIGHT = 2
};

struct LightSouce {
	using Vector4 = DirectX::SimpleMath::Vector4;

	Vector4 position;
	Vector4 direction;
	LightSourceType sourceType;
	float shineDistance;
	float angle;
	float intensity = 1.0f;
};