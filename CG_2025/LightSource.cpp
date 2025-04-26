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

	additionalBuffer = bufferManager->createConstDynamicBufferCPUWrite(addData);
}

void LightSource::mapAdditionalBuffer()
{
	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->Map(rawAdditionalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &addData, sizeof(AdditionalData));
	context->Unmap(rawAdditionalBuffer, 0);
}
