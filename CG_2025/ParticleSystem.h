#pragma once

#include <vector>

#include <wrl/client.h>
#include "d3d11.h"
#include "directxtk/SimpleMath.h"

#include "Particle.h"

using Microsoft::WRL::ComPtr;

class ParticleSystem
{
protected:
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Color3 = DirectX::SimpleMath::Vector3;
	using Color4 = DirectX::SimpleMath::Vector4;

public:
	ParticleSystem(unsigned int maxParticles, Vector3 origin): maxParticles(maxParticles), origin(origin) {};
	virtual void update(float deltaTime) = 0;
	virtual void simulate(float deltaTime) = 0;
	virtual void render() = 0;
	virtual void emit(int countToInit) = 0;
	virtual void init() = 0;

protected:
	virtual void initParticle(int index) = 0;
	// TODO: remove/replace?
	std::vector<Particle> particleList;
	ComPtr<ID3D11Buffer> particleBuffer = nullptr;
	ComPtr<ID3D11UnorderedAccessView> particleBufferUAV;
	ComPtr<ID3D11ShaderResourceView> particleBufferSRV;
	ComPtr<ID3D11Buffer> sortListBuffer = nullptr;
	ComPtr<ID3D11UnorderedAccessView> sortListBufferUAV;
	ComPtr<ID3D11ShaderResourceView> sortListBufferSRV;
	ComPtr<ID3D11Buffer> deadListBuffer = nullptr;
	ComPtr<ID3D11UnorderedAccessView> deadListBufferAppendUAV;
	ComPtr<ID3D11UnorderedAccessView> deadListBufferConsumeUAV;
	ComPtr<ID3D11ShaderResourceView> deadListBufferSRV;
	ComPtr<ID3D11Buffer> indirectArgsBuffer;
	unsigned int maxParticles;
	unsigned int numParticles;
	Vector3 origin;
	Vector3 force;
};