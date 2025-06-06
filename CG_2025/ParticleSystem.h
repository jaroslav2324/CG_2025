#pragma once

#include <vector>

#include <wrl/client.h>
#include "d3d11.h"
#include "directxtk/SimpleMath.h"

#include "Particle.h"

using Microsoft::WRL::ComPtr;

constexpr int injectionBufferSize = 1024;

class ParticleSystem
{
protected:
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Color3 = DirectX::SimpleMath::Vector3;
	using Color4 = DirectX::SimpleMath::Vector4;
	using Matrix = DirectX::SimpleMath::Matrix;

public:
	ParticleSystem(unsigned int maxParticles, Vector3 origin): maxParticles(maxParticles), origin(origin) {};
	virtual void update(float deltaTime) = 0;
	virtual void simulate(float deltaTime) = 0;
	virtual void render() = 0;
	virtual void emit(int countToInit) = 0;
	virtual void init();
	void swapSortListBuffers();
	void sort();

protected:
	virtual void initParticle(int index) = 0;
	void setConstants(UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight);
	// TODO: remove/replace?
	std::vector<Particle> particleList;
	ComPtr<ID3D11Buffer> particleBuffer = nullptr;
	ComPtr<ID3D11UnorderedAccessView> particleBufferUAV;
	ComPtr<ID3D11ShaderResourceView> particleBufferSRV;
	ComPtr<ID3D11Buffer> sortListBuffer = nullptr;
	ComPtr<ID3D11UnorderedAccessView> sortListBufferUAV;
	ComPtr<ID3D11ShaderResourceView> sortListBufferSRV;
	ComPtr<ID3D11Buffer> sortListBuffer2 = nullptr;
	ComPtr<ID3D11UnorderedAccessView> sortListBufferUAV2;
	ComPtr<ID3D11ShaderResourceView> sortListBufferSRV2;
	ComPtr<ID3D11Buffer> deadListBuffer = nullptr;
	ComPtr<ID3D11UnorderedAccessView> deadListBufferUAV;
	ComPtr<ID3D11ShaderResourceView> deadListBufferSRV;
	ComPtr<ID3D11Buffer> indirectArgsBuffer;
	ComPtr<ID3D11Buffer> constSortBuf;

	ComPtr<ID3D11ComputeShader> bitonicSortShader;
	ComPtr<ID3D11ComputeShader> bitonicTransposeShader;

	unsigned int maxParticles;
	Vector3 origin;

	struct CBSort
	{
		UINT iLevel;
		UINT iLevelMask;
		UINT iWidth;
		UINT iHeight;
	};

	CBSort cbSortData;

	const int maxNumInjectionParticles = injectionBufferSize;
	Particle injectionParticleData[injectionBufferSize];
	ComPtr<ID3D11Buffer> injectionBuffer;
	ComPtr<ID3D11ShaderResourceView> injectionSRV;
};