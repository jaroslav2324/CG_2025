#include "FireParticleSystem.h"

#include <array>

#include "global.h"
#include "SortListStruct.h"

void FireParticleSystem::update(float deltaTime)
{
	emit(10);
	simulate(deltaTime);
	sort();
	
}

// static bool firstSimulation = true;
void FireParticleSystem::simulate(float deltaTime)
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();

	particleData.deltaTime = deltaTime;
	particleData.maxNumParticles = maxParticles;
	particleData.cameraPosition = GE::getCameraPosition();
	mapConstantBuffer(particleDataBuffer, particleData, context);
	context->CopyStructureCount(particleDataBuffer.Get(), 12, sortListBufferUAV.Get());
	
	UINT minOne = (UINT) - 1;
	UINT zero = 0;
	context->CSSetUnorderedAccessViews(0, 1, particleBufferUAV.GetAddressOf(), nullptr);
	//context->CSSetUnorderedAccessViews(1, 1, sortListBufferUAV.GetAddressOf(), &zero);
	context->CSSetUnorderedAccessViews(1, 1, sortListBufferUAV.GetAddressOf(), &minOne);

	context->CSSetUnorderedAccessViews(2, 1, deadListBufferUAV.GetAddressOf(), &minOne);
	ID3D11Buffer* rawParticleDataBuffer = particleDataBuffer.Get();
	context->CSSetConstantBuffers(0, 1, &rawParticleDataBuffer);
	context->CSSetShader(computeSimulateShader.Get(), nullptr, 0);
	// TODO: calculate from max particles
	context->Dispatch(32, 32, 1);
	ID3D11UnorderedAccessView* np = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &np, nullptr);
	context->CSSetUnorderedAccessViews(1, 1, &np, nullptr);
	context->CSSetUnorderedAccessViews(2, 1, &np, nullptr);

}

void FireParticleSystem::render()
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();

	camData.view = GE::getCameraViewMatrix().Transpose();
	camData.proj = GE::getProjectionMatrix().Transpose();
	camData.forward = GE::getCameraForwardVector();
	camData.up = GE::getCameraUpVector();
	mapConstantBuffer(camDataBuffer, camData, context);
	context->CopyStructureCount(
		indirectArgsBuffer.Get(), 0, sortListBufferUAV.Get());

	ID3D11ShaderResourceView* views[] = { sortListBufferSRV.Get() };
	context->VSSetShaderResources(0, 1, views);
	context->VSSetShader(vertexShader.Get(), nullptr, 0);

	views[0] = particleBufferSRV.Get();
	context->GSSetShaderResources(0, 1, views);
	context->GSSetConstantBuffers(0, 1, camDataBuffer.GetAddressOf());
	context->GSSetShader(geometryShader.Get(), nullptr, 0);
	
	context->PSSetShader(pixelShader.Get(), nullptr, 0);

	context->IASetInputLayout(nullptr);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	context->DrawInstancedIndirect(indirectArgsBuffer.Get(), 0);
}

static bool firstEmit = true;
void FireParticleSystem::emit(int countToInit)
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	particleData.numEmitInThisFrame = countToInit;
	particleData.emitPosition = origin;
	particleData.maxNumParticles = maxParticles;
	particleData.cameraPosition = GE::getCameraPosition();
	if (firstEmit) {
		particleData.numAliveParticles = 0;
	}
	
	mapConstantBuffer(particleDataBuffer, particleData, context);
	if (!firstEmit) {
		context->CopyStructureCount(particleDataBuffer.Get(), 12, sortListBufferUAV.Get());
	}

	UINT minOne = (UINT) -1;
	UINT zero = 0;
	context->CSSetShader(computeEmitShader.Get(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, particleBufferUAV.GetAddressOf(), nullptr);
	if (firstEmit) {
		context->CSSetUnorderedAccessViews(1, 1, sortListBufferUAV.GetAddressOf(), &zero);
		context->CSSetUnorderedAccessViews(2, 1, deadListBufferUAV.GetAddressOf(), &maxParticles);
		firstEmit = false;
	}
	else { 
		context->CSSetUnorderedAccessViews(1, 1, sortListBufferUAV.GetAddressOf(), &minOne);
		context->CSSetUnorderedAccessViews(2, 1, deadListBufferUAV.GetAddressOf(), &minOne);
	}
	
	ID3D11Buffer* rawParticleDataBuffer = particleDataBuffer.Get();
	context->CSSetConstantBuffers(0, 1, &rawParticleDataBuffer);
	context->Dispatch(1, 1, 1);
	ID3D11UnorderedAccessView* np = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &np, nullptr);
	context->CSSetUnorderedAccessViews(1, 1, &np, nullptr);
	context->CSSetUnorderedAccessViews(2, 1, &np, nullptr);
	particleData.numEmitInThisFrame = 0;
}

void FireParticleSystem::init()
{
	ParticleSystem::init();

	auto device = GE::getGameSubsystem()->getDevice();
	auto bufferManager = GE::getBufferManager();
	auto shaderManager = GE::getShaderManager();
	particleBuffer = bufferManager->createRWStructuredBuffer<Particle>(device, maxParticles);
	std::vector<SortListStruct> sl;
	sl.reserve(maxParticles);
	for (uint32_t i = 0; i < maxParticles; ++i)
	{
		sl.push_back({ i, 100000.0f });
	}
	sortListBuffer = bufferManager->createRWStructuredBuffer<SortListStruct>(device, maxParticles, sl.data());
	std::vector<uint32_t> deadIndices(maxParticles);
	deadIndices.reserve(maxParticles);
	for (uint32_t i = 0; i < maxParticles; ++i) {
		deadIndices[i] = i;
	}
	deadListBuffer = bufferManager->createRWStructuredBuffer<unsigned int>(device, maxParticles, deadIndices.data());
	particleData.numAliveParticles = 0;
	particleDataBuffer = bufferManager->createConstDynamicBufferCPUWrite(particleData);

	// particle buffer
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN; 
	device->CreateUnorderedAccessView(particleBuffer.Get(), &uavDesc, particleBufferUAV.GetAddressOf());
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = maxParticles;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	device->CreateShaderResourceView(particleBuffer.Get(), &srvDesc, particleBufferSRV.GetAddressOf());

	// sort list
	uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
	device->CreateUnorderedAccessView(sortListBuffer.Get(), &uavDesc, sortListBufferUAV.GetAddressOf());
	srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.NumElements = maxParticles;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	device->CreateShaderResourceView(sortListBuffer.Get(), &srvDesc, sortListBufferSRV.GetAddressOf());

	// dead list
	uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = maxParticles;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	device->CreateUnorderedAccessView(deadListBuffer.Get(), &uavDesc, deadListBufferUAV.GetAddressOf());
	srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = maxParticles;
	device->CreateShaderResourceView(deadListBuffer.Get(), &srvDesc, deadListBufferSRV.GetAddressOf());

	// indirect args buffer
	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = 16;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	desc.CPUAccessFlags = 0;

	D3D11_DRAW_INSTANCED_INDIRECT_ARGS args = {};
	args.VertexCountPerInstance = 0; 
	args.InstanceCount = 1;          
	args.StartVertexLocation = 0;
	args.StartInstanceLocation = 0;

	D3D11_SUBRESOURCE_DATA srd = {};
	srd.pSysMem = &args;
	device->CreateBuffer(&desc, &srd, indirectArgsBuffer.GetAddressOf());

	camDataBuffer = bufferManager->createConstDynamicBufferCPUWrite(camData);

	computeSimulateShader = shaderManager->compileCreateComputeShader(L"./shaders/fireSimulateComputeShader.hlsl", "CSMain", "cs_5_0", nullptr);
	computeEmitShader = shaderManager->compileCreateComputeShader(L"./shaders/fireEmitComputeShader.hlsl", "CSMain", "cs_5_0", nullptr);
	vertexShader = shaderManager->compileCreateShader<ID3D11VertexShader>(device, L"./shaders/fireVertexShader.hlsl", "VSMain", "vs_5_0", nullptr);
	geometryShader = shaderManager->compileCreateShader<ID3D11GeometryShader>(device, L"./shaders/fireGeometryShader.hlsl", "GSMain", "gs_5_0", nullptr);
	pixelShader = shaderManager->compileCreateShader<ID3D11PixelShader>(device, L"./shaders/firePixelShader.hlsl", "PSMain", "ps_5_0", nullptr);
}

void FireParticleSystem::initParticle(int index)
{

}
