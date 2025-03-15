#pragma once

#include <algorithm>
#include <DirectXMath.h>

#include "MeshComponent.h"
#include "addData.h"
#include "Face.h"

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

	void initTexturedObject(const std::wstring& modelPath);

	void setColor(DirectX::XMFLOAT4 clr);

	bool isAttached() const;
	void setAttached(CatamariBall* ball, bool flag = true);

	Vector3 getLowestPoint();

	Vector3 getSize() const;
	Vector3 getPosition() const;
	void setPosition(Vector3 pos);
	Quaternion getRotation() const;
	void setRotation(Quaternion rotAngles);
	void rotateAttached(Matrix rot);

	DirectX::BoundingBox getAABB() const;

protected:
	bool attached = false; // Флаг приклеенного объекта
	Vector3 size = { 1.0f, 1.0f, 1.0f }; // Длина, ширина, высота
	Vector3 position = { 0.0f, 0.0f, 0.0f }; // Позиция в мире
	Quaternion rotation;
	Vector3 attachedOffset = { 0, 0 ,0 };

	ComPtr<ID3D11Buffer> additionalBuffer = nullptr;
	AdditionalData addData;
	CatamariBall* ball = nullptr;

	bool drawDebugCollider = true;

	bool texturedModelSet = false;
	std::unique_ptr<DirectX::Model> model;
	std::unique_ptr<DirectX::IEffectFactory> fxFactory;
	std::unique_ptr<DirectX::CommonStates> states;

	std::vector<Vector4> cornersPoints;
	ComPtr<ID3D11Buffer> debugVertexBuffer;
	ComPtr<ID3D11Buffer> debugIndexBuffer;

	std::vector<Vector3> modelVertices;
	std::vector<Vector2> modelTexCoords;
	std::vector<Vector3> modelNormals;
	std::vector<Face> modelFaces;
};