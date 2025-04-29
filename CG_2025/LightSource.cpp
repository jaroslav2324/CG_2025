#include "LightSource.h"
#include "global.h"
#include "AABB.h"
#include "BufferManager.h"

using namespace DirectX::SimpleMath;

void LightSource::init()
{
	shMap.init();

	auto bufferManager = GE::getBufferManager();
	if (ls.sourceType == POINT_LIGHT) {
		AABB box = getAABBForPointLight(ls);
		std::vector<Vector4> vertices = generateAABBVertices(box);
		std:: vector<int> indicies = generateAABBIndices();
		vertexBufferPointSpot = bufferManager->createVertexBuffer(vertices);
		indexBufferPointSpot = bufferManager->createIndexBuffer(indicies);
	}
	else if (ls.sourceType == SPOT_LIGHT) {
		AABB box = getAABBForSpotLight(ls);
		std::vector<Vector4> vertices = generateAABBVertices(box);
		std::vector<int> indicies = generateAABBIndices();
		vertexBufferPointSpot = bufferManager->createVertexBuffer(vertices);
		indexBufferPointSpot = bufferManager->createIndexBuffer(indicies);
	}

	additionalConstBuffer = bufferManager->createConstDynamicBufferCPUWrite(addDeferredData);
	lightSourceDataConstBuffer = bufferManager->createConstDynamicBufferCPUWrite(ls);
	shadowMapDataBuffer = bufferManager->createConstDynamicBufferCPUWrite(bindedShadowMapData);
}

void LightSource::mapAdditionalConstBuffer()
{
	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawAdditionalBuffer = additionalConstBuffer.Get();
	context->Map(rawAdditionalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &addDeferredData, sizeof(addDeferredData));
	context->Unmap(rawAdditionalBuffer, 0);
}

void LightSource::mapLightSourceDataConstBuffer()
{
	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawLSDataBuffer = lightSourceDataConstBuffer.Get();
	context->Map(rawLSDataBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &ls, sizeof(ls));
	context->Unmap(rawLSDataBuffer, 0);
}

void LightSource::mapShadowMap()
{
	bindedShadowMapData = shMap.data;
	bindedShadowMapData.viewMatrix = shMap.data.viewMatrix.Transpose();
	for (int j = 0; j < cascadeCount; j++) {
		bindedShadowMapData.projectionMatrices[j] = shMap.data.projectionMatrices[j].Transpose();
	}

	auto deviceContext = GE::getGameSubsystem()->getDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	deviceContext->Map(shadowMapDataBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	auto dataPtr = reinterpret_cast<float*>(mappedResource.pData);
	memcpy(dataPtr, &bindedShadowMapData, sizeof(bindedShadowMapData) );
	deviceContext->Unmap(shadowMapDataBuffer.Get(), 0);
}
