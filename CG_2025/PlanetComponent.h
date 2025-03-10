#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <directxtk/GeometricPrimitive.h>

#include <ctime>
#include <random>

#include "MeshComponent.h"
#include "global.h"
#include "addData.h"

class PlanetComponent : public MeshComponent {
public:
	using Vector2 = DirectX::SimpleMath::Vector2;
	using Vector3 = DirectX::SimpleMath::Vector3;
	PlanetComponent(DirectX::SimpleMath::Vector3 position, float radius);
	int init(const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;
	int draw(float deltaTime) override;
	int update(float deltaTime) override;
	void destroyResources() override;

	void setPlanetAxis(Vector3 axis);
	void setRotationAroundParentAxis(Vector3 axis);

	void setAngularSpeedSelf(float speed) { angularSpeedSelf = speed; };
	void setAngularSpeedAroundParent(float speed) { angularSpeedAroundParent = speed; };

	void setBoxMesh(DirectX::SimpleMath::Vector3 position, Vector3 widthHeightDephths);
	void setSphereMesh(DirectX::SimpleMath::Vector3 position, float radius, int stacks = 16, int slices = 16);

	void setColor(DirectX::XMFLOAT4 clr);
	void setStarsColor(float probability);

	void setParentPlanet(PlanetComponent* parent) { parentPlanet = parent; };
	PlanetComponent* getParentPlanet() { return parentPlanet; };
	DirectX::SimpleMath::Matrix getTransformMatrix() {
		return scaleMatrix * rotationMatrix * translationMatrix;
	};
protected:
	PlanetComponent* parentPlanet = nullptr;
	DirectX::SimpleMath::Matrix translationMatrix;
	DirectX::SimpleMath::Matrix rotationMatrix;
	DirectX::SimpleMath::Matrix scaleMatrix;
	DirectX::SimpleMath::Matrix rotationAroundParentMatrix;
	DirectX::SimpleMath::Matrix initialrotationAroundParentMatrix;
	DirectX::SimpleMath::Matrix initialTranslationMatrix;
	DirectX::SimpleMath::Vector3 planetAxis = { 0.0f, 1.0f, 0.0f };
	Vector3 rotationAroundParentAxis = { 0.0f, 1.0f, 0.0f };
	float angularSpeedSelf = 0.1;
	float angularSpeedAroundParent = 0.2f;
	float totalTime = 0.0f;
	float summedAroundParentAngle = 0.0f;

	ComPtr<ID3D11Buffer> additionalBuffer = nullptr;
	AdditionalData addData;
};