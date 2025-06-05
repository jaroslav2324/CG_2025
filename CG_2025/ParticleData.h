#pragma once

#include "directxtk/SimpleMath.h"

struct ParticleData
{
private:
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Matrix = DirectX::SimpleMath::Matrix;
public:
	float deltaTime;
	unsigned int maxNumParticles;
	unsigned int numEmitInThisFrame;
	unsigned int numAliveParticles;
	Vector3 emitPosition;
	int _1;
	Vector3 cameraPosition;
	int _2;
	Matrix view;
	Matrix proj;
};