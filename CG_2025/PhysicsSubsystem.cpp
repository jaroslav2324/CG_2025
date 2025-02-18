
#include <iostream>
#include "PhysicsSubsystem.h"

using namespace JPH;
using namespace JPH::literals;
using std::cout;
using std::endl;

static void TraceImpl(const char* inFMT, ...)
{
	// Format the message
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	// Print to the TTY
	cout << buffer << endl;
}

#ifdef JPH_ENABLE_ASSERTS

// Callback for asserts, connect this to your own assert handler if you have one
static bool AssertFailedImpl(const char* inExpression, const char* inMessage, const char* inFile, uint inLine)
{
	// Print to the TTY
	cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage != nullptr ? inMessage : "") << endl;

	// Breakpoint
	return true;
};

#endif // JPH_ENABLE_ASSERTS

bool ObjectLayerPairFilterImpl::ShouldCollide(ObjectLayer inObject1, ObjectLayer inObject2) const
{
	switch (inObject1)
	{
	case Layers::NON_MOVING:
		return inObject2 == Layers::MOVING; // Non moving only collides with moving
	case Layers::MOVING:
		return true; // Moving collides with everything
	default:
		JPH_ASSERT(false);
		return false;
	}
}

void MyBodyActivationListener::OnBodyActivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
{
	std::cout << "A body got activated" << std::endl;
}

void MyBodyActivationListener::OnBodyDeactivated(const JPH::BodyID& inBodyID, uint64 inBodyUserData)
{
	std::cout << "A body went to sleep" << std::endl;
}

// See: ContactListener
ValidateResult MyContactListener::OnContactValidate(const Body& inBody1, const Body& inBody2, RVec3Arg inBaseOffset, const CollideShapeResult& inCollisionResult) {
	std::cout << "Contact validate callback" << std::endl;

	// Allows you to ignore a contact before it is created (using layers to not make objects collide is cheaper!)
	return ValidateResult::AcceptAllContactsForThisBodyPair;
}

void MyContactListener::OnContactAdded(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	std::cout << "A contact was added" << std::endl;
}

void MyContactListener::OnContactPersisted(const Body& inBody1, const Body& inBody2, const ContactManifold& inManifold, ContactSettings& ioSettings)
{
	std::cout << "A contact was persisted" << std::endl;
}

void MyContactListener::OnContactRemoved(const SubShapeIDPair& inSubShapePair)
{
	std::cout << "A contact was removed" << std::endl;
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* BPLayerInterfaceImpl::GetBroadPhaseLayerName(BroadPhaseLayer inLayer) const
{
	switch ((BroadPhaseLayer::Type)inLayer)
	{
	case (BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:	return "NON_MOVING";
	case (BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:		return "MOVING";
	default:													JPH_ASSERT(false); return "INVALID";
	}
}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

BPLayerInterfaceImpl::BPLayerInterfaceImpl()
{
	// Create a mapping table from object to broad phase layer
	mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
	mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
}

uint					BPLayerInterfaceImpl::GetNumBroadPhaseLayers() const
{
	return BroadPhaseLayers::NUM_LAYERS;
}

BroadPhaseLayer			BPLayerInterfaceImpl::GetBroadPhaseLayer(ObjectLayer inLayer) const
{
	JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
	return mObjectToBroadPhase[inLayer];
}

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(ObjectLayer inLayer1, BroadPhaseLayer inLayer2) const
{
	switch (inLayer1)
	{
	case Layers::NON_MOVING:
		return inLayer2 == BroadPhaseLayers::MOVING;
	case Layers::MOVING:
		return true;
	default:
		JPH_ASSERT(false);
		return false;
	}
}

PhysicsSubsystem::PhysicsSubsystem()
{
	RegisterDefaultAllocator();

	// Install trace and assert callbacks
	Trace = TraceImpl;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = AssertFailedImpl;)

		Factory::sInstance = new Factory();
	RegisterTypes();
	tempAllocator = std::make_shared<TempAllocatorImpl>(10 * 1024 * 1024);
	jobSystem = std::make_shared<JobSystemThreadPool>(cMaxPhysicsJobs, cMaxPhysicsBarriers, thread::hardware_concurrency() - 1);

	const uint cMaxBodies = 1024;
	const uint cNumBodyMutexes = 0;
	const uint cMaxBodyPairs = 1024;
	const uint cMaxContactConstraints = 1024;

	physicsSystem.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broadPhaseLayerInterface, objectVsBroadphaseLayerFilter, objectVsObjectLayerFilter);

	physicsSystem.SetBodyActivationListener((BodyActivationListener*)&bodyActivationListener);
	physicsSystem.SetContactListener((ContactListener*)&contactListener);

	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

	BoxShapeSettings floor_shape_settings(Vec3(100.0f, 1.0f, 100.0f));
	floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

	// Create the shape
	ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

	BodyCreationSettings floor_settings(floor_shape, RVec3(0.0_r, -1.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	floor = bodyInterface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

	// Add it to the world
	bodyInterface.AddBody(floor->GetID(), EActivation::DontActivate);
	BodyCreationSettings sphere_settings(new SphereShape(0.5f), RVec3(0.0_r, 2.0_r, 0.0_r), Quat::sIdentity(), EMotionType::Dynamic, Layers::MOVING);
	sphere_id = bodyInterface.CreateAndAddBody(sphere_settings, EActivation::Activate);

	bodyInterface.SetLinearVelocity(sphere_id, Vec3(0.0f, -5.0f, 0.0f));

	physicsSystem.OptimizeBroadPhase();
}

PhysicsSubsystem::~PhysicsSubsystem()
{
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

	bodyInterface.RemoveBody(sphere_id);
	bodyInterface.DestroyBody(sphere_id);

	// Remove and destroy the floor
	bodyInterface.RemoveBody(floor->GetID());
	bodyInterface.DestroyBody(floor->GetID());

	// Unregisters all types with the factory and cleans up the default material
	UnregisterTypes();

	// Destroy the factory
	delete Factory::sInstance;
	Factory::sInstance = nullptr;
}

void PhysicsSubsystem::updatePhysics(float deltaTime)
{
	BodyInterface& bodyInterface = physicsSystem.GetBodyInterface();

	// Now we're ready to simulate the body, keep simulating until it goes to sleep
	uint step = 0;
	while (bodyInterface.IsActive(sphere_id))
	{
		// Next step
		++step;

		// Output current position and velocity of the sphere
		RVec3 position = bodyInterface.GetCenterOfMassPosition(sphere_id);
		Vec3 velocity = bodyInterface.GetLinearVelocity(sphere_id);
		cout << "Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << endl;

		// If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
		const int cCollisionSteps = 1;

		// Step the world
		physicsSystem.Update(deltaTime, cCollisionSteps, (TempAllocator*)&tempAllocator, (JobSystem*)&jobSystem);
	}
}

void PhysicsSubsystem::addComponent(std::shared_ptr<PhysicsComponent> component)
{
	components.push_back(component);
}
