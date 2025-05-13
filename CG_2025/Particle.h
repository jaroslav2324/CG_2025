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
	Vector3 position;		float _1;
	Vector3 prevPosition;	float _2;
	Vector3 velocity;		float _3;
	Vector3 acceleration;	float _4;
	float maxLifetime;
	float lifetime;
	float initialSize= 1.0f;
	float endSize = 1.0f;
	float initialWeight = 1.0f;
	float endWeight = 1.0f;
};