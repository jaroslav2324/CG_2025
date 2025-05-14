#include "FireParticleSystem.h"

#include <array>

#include "global.h"

using DirectX::SimpleMath::Vector2;

void FireParticleSystem::update(float deltaTime)
{
	for (auto& p : particleList) {
		p.velocity += p.acceleration * deltaTime;
		p.position += p.velocity * deltaTime;
		p.lifetime += deltaTime;
	}
}

void FireParticleSystem::simulate(float deltaTime)
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();

	particleData.deltaTime = deltaTime;
	particleData.numParticles = maxParticles;
	mapConstantBuffer(particleDataBuffer, particleData, context);

	ID3D11UnorderedAccessView* views[] = { particleBufferUAV.Get(), sortListBufferUAV.Get(), deadListBufferAppendUAV.Get() };
	context->CSSetUnorderedAccessViews(0, 1, views, nullptr);
	ID3D11Buffer* rawParticleDataBuffer = particleDataBuffer.Get();
	context->CSSetConstantBuffers(0, 1, &rawParticleDataBuffer);
	context->CSSetShader(computeSimulateShader.Get(), nullptr, 0);
	context->Dispatch(32, 24, 1);
	context->CSSetUnorderedAccessViews(0, 1, nullptr, nullptr);
}

void FireParticleSystem::render()
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();

	// сработает?
	context->CopyStructureCount(
		indirectArgsBuffer.Get(), 4, sortListBufferUAV.Get());

	context->VSSetShader(vertexShader.Get(), nullptr, 0);

	context->GSSetShader(geometryShader.Get(), nullptr, 0);
	
	context->PSSetShader(pixelShader.Get(), nullptr, 0);

	context->DrawInstancedIndirect(indirectArgsBuffer.Get(), 0);
}

void FireParticleSystem::emit(int countToInit)
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	particleData.numEmitInThisFrame = countToInit;
	particleData.emitPosition = origin;
	mapConstantBuffer(particleDataBuffer, particleData, context);

	context->CSSetShader(computeEmitShader.Get(), nullptr, 0);
	ID3D11UnorderedAccessView* views[] = {particleBufferUAV.Get(), deadListBufferConsumeUAV.Get()};
	context->CSSetUnorderedAccessViews(0, 2, views, nullptr);
	ID3D11Buffer* rawParticleDataBuffer = particleDataBuffer.Get();
	context->CSSetConstantBuffers(0, 1, &rawParticleDataBuffer);
	// TODO: dispatch in multiple threads
	context->Dispatch(1, 1, 1);
	context->CSSetUnorderedAccessViews(0, 2, nullptr, nullptr);
	particleData.numEmitInThisFrame = 0;
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
	device->CreateUnorderedAccessView(deadListBuffer.Get(), &uavDesc, &deadListBufferAppendUAV);
	uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = 0;
	device->CreateUnorderedAccessView(deadListBuffer.Get(), &uavDesc, &deadListBufferConsumeUAV);
	srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;
	device->CreateShaderResourceView(deadListBuffer.Get(), &srvDesc, &deadListBufferSRV);

	// indirect args buffer
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = 16;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	desc.CPUAccessFlags = 0;

	D3D11_DRAW_INSTANCED_INDIRECT_ARGS args = {};
	args.VertexCountPerInstance = 1; 
	args.InstanceCount = 0;          
	args.StartVertexLocation = 0;
	args.StartInstanceLocation = 0;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = &args;

	device->CreateBuffer(&desc, &srd, &indirectArgsBuffer);

	computeSimulateShader = shaderManager->compileCreateComputeShader(L"./shaders/fireSimulateComputeShader.hlsl", "CSMain", "cs_5_0", nullptr);
	computeEmitShader = shaderManager->compileCreateComputeShader(L"./shaders/fireEmitComputeShader.hlsl", "CSMain", "cs_5_0", nullptr);
	vertexShader = shaderManager->compileCreateShader<ID3D11VertexShader>(device, L"./shaders/fireVertexShader.hlsl", "VSMain", "vs_5_0", nullptr);
	geometryShader = shaderManager->compileCreateShader<ID3D11GeometryShader>(device, L"./shaders/fireGeometryShader.hlsl", "GSMain", "gs_5_0", nullptr);
	pixelShader = shaderManager->compileCreateShader<ID3D11PixelShader>(device, L"./shaders/firePixelShader.hlsl", "PSMain", "ps_5_0", nullptr);
}

void FireParticleSystem::initParticle(int index)
{

}
