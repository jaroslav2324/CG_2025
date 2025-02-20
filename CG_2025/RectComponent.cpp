#include "RectComponent.h"

using Vector2 = DirectX::SimpleMath::Vector2;


int RectComponent::update(float deltaTime)
{
	//TODO: update grahics buffer from rects
	return 0;
}

void RectComponent::initRect(int x, int y, int w, int h)
{
	if (rect)
		return;

	rect = std::make_shared<DirectX::SimpleMath::Rectangle>(
		DirectX::SimpleMath::Rectangle(x, y, w, h)
	);


}

void RectComponent::setDirection(Vector2 direction)
{
	this->direction = direction;
}

void RectComponent::setPosition(Vector2 pos)
{
	this->centerPosition = pos;
}

void RectComponent::setVelocity(float vel)
{
	velocity = vel;
}

Vector2 RectComponent::getDirection()
{
	return direction;
}

Vector2 RectComponent::getPosition()
{
	return centerPosition;
}

float RectComponent::getVelocity()
{
	return velocity;
}

std::shared_ptr<DirectX::SimpleMath::Rectangle> RectComponent::getRect()
{
	return rect;
}

