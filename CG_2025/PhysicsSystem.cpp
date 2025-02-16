
#include <iostream>
#include "PhysicsSubsystem.h"

using namespace JPH;

PhysicsSubsystem::PhysicsSubsystem()
{

	BroadPhaseLayerInterfaceTable broadPhaseLayerInterfaceTable(1, 1);
	BroadPhaseLayerInterface* layerInterface = (BroadPhaseLayerInterface*)(&broadPhaseLayerInterfaceTable);
	ObjectLayerPairFilterTable objectLayerPairFilterTable(1);
	ObjectLayerPairFilter* pairFilter = (ObjectLayerPairFilter*)(&objectLayerPairFilterTable);
	ObjectVsBroadPhaseLayerFilterTable objectVsBroadPhaseLayerFilter(*layerInterface, 1, *pairFilter, 1);
	ObjectVsBroadPhaseLayerFilter* vsBroadPhaseLayerFilter = (ObjectVsBroadPhaseLayerFilter*)(&objectVsBroadPhaseLayerFilter);

	physicsSystem.Init(1024, 0, 1024, 1024, *layerInterface, *vsBroadPhaseLayerFilter, *pairFilter);
	physicsSystem.SetGravity(JPH::Vec3(0, -9.81f, 0));
	jobSystem = std::make_unique<JobSystemThreadPool>(1024, 0);
	tempAllocator = std::make_unique<TempAllocatorImpl>(10 * 1024 * 1024);

	JPH::BoxShapeSettings box_settings(JPH::Vec3(1, 1, 1));
	JPH::ShapeSettings::ShapeResult result = box_settings.Create();
	JPH::BodyCreationSettings body_settings(result.Get(), JPH::Vec3(0, 10, 0), JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, 0);
	JPH::Body* body = physicsSystem.GetBodyInterface().CreateBody(body_settings);
	physicsSystem.GetBodyInterface().AddBody(body->GetID(), JPH::EActivation::Activate);
}

PhysicsSubsystem::~PhysicsSubsystem()
{

}

void PhysicsSubsystem::updatePhysics(float deltaTime)
{

	physicsSystem.Update(deltaTime, 1, tempAllocator.get(), jobSystem.get());

	//for (auto& component : components)
	//{
	//	if (component) component->update();
	//}
}

void PhysicsSubsystem::addComponent(std::shared_ptr<PhysicsComponent> component)
{
	components.push_back(component);
}
