#pragma once

#include "ParticleSystem.h"
#include "ParticleData.h"

class FireParticleSystem : public ParticleSystem {
public:
	FireParticleSystem(unsigned int maxParticles, Vector3 origin) : ParticleSystem(maxParticles, origin) {};
	void update(float deltaTime) override;
	void simulate(float deltaTime) override;
	void render() override;
	void emit(int countToInit) override;
	void init() override;

protected:
	DirectX::SimpleMath::Vector4 randClr();
	void initParticles(int count);
	void initParticle(int index) override;
	ComPtr<ID3D11ComputeShader> computeSimulateShader;
	ComPtr<ID3D11ComputeShader> computeEmitShader;
	ComPtr<ID3D11VertexShader> vertexShader;
	ComPtr<ID3D11GeometryShader> geometryShader;
	ComPtr<ID3D11PixelShader> pixelShader;
	ParticleData particleData;
	ComPtr<ID3D11Buffer> particleDataBuffer;

	ComPtr<ID3D11SamplerState> sampler;
	ID3D11ShaderResourceView* particleTexture = nullptr;

	struct CameraData
	{
		Matrix view;
		Matrix proj;
		Vector3 forward;
		float _1;
		Vector3 up;
		float _2;
	};

	CameraData camData;
	ComPtr<ID3D11Buffer> camDataBuffer;
};