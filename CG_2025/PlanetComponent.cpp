#include "PlanetComponent.h"

using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Quaternion;

int PlanetComponent::init(const std::wstring& vertShaderPath, const std::wstring& pixShaderPath)
{
	MeshComponent::init(vertShaderPath, pixShaderPath);
	auto bufferManager = GE::getBufferManager();
	layout = bufferManager->createInputLayout_PosF4_ClrF4(vertexByteCode);
	vertexBuffer = bufferManager->createVertexBuffer(points);
	indexBuffer = bufferManager->createIndexBuffer(indices);
	additionalBuffer = bufferManager->createConstDynamicBufferCPUWrite(addData);
	return 0;
}

PlanetComponent::PlanetComponent(Vector3 position, float radius, Vector3 color) : MeshComponent()
{
	planetAxis.Normalize();
	translationMatrix = translationMatrix.CreateTranslation(position);
	initialTranslationMatrix = translationMatrix;

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
			float theta = DirectX::XM_2PI * j / slices;

			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);
			//std::cout << x << " " << y << " " << z << " " << std::endl;

			points.push_back({ x, y, z, 1.0f });
			points.push_back({ color.x, color.y, color.z, 1.0f });
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
	//totalTime += deltaTime;
	//if (totalTime >= 1.0f) {
	//	totalTime -= 1.0f;
	//}
	//if (parentPlanet != nullptr) {
	//	summedAroundParentAngle += angularSpeedAroundParent * deltaTime * 6;
	//	if (summedAroundParentAngle > 360.0) { summedAroundParentAngle -= 360.0; }
	//	if (summedAroundParentAngle < 0) { summedAroundParentAngle += 360.0; }
	//	rotationAroundParentMatrix = Matrix::CreateFromAxisAngle(rotationAroundParentAxis, summedAroundParentAngle);
	//	translationMatrix = initialTranslationMatrix * rotationAroundParentMatrix;
	//}

	//rotationMatrix *= Matrix::CreateFromAxisAngle(planetAxis, angularSpeedSelf * deltaTime * DirectX::XM_2PI);
	//Matrix transformMatrix = scaleMatrix * rotationMatrix *
	//	rotationAroundParentMatrix.Invert() * initialTranslationMatrix * rotationAroundParentMatrix;
	//PlanetComponent* parent = parentPlanet;
	//while (parent != nullptr) {
	//	transformMatrix = transformMatrix * parent->initialTranslationMatrix * parent->rotationAroundParentMatrix; //parent->translationMatrix;
	//	parent = parent->parentPlanet;
	//}
	//Matrix transformMatrix = scaleMatrix * rotationMatrix * initialTranslationMatrix;
	Matrix transformMatrix = scaleMatrix * rotationMatrix * translationMatrix;
	transformMatrix = transformMatrix * GE::getCameraViewMatrix() * GE::getProjectionMatrix();
	addData.transformMatrix = transformMatrix.Transpose();

	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawAdditionalBuffer = additionalBuffer.Get();
	context->Map(rawAdditionalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &addData, sizeof(addData));
	context->Unmap(rawAdditionalBuffer, 0);

	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	context->VSSetConstantBuffers(0, 1, &rawAdditionalBuffer);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);

	context->DrawIndexed(indices.size(), 0, 0);
	return 0;
}

int PlanetComponent::update(float deltaTime)
{
	return 0;
}

void PlanetComponent::destroyResources()
{
	MeshComponent::destroyResources();
}

void PlanetComponent::setPosition(Vector3 pos)
{
	translationMatrix = translationMatrix.CreateTranslation(pos);
}

void PlanetComponent::setPlanetAxis(Vector3 axis)
{
	axis.Normalize();
	planetAxis = axis;
}

void PlanetComponent::setRotationAroundParentAxis(Vector3 axis)
{
	axis.Normalize();
	rotationAroundParentAxis = axis;
}

