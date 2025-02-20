#pragma once

#include <map>
#include <memory>
#include <vector>
#include <cstdarg>

#include <directxtk/SimpleMath.h>

#include "util.h"

class RectComponent;

class PhysicsSubsystem
{
public:
	PhysicsSubsystem();
	~PhysicsSubsystem();

	void updatePhysics(float deltaTime);
	void registerRect(std::shared_ptr<RectComponent> rect);
	// TODO: unregisterRect

private:
	std::vector<std::shared_ptr<RectComponent>> rectsVec;
};