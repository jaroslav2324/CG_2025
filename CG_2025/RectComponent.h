
#pragma once

#include <array>
#include <functional>
#include <chrono>
#include <directxtk/SimpleMath.h>

#include "global.h"
#include "MeshComponent.h"
#include "RectF.h"

class RectComponent : public MeshComponent {
public:
	using Vector2 = DirectX::SimpleMath::Vector2;

	RectComponent(std::vector<DirectX::XMFLOAT4>&& points,
		std::vector<UINT>&& strides,
		std::vector<UINT>&& offsets,
		std::vector<int>&& indices);
	RectComponent(Vector2 centerPoint, float width, float height);
	//TODO: ~RectCompunent(): unregister rect in physics system
	//TODO: update physics

	int init(const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;

	int update(float deltaTime) override;
	int draw() override;

	void setDirection(Vector2 direction);
	void setPosition(Vector2 pos);
	void setVelocity(float vel);
	void setCollisionCallback(RectComponent* rectComponent,
		std::function <void(RectComponent*, RectComponent*, DirectX::SimpleMath::Vector2)>&& callback);
	void callCollisionCallback(RectComponent* rectComponent, RectComponent* rect2, Vector2 collisionNormal) const;
	Vector2 getDirection() const;
	Vector2 getPosition() const;
	float getVelocity() const;
	DirectX::SimpleMath::Rectangle getRect();

	bool collided();
	void setCollided(bool flag);

	void setDrawableCircle();

private:
	bool collisionCallbackSet = false;
	std::function <void(RectComponent*, RectComponent*, DirectX::SimpleMath::Vector2)> collisionCallback = nullptr;
	RectF rect = { 0,0, 0, 0 };
	Vector2 direction = { 1, 0 };
	float velocity = 0;
	Vector2 centerPosition = { 0, 0 };
	Vector2 startCenterPosition = { 0, 0 };
	DirectX::XMFLOAT4 centerPointOffset = { 0.0f, 0.0f, 0.0f, 0.0f };
	ComPtr <ID3D11Buffer> centerOffsetBuffer = nullptr;
	float width = 0;
	float height = 0;

	// очередной костыль
	bool collidedFlag = false;
	std::chrono::time_point<std::chrono::steady_clock> lastCollisionTime;
	int msUntilCollisionFlagReset = 500;

};