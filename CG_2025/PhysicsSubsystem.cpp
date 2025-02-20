
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
			auto dxrect1 = rectPtr->getRect();
			auto dxrect2 = rectPtr2->getRect();
			if (dxrect1->Intersects(*dxrect2.get())) {
				// collided, call collision callback
			}
		}
	}

	// moving
	for (auto rectPtr : rectsVec) {
		float vel = rectPtr->getVelocity();
		if (!apprEqual(vel, 0.0f)) {
			Vector2 pos = rectPtr->getPosition();
			pos += vel * rectPtr->getDirection();
			rectPtr->setPosition(pos);
		}
	}
}

void PhysicsSubsystem::registerRect(std::shared_ptr<RectComponent> rect)
{
	if (std::find(rectsVec.begin(), rectsVec.end(), rect) == rectsVec.end())
		rectsVec.push_back(rect);
}


