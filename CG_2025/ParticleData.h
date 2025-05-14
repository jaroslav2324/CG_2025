#pragma once

#include "directxtk/SimpleMath.h"

struct ParticleData
{
private:
	using Vector3 = DirectX::SimpleMath::Vector3;
public:
	float deltaTime;
	unsigned int numParticles;
	unsigned int numEmitInThisFrame;
	int _1;
	Vector3 emitPosition;
	int _2;
};