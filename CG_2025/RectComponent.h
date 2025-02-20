#pragma once

#include <functional>
#include <directxtk/SimpleMath.h>

#include "global.h"
#include "MeshComponent.h"

class RectComponent : public MeshComponent {
public:
	using Vector2 = DirectX::SimpleMath::Vector2;

	RectComponent();

	int update(float deltaTime) override;
	void initRect(int x, int y, int w, int h);
	void setDirection(Vector2 direction);
	void setPosition(Vector2 pos);
	void setVelocity(float vel);
	Vector2 getDirection();
	Vector2 getPosition();
	float getVelocity();
	std::shared_ptr<DirectX::SimpleMath::Rectangle> getRect();
private:
	// std::function <void(void)> 
	std::shared_ptr<DirectX::SimpleMath::Rectangle> rect = nullptr;
	Vector2 direction = { 1, 0 };
	float velocity = 0;
	Vector2 centerPosition = { 0, 0 };
};