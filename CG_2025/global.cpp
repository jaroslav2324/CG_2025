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

const std::shared_ptr<Game> GE::getGameSubsystem()
{
	assert(gameSubsystem);
	return gameSubsystem;
}

const std::shared_ptr<ShaderManager> GE::getShaderManager()
{
	assert(shaderManager);
	return shaderManager;
}

const std::shared_ptr<BufferManager> GE::getBufferManager()
{
	assert(bufferManager);
	return bufferManager;
}

void GE::initGraphicsEngine()
{
	initWindowHandler();
	initGameSubsystem();
	initBufferManager();
	initShaderManager();
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

void GE::initGameSubsystem()
{
	if (!gameSubsystem) {
		gameSubsystem = std::make_shared<Game>();
	}
}

void GE::initShaderManager()
{
	if (!shaderManager) {
		shaderManager = std::make_shared<ShaderManager>();
	}
}

void GE::initBufferManager()
{
	if (!bufferManager) {
		bufferManager = std::make_shared<BufferManager>();
	}
}