void PlanetComponent::setBoxMesh(DirectX::SimpleMath::Vector3 position, Vector3 widthHeightDephths)
{
	translationMatrix = translationMatrix.CreateTranslation(position);
	initialTranslationMatrix = translationMatrix;

	indices.clear();
	points.clear();
	strides.clear();
	offsets.clear();

	float halfWidth = widthHeightDephths.x * 0.5f;
	float halfHeight = widthHeightDephths.y * 0.5f;
	float halfDepth = widthHeightDephths.z * 0.5f;

	std::vector<DirectX::SimpleMath::Vector4> vertices = {
		{ -halfWidth, -halfHeight,  halfDepth, 1.0f },
		{  halfWidth, -halfHeight,  halfDepth, 1.0f },
		{  halfWidth,  halfHeight,  halfDepth, 1.0f },
		{ -halfWidth,  halfHeight,  halfDepth, 1.0f },
		{ -halfWidth, -halfHeight, -halfDepth, 1.0f },
		{  halfWidth, -halfHeight, -halfDepth, 1.0f },
		{  halfWidth,  halfHeight, -halfDepth, 1.0f },
		{ -halfWidth,  halfHeight, -halfDepth, 1.0f }
	};

	for (const auto& vertex : vertices) {
		points.push_back(vertex);
		points.push_back({ 0.5f, 1.0f, 1.0f, 1.0f });
	}

	indices = {
		0, 1, 2,  0, 2, 3,
		4, 6, 5,  4, 7, 6,
		0, 4, 5,  0, 5, 1,
		3, 2, 6,  3, 6, 7,
		0, 3, 7,  0, 7, 4,
		1, 5, 6,  1, 6, 2
	};

	strides = { 32 };
	offsets = { 0 };

	init(L"./shaders/planetShader.hlsl",
		L"./shaders/pixelShader.hlsl");
}

void PlanetComponent::setSphereMesh(DirectX::SimpleMath::Vector3 position, float radius, int stacks, int slices)
{
	translationMatrix = translationMatrix.CreateTranslation(position);
	initialTranslationMatrix = translationMatrix;

	indices.clear();
	points.clear();

	auto context = GE::getGameSubsystem()->getDeviceContext();

	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();
	addData.screenCoords.x = winWidth;
	addData.screenCoords.y = winHeight;

	for (int i = 0; i <= stacks; ++i) {
		float phi = DirectX::XM_PI * i / stacks;
		for (int j = 0; j <= slices; ++j) {
			float theta = DirectX::XM_2PI * j / slices; // От 0 до 2PI

			float x = radius * sinf(phi) * cosf(theta);
			float y = radius * cosf(phi);
			float z = radius * sinf(phi) * sinf(theta);

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

	init(L"./shaders/planetShader.hlsl",
		L"./shaders/pixelShader.hlsl");
}

void PlanetComponent::setColor(DirectX::XMFLOAT4 clr)
{
	int size = points.size();
	for (int i = 0; i < size; i++) {
		if (i % 2 == 1) {
			auto& p = points[i];
			p.x = clr.x;
			p.y = clr.y;
			p.z = clr.z;
		}
	}
	init(L"./shaders/planetShader.hlsl",
		L"./shaders/pixelShader.hlsl");
}

void PlanetComponent::setStarsColor(float probability)
{
	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_real_distribution<double> dist(0.0, 1.0);

	int size = points.size();
	for (int i = 0; i < size; i++) {
		if (i % 2 == 1) {
			auto& p = points[i];
			if (dist(gen) > probability) {
				p.x = 1.0f;
				p.y = 1.0f;
				p.z = 1.0f;
			}
			else {
				p.x = 0.0f;
				p.y = 0.0f;
				p.z = 0.0f;
			}

		}
	}
	init(L"./shaders/planetShader.hlsl",
		L"./shaders/pixelShader.hlsl");
}
