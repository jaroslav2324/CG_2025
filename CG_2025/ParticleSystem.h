#pragma once

#include <vector>

#include "directxtk/SimpleMath.h"

#include "Particle.h"

class ParticleSystem
{
protected:
	using Vector3 = DirectX::SimpleMath::Vector3;

public:
	ParticleSystem(int maxParticles, Vector3 origin): maxParticles(maxParticles), origin(origin) {};
	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;
	virtual void emit(int numParticles) {};
	virtual void init() {};

protected:
	virtual void initParticle(int index) = 0;
	std::vector<Particle> particleList;
	int maxParticles;
	int numParticles;
	Vector3 origin;
	float lastEmitTime;
	Vector3 force;
};