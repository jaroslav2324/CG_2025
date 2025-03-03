#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <wrl/client.h>

#include "WindowHandler.h"
#include "InputDevice.h"
#include "PhysicsSubsystem.h"
#include "ShaderManager.h"
#include "BufferManager.h"
#include "Game.h"

using Microsoft::WRL::ComPtr;

//#define PONG
#define PLANETS

namespace GE {
	using Matrix = DirectX::SimpleMath::Matrix;
	using Vector3 = DirectX::SimpleMath::Vector3;

	static std::shared_ptr<InputDevice> inputDevice; // defaults to nullptr
	static std::shared_ptr<WindowHandler> winHandler; // defaults to nullptr
	static std::shared_ptr<PhysicsSubsystem> physicsSubsystem; // defaults to nullptr
	static std::shared_ptr<Game> gameSubsystem; // defaults to nullptr
	static std::shared_ptr<ShaderManager> shaderManager; // defaults to nullptr
	static std::shared_ptr<BufferManager> bufferManager; // defaults to nullptr

	void initGraphicsEngine();
	static void initWindowHandler();
	static void initInputDevice(std::shared_ptr<WindowHandler> winHandler);
	static void initPhysicsSubsystem();
	static void initGameSubsystem();
	static void initShaderManager();
	static void initBufferManager();

	const std::shared_ptr<WindowHandler> getWindowHandler();
	const std::shared_ptr<InputDevice> getInputDevice();
	const std::shared_ptr<PhysicsSubsystem> getPhysicsSubsystem();
	const std::shared_ptr<Game> getGameSubsystem();
	const std::shared_ptr<ShaderManager> getShaderManager();
	const std::shared_ptr<BufferManager> getBufferManager();

	const Matrix getPerspectiveMatrix();
	const Matrix getCameraViewMatrix();
	Vector3 getCameraPosition();
	void setCameraPosition(Vector3 pos);
}
