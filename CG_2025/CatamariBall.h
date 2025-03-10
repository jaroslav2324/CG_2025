#pragma once


#include <vector>
#include <DirectXMath.h>

#include "MeshComponent.h"
#include "addData.h"

class CatamariBox;

class CatamariBall : public MeshComponent {
public:
	using Vector3 = DirectX::SimpleMath::Vector3;
	using Quaternion = DirectX::SimpleMath::Quaternion;
	CatamariBall(DirectX::SimpleMath::Vector3 position, float radius, int stacks, int slices);

	int init(const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;
	int draw(float deltaTime) override;
	int update(float deltaTime) override;
	void destroyResources() override;

	void setColor(DirectX::XMFLOAT4 clr);

	void moveBall(Vector3 shiftVec, Vector3 rotationAxis, float rotationAngle);

	void attachObject(CatamariBox* box);
	float getRadius() const;
	Vector3 getPosition() const;
	void setPosition(Vector3 pos);
	Quaternion getRotation() const;
	void setRotation(Quaternion rotAngles);

protected:
	std::vector<CatamariBox*> attachedObjects; // Список приклеенных объектов
	float radius = 1.0f;                        // Радиус шара
	Vector3 position = { 0.0f, 0.0f, 0.0f }; // Позиция шара в мире
	//Vector3 rotation = { 0.0f, 0.0f, 0.0f }; // Вращение шара (Эйлеровы углы)
	Quaternion qRot;
	//Vector3 forward = {};

	ComPtr<ID3D11Buffer> additionalBuffer = nullptr;
	AdditionalData addData;
};
