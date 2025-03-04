#include "global.h"

using DirectX::SimpleMath::Matrix;
using DirectX::SimpleMath::Vector3;

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

static Matrix projectionMatrix;
const Matrix GE::getProjectionMatrix()
{
	return projectionMatrix;
}

Vector3 cameraPosition = Vector3(0.0f, 0.0f, -1.0f);
Vector3 upCameraVector = Vector3(0.0f, 1.0f, 0.0f);

const Matrix GE::getCameraViewMatrix()
{
	Vector3 centerPos = Vector3(0.0f, 0.0f, 0.0f);

	Matrix cameraMatrix = Matrix::CreateLookAt(cameraPosition, centerPos, upCameraVector);
	return cameraMatrix;
}

Vector3 GE::getCameraPosition()
{
	return cameraPosition;
}

Vector3 GE::getCameraUpVector()
{
	return upCameraVector;
}

void GE::setCameraPosition(Vector3 pos)
{
	cameraPosition = pos;
}

void GE::rotateCameraAroundCenter(Matrix rotationMatrix)
{
	cameraPosition = Vector3::Transform(cameraPosition, rotationMatrix);
	Vector3 tmpUp = Vector3::Transform(upCameraVector, rotationMatrix);
	tmpUp.Normalize();
	upCameraVector = tmpUp;
}

void GE::setPerspectiveMatrix(float fov, float ratio)
{
	float nearPlane = 1.0f;
	float farPlane = 1000.0f;
	projectionMatrix = Matrix::CreatePerspectiveFieldOfView(fov, ratio, nearPlane, farPlane);
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
