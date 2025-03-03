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
	PlanetComponent(DirectX::SimpleMath::Vector3 position, float radius);
	int init(const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;
	int draw(float deltaTime) override;
	int update(float deltaTime) override;
	void destroyResources() override;

	void setBoxMesh();
	void setSphereMesh();

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
	DirectX::SimpleMath::Vector3 planetAxis = { 0.0f, 1.0f, 0.0f };
	float angularSpeed = 0.1;
	float totalTime = 0.0f;

	ComPtr<ID3D11Buffer> additionalBuffer = nullptr;
	AdditionalData addData;
};