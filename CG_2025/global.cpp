#include "global.h"

const std::shared_ptr<WindowHandler> GE::getWindowHandler()
{
	assert(winHandler);
	return winHandler;
}

const std::shared_ptr<InputDevice> GE::getInputDevice()
{
	assert(inputDevice);
	return inputDevice;
}

const std::shared_ptr<PhysicsSubsystem> GE::getPhysicsSubsystem()
{
	assert(physicsSubsystem);
	return physicsSubsystem;
}

void GE::initGraphicsEngine()
{
	initWindowHandler();
	initInputDevice(winHandler);
	initPhysicsSubsystem();
}

void GE::initWindowHandler()
{
	if (!winHandler) {
		winHandler = std::make_shared<WindowHandler>();
	}
}

void GE::initInputDevice(std::shared_ptr<WindowHandler> winHandler)
{
	if (!inputDevice) {
		inputDevice = std::make_shared<InputDevice>(winHandler);
	}
}

void GE::initPhysicsSubsystem()
{
	if (!physicsSubsystem) {
		physicsSubsystem = std::make_shared<PhysicsSubsystem>();
	}
}
