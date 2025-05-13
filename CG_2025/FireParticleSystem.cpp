#include "FireParticleSystem.h"

#include "global.h"

using DirectX::SimpleMath::Vector2;

void FireParticleSystem::update(float deltaTime)
{
	for (auto& p : particleList) {
		p.velocity += p.acceleration * deltaTime;
		p.position += p.velocity * deltaTime;
		p.lifetime += deltaTime;
	}
	particleData.deltaTime = deltaTime;
	particleData.numParticles = maxParticles;
}

void FireParticleSystem::render()
{
	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawParticleDataBuffer = particleDataBuffer.Get();
	context->Map(rawParticleDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &particleData, sizeof(particleData));
	context->Unmap(rawParticleDataBuffer, 0);

	// TODO: move simulation somewhere?
	context->CSSetUnorderedAccessViews(0, 1, &particleBufferUAV, nullptr);
	rawParticleDataBuffer = particleDataBuffer.Get();
	context->CSSetConstantBuffers(0, 1, &rawParticleDataBuffer);
	context->CSSetShader(computeSimulateShader.Get(), nullptr, 0);
	context->Dispatch(32, 24, 1);
	context->CSSetUnorderedAccessViews(0, 1, nullptr, nullptr);
	particleData.numEmitInThisFrame = 0;
}

void FireParticleSystem::emit(int countToInit)
{
	particleData.numEmitInThisFrame = countToInit;
	// TODO: call emit compute shader?

}

void FireParticleSystem::init()
{
	auto device = GE::getGameSubsystem()->getDevice();
	auto bufferManager = GE::getBufferManager();
	auto shaderManager = GE::getShaderManager();
	particleBuffer = bufferManager->createRWStructuredBuffer<Particle>(device, maxParticles);
	sortListBuffer = bufferManager->createRWStructuredBuffer<Vector2>(device, maxParticles);
	deadListBuffer = bufferManager->createRWStructuredBuffer<unsigned int>(device, maxParticles);
	particleDataBuffer = bufferManager->createConstDynamicBufferCPUWrite(particleData);

	// particle buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; 
	device->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, &particleBufferUAV);
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = maxParticles;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	device->CreateShaderResourceView(particleBuffer.Get(), &srvDesc, &particleBufferSRV);

	// sort list
	uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	device->CreateUnorderedAccessView(sortListBuffer.Get(), &uavDesc, &sortListBufferUAV);
	srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = maxParticles;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	device->CreateShaderResourceView(sortListBuffer.Get(), &srvDesc, &sortListBufferSRV);

	// dead list
	uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	device->CreateUnorderedAccessView(deadListBuffer.Get(), &uavDesc, &deadListBufferUAV);
	srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;
	device->CreateShaderResourceView(deadListBuffer.Get(), &srvDesc, &deadListBufferSRV);

	computeSimulateShader = shaderManager->compileCreateComputeShader(L"./shaders/fireSimulateComputeShader.hlsl", "CSMain", "cs_5_0", nullptr);
	computeEmitShader = shaderManager->compileCreateComputeShader(L"./shaders/fireEmitComputeShader.hlsl", "CSMain", "cs_5_0", nullptr);
}

void FireParticleSystem::initParticle(int index)
{
	// TODO: USELESS?
	Particle& p = particleList[index];
	p.position = origin;
	p.velocity = Vector3(0.0f, 1.0f, 0.0f);
	p.acceleration = Vector3(0, 0.05f, 0);
	p.maxLifetime = 3.0f;
	p.initialColor = Color4(1.0, 0.5, 0.0, 1.0);
	p.endColor = Color4(0.5, 0.2, 0.0, 1.0);
}
