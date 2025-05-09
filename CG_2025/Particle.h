#pragma once

#include "directxtk/SimpleMath.h"

struct Particle
{
private:
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Vector4 = DirectX::SimpleMath::Vector4;
public:
	Vector4 initialColor;
	Vector4 endColor;
	Vector3 position;
	Vector3 prevPosition;
	Vector3 velocity;
	Vector3 acceleration;
	float maxLifetime;
	float lifetime;
	float initialSize;
	float endSize;
	float initialWeight;
	float endWeight;
};