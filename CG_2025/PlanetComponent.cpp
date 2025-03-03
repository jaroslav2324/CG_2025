#include "PlanetComponent.h"

using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Matrix;

int PlanetComponent::init(const std::wstring& vertShaderPath, const std::wstring& pixShaderPath)
{
	MeshComponent::init(vertShaderPath, pixShaderPath);
	auto bufferManager = GE::getBufferManager();

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = &addData;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC buffDesc = {};
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;
	buffDesc.ByteWidth = sizeof(addData);

	additionalBuffer = bufferManager->createBuffer(buffDesc, subresourceData);
	return 0;
}

PlanetComponent::PlanetComponent(Vector3 position, float radius) : MeshComponent()
{
	planetAxis.Normalize();
	translationMatrix = translationMatrix.CreateTranslation(position);

	int stacks = 16, slices = 16;

	auto context = GE::getGameSubsystem()->getDeviceContext();

	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();
	//addData.screenCoords.x = 0.5;
	addData.screenCoords.x = winWidth;
	addData.screenCoords.y = winHeight;

	for (int i = 0; i <= stacks; ++i) {
		float phi = DirectX::XM_PI * i / stacks;
		for (int j = 0; j <= slices; ++j) {
			float theta = DirectX::XM_2PI * j / slices; // От 0 до 2PI

			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);
			std::cout << x << " " << y << " " << z << " " << std::endl;

			points.push_back({ x, y, z, 1.0f });
			points.push_back({ 0.5f, 1.0f, 1.0f, 1.0f });
		}
	}

	for (int i = 0; i < stacks; ++i) {
		for (int j = 0; j < slices; ++j) {
			int first = i * (slices + 1) + j;
			int second = first + slices + 1;

			indices.push_back(first);
			indices.push_back(second);
			indices.push_back(first + 1);

			indices.push_back(second);
			indices.push_back(second + 1);
			indices.push_back(first + 1);
		}
	}

	strides = { 32 };
	offsets = { 0 };
}

int PlanetComponent::draw(float deltaTime)
{
	totalTime += deltaTime;
	if (totalTime >= 1.0f) {
		totalTime -= 1.0f;
	}
	rotationMatrix *= Matrix::CreateFromAxisAngle(planetAxis, angularSpeed * deltaTime * DirectX::XM_2PI);
	Matrix transformMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	PlanetComponent* parent = parentPlanet;
	while (parent != nullptr) {
		//transformMatrix *= parent->getTransformMatrix();
		// rotate around parent : 
		transformMatrix *= parentPlanet->translationMatrix;
		transformMatrix *= parent->rotationMatrix;
		parent = parent->parentPlanet;
	}
	transformMatrix *= GE::getCameraViewMatrix() * GE::getPerspectiveMatrix();
	addData.transformMatrix = transformMatrix.Transpose();

	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->VSSetConstantBuffers(0, 1, &rawAdditionalBuffer);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->DrawIndexed(indices.size(), 0, 0);
	return 0;
}

int PlanetComponent::update(float deltaTime)
{
	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->Map(rawAdditionalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &addData, sizeof(AdditionalData));
	context->Unmap(rawAdditionalBuffer, 0);
	return 0;
}

void PlanetComponent::destroyResources()
{
	MeshComponent::destroyResources();
}
