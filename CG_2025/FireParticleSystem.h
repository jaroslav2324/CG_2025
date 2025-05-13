#pragma once

#include "ParticleSystem.h"
#include "ParticleData.h"

class FireParticleSystem : public ParticleSystem {
public:
	FireParticleSystem(unsigned int maxParticles, Vector3 origin) : ParticleSystem(maxParticles, origin) {};
	void update(float deltaTime) override;
	void render() override;
	void emit(int countToInit) override;
	void init() override;

protected:
	void initParticle(int index) override;
	ComPtr<ID3D11ComputeShader> computeSimulateShader;
	ComPtr<ID3D11ComputeShader> computeEmitShader;
	ParticleData particleData;
	ComPtr<ID3D11Buffer> particleDataBuffer;
};