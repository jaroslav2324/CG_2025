#include "global.h"
#include "WindowHandler.h"

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

const std::shared_ptr<RenderSubsystem> GE::getRenderSubsystem()
{
	assert(renderSubsystem);
	return renderSubsystem;
}

static Matrix projectionMatrix;
const Matrix GE::getProjectionMatrix()
{
	return projectionMatrix;
}

Vector3 cameraForwardVector = Vector3(0.0f, 0.0f, 1.0f);
Vector3 cameraPosition = Vector3(0.0f, 0.0f, -1.0f);
Vector3 upCameraVector = Vector3(0.0f, 1.0f, 0.0f);

const Matrix GE::getCameraViewMatrix()
{
	Matrix cameraMatrix = Matrix::CreateLookAt(cameraPosition, cameraPosition + 3.4f * cameraForwardVector, upCameraVector);
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

Vector3 GE::getCameraForwardVector()
{
	return cameraForwardVector;
}

void GE::setCameraPosition(Vector3 pos)
{
	cameraPosition = pos;
}

void GE::setCameraForwardVector(Vector3 vec)
{
	vec.Normalize();
	cameraForwardVector = vec;
}

void GE::setCameraUpVector(Vector3 vec)
{
	vec.Normalize();
	upCameraVector = vec;
}

void GE::rotateCamera(Matrix rotationMatrix)
{
	//cameraPosition = Vector3::Transform(cameraPosition, rotationMatrix);
	Vector3 tmpUp = Vector3::Transform(upCameraVector, rotationMatrix);
	tmpUp.Normalize();
	Vector3 tmpForward = Vector3::Transform(cameraForwardVector, rotationMatrix);
	tmpForward.Normalize();
	upCameraVector = tmpUp;
	cameraForwardVector = tmpForward;
}

void GE::rotateCameraAroundCenter(Vector3 camPos, Matrix rotationMatrix)
{

	Vector3 tmpPos = Vector3::Transform(cameraPosition - camPos, rotationMatrix) + camPos;
	Vector3 tmpUp = Vector3::Transform(upCameraVector, rotationMatrix);
	tmpUp.Normalize();
	Vector3 tmpForward = Vector3::Transform(cameraForwardVector, rotationMatrix);
	tmpForward.Normalize();
	if (tmpUp.Dot(Vector3(0, 1, 0)) > 0.9 || tmpUp.Dot(Vector3(0, -1.0, 0.0)) > 0.5) {
		return;
	}

	upCameraVector = tmpUp;
	cameraForwardVector = tmpForward;
	cameraPosition = tmpPos;
}

void GE::setPerspectiveMatrix(float fov, float ratio)
{
	float nearPlane = 0.1f;
	float farPlane = 10.0f;
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
	initRenderSubsystem();
	setPerspectiveMatrix();
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

void GE::initRenderSubsystem()
{
	if (!renderSubsystem) {
		renderSubsystem = std::make_shared<RenderSubsystem>();
	}
}
