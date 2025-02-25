
#include <iostream>
#include "PhysicsSubsystem.h"
#include "RectComponent.h"

using namespace DirectX::SimpleMath;

PhysicsSubsystem::PhysicsSubsystem()
{

}

PhysicsSubsystem::~PhysicsSubsystem()
{

}

void PhysicsSubsystem::updatePhysics(float deltaTime)
{
	// collision detection
	for (auto rectPtr : rectsVec) {
		for (auto rectPtr2 : rectsVec) {
			if (rectPtr == rectPtr2) {
				continue;
			}
			auto dxrect1 = rectPtr->getRect();
			auto dxrect2 = rectPtr2->getRect();

			auto intersectionRect = dxrect1.Intersect(dxrect1, dxrect2);
			if (intersectionRect.width || intersectionRect.height) {
				// collided, calculate collision normal and call collision callback
				Vector2 collisionNormal;
				if (intersectionRect.height >= intersectionRect.width) {
					collisionNormal = { -1, 0 };
					if (rectPtr->getPosition().x > rectPtr2->getPosition().x) {
						collisionNormal *= -1;
					}
				}
				else {
					collisionNormal = { 0, 1 };
					if (rectPtr->getPosition().y > rectPtr2->getPosition().y) {
						collisionNormal *= -1;
					}
				}
				rectPtr->callCollisionCallback(rectPtr, collisionNormal);
				collisionNormal *= -1;
				rectPtr2->callCollisionCallback(rectPtr2, collisionNormal);
			}
		}
	}

	// moving
	for (auto rectPtr : rectsVec) {
		float vel = rectPtr->getVelocity();
		if (!apprEqual(vel, 0.0f)) {
			Vector2 pos = rectPtr->getPosition();
			pos += vel * rectPtr->getDirection() * deltaTime;
			rectPtr->setPosition(pos);
		}
	}
}

void PhysicsSubsystem::registerRect(RectComponent* rect)
{
	if (std::find(rectsVec.begin(), rectsVec.end(), rect) == rectsVec.end())
		rectsVec.push_back(rect);
}


