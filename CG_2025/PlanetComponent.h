#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>
#include <directxtk/GeometricPrimitive.h>

#include "MeshComponent.h"
#include "global.h"

struct AdditionalData {
	DirectX::SimpleMath::Matrix transformMatrix;
	DirectX::SimpleMath::Vector2 screenCoords;
	DirectX::SimpleMath::Vector2 unused;

};

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

	void setAngularSpeedSelf(float speed) { angularSpeedSelf = speed; };
	void setAngularSpeedAroundParent(float speed) { angularSpeedAroundParent = speed; };

	void setBoxMesh(DirectX::SimpleMath::Vector3 position, Vector3 widthHeightDephths);
	void setSphereMesh(DirectX::SimpleMath::Vector3 position, float radius);

	void setParentPlanet(PlanetComponent* parent) { parentPlanet = parent; };
	PlanetComponent* getParentPlanet() { return parentPlanet; };
	DirectX::SimpleMath::Matrix getTransformMatrix() {
		return scaleMatrix * rotationMatrix * translationMatrix;
	};
protected:
	void rotateAroundParent();
	PlanetComponent* parentPlanet = nullptr;
	DirectX::SimpleMath::Matrix translationMatrix;
	DirectX::SimpleMath::Matrix rotationMatrix;
	DirectX::SimpleMath::Matrix scaleMatrix;
	DirectX::SimpleMath::Matrix rotationAroundParentMatrix;
	DirectX::SimpleMath::Matrix initialTranslationMatrix;
	DirectX::SimpleMath::Vector3 planetAxis = { 0.0f, 1.0f, 0.0f };
	Vector3 rotationAroundParentAxis = { 0.0f, 1.0f, 0.0f };
	float angularSpeedSelf = 0.1;
	float angularSpeedAroundParent = 0.2f;
	float totalTime = 0.0f;

	ComPtr<ID3D11Buffer> additionalBuffer = nullptr;
	AdditionalData addData;
};