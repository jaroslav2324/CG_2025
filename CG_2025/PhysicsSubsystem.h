#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyInterface.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceTable.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/ObjectVsBroadPhaseLayerFilterTable.h>
#include <Jolt/Physics/Collision/ObjectLayerPairFilterTable.h>
#include <memory>
#include <vector>

class PhysicsComponent;

class PhysicsSubsystem
{
public:
	PhysicsSubsystem();
	~PhysicsSubsystem();

	void updatePhysics(float deltaTime);
	void addComponent(std::shared_ptr<PhysicsComponent> component);

private:
	JPH::PhysicsSystem physicsSystem;
	std::vector<std::shared_ptr<PhysicsComponent>> components;

	std::unique_ptr<JPH::TempAllocator> tempAllocator;
	std::unique_ptr<JPH::JobSystemThreadPool> jobSystem;
};