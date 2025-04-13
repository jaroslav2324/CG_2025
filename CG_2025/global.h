#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <wrl/client.h>

#include "InputDevice.h"
#include "PhysicsSubsystem.h"
#include "ShaderManager.h"
#include "BufferManager.h"
#include "Game.h"
#include "RenderSubsystem.h"

using Microsoft::WRL::ComPtr;

class WindowHandler;

//#define PONG
//#define PLANETS
#define CATAMARI

namespace GE {
	using Matrix = DirectX::SimpleMath::Matrix;
	using Vector3 = DirectX::SimpleMath::Vector3;

	static std::shared_ptr<InputDevice> inputDevice; // defaults to nullptr
	static std::shared_ptr<WindowHandler> winHandler; // defaults to nullptr
	static std::shared_ptr<PhysicsSubsystem> physicsSubsystem; // defaults to nullptr
	static std::shared_ptr<Game> gameSubsystem; // defaults to nullptr
	static std::shared_ptr<ShaderManager> shaderManager; // defaults to nullptr
	static std::shared_ptr<BufferManager> bufferManager; // defaults to nullptr
	static std::shared_ptr<RenderSubsystem> renderSubsystem; // defaults to nullptr

	void initGraphicsEngine();
	static void initWindowHandler();
	static void initInputDevice(std::shared_ptr<WindowHandler> winHandler);
	static void initPhysicsSubsystem();
	static void initGameSubsystem();
	static void initShaderManager();
	static void initBufferManager();
	static void initRenderSubsystem();
	
	const std::shared_ptr<WindowHandler> getWindowHandler();
	const std::shared_ptr<InputDevice> getInputDevice();
	const std::shared_ptr<PhysicsSubsystem> getPhysicsSubsystem();
	const std::shared_ptr<Game> getGameSubsystem();
	const std::shared_ptr<ShaderManager> getShaderManager();
	const std::shared_ptr<BufferManager> getBufferManager();
	const std::shared_ptr<RenderSubsystem> getRenderSubsystem();

	const Matrix getProjectionMatrix();
	const Matrix getCameraViewMatrix();
	Vector3 getCameraPosition();
	Vector3 getCameraUpVector();
	Vector3 getCameraForwardVector();
	void setCameraPosition(Vector3 pos);
	void setCameraForwardVector(Vector3 vec);
	void setCameraUpVector(Vector3 vec);
	void rotateCamera(Matrix rotationMatrix);
	void rotateCameraAroundCenter(Vector3 parentPos, Matrix rotationMatrix);
	void setPerspectiveMatrix(float fov = 45.0 / 180.0 * DirectX::XM_PI, float ratio = 1.0);
}
