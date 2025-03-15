#pragma once

#include <vector>
#include <DirectXMath.h>

#include <directxtk/DDSTextureLoader.h>

#include "Face.h"
#include "MeshComponent.h"
#include "addData.h"
#include "ModelImporter.h"
#include "Vertex.h"

class CatamariBox;

class CatamariBall : public MeshComponent {
public:
	using Vector2 = DirectX::SimpleMath::Vector2;
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Vector4 = DirectX::SimpleMath::Vector4;
	using Quaternion = DirectX::SimpleMath::Quaternion;
	using Matrix = DirectX::SimpleMath::Matrix;
	CatamariBall(DirectX::SimpleMath::Vector3 position, float radius, int stacks, int slices);

	int init(const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;
	int draw(float deltaTime) override;
	int update(float deltaTime) override;
	void destroyResources() override;

	void setColor(DirectX::XMFLOAT4 clr);

	void initTexturedObject(const std::string& modelPath);

	void moveBall(Vector3 shiftVec, Vector3 rotationAxis, float rotationAngle);

	void attachObject(CatamariBox* box);
	float getRadius() const;
	Vector3 getPosition() const;
	void setPosition(Vector3 pos);
	Quaternion getRotation() const;
	void setRotation(Quaternion rotAngles);

protected:
	std::vector<CatamariBox*> attachedObjects;
	float radius = 1.0f;
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Matrix scale = Matrix::CreateScale(0.01);
	Quaternion qRot;

	ComPtr<ID3D11Buffer> additionalBuffer = nullptr;
	AdditionalData addData;

	bool texturedModelLoaded = false;

	std::vector<Vertex> vertexBufferData;
	std::vector<unsigned int> indexBufferData;

	ComPtr<ID3D11Buffer> modelVerticesBuffer = nullptr;
	ComPtr<ID3D11Buffer> modelIndiciesBuffer = nullptr;
};
