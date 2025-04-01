#pragma once

#include <algorithm>
#include <DirectXMath.h>
#include <directxtk/DDSTextureLoader.h>

#include "MeshComponent.h"
#include "addData.h"
#include "Face.h"
#include "Vertex.h"

class CatamariBall;

class CatamariBox : public MeshComponent {
public:
	using Vector2 = DirectX::SimpleMath::Vector2;
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Vector4 = DirectX::SimpleMath::Vector4;
	using Matrix = DirectX::SimpleMath::Matrix;
	using Quaternion = DirectX::SimpleMath::Quaternion;
	friend class Game;

	CatamariBox(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size);
	int init(const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;
	int draw(float deltaTime) override;
	int update(float deltaTime) override;
	void destroyResources() override;

	void initTexturedObject(const std::string& modelPath);
	void setTexture(const std::wstring& path);

	bool isAttached() const;
	void setAttached(CatamariBall* ball, bool flag = true);

	Vector3 getLowestPoint();

	Vector3 getSize() const;
	Vector3 getPosition() const;
	void setPosition(Vector3 pos);
	Quaternion getRotation() const;
	void setRotation(Quaternion rotAngles);
	void rotateAttached(Matrix rot);

	void setModelPath(const std::string& path);
	void setTexturePath(const std::wstring& path);

	DirectX::BoundingBox getAABB() const;

protected:
	bool attached = false; 
	Vector3 size ; 
	Matrix scale = Matrix::CreateScale(0.3f);
	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Quaternion rotation;
	Vector3 attachedOffset = { 0, 0 ,0 };

	ComPtr<ID3D11Buffer> additionalBuffer = nullptr;
	AdditionalData addData;
	CatamariBall* ball = nullptr;

	bool texturedModelLoaded = false;

	std::vector<Vertex> vertexBufferData;
	std::vector<unsigned int> indexBufferData;

	ComPtr<ID3D11Buffer> modelVerticesBuffer = nullptr;
	ComPtr<ID3D11Buffer> modelIndiciesBuffer = nullptr;
	ID3D11ShaderResourceView* texture = nullptr;

	Material material;

	std::string modelPath = "./models/cube.obj";
	std::wstring texturePath = L"./models/wood.dds";
};