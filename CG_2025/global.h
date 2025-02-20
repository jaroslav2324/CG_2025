#pragma once

#include <wrl/client.h>

#include "WindowHandler.h"
#include "InputDevice.h"
#include "PhysicsSubsystem.h"
#include "Game.h"

using Microsoft::WRL::ComPtr;

namespace GE {
	static std::shared_ptr<InputDevice> inputDevice; // defaults to nullptr
	static std::shared_ptr<WindowHandler> winHandler; // defaults to nullptr
	static std::shared_ptr<PhysicsSubsystem> physicsSubsystem; // defaults to nullptr
	static std::shared_ptr<Game> gameSubsystem; // defaults to nullptr


	void initGraphicsEngine();
	static void initWindowHandler();
	static void initInputDevice(std::shared_ptr<WindowHandler> winHandler);
	static void initPhysicsSubsystem();
	static void initGameSubsystem();

	const std::shared_ptr<WindowHandler> getWindowHandler();
	const std::shared_ptr<InputDevice> getInputDevice();
	const std::shared_ptr<PhysicsSubsystem> getPhysicsSubsystem();
	const std::shared_ptr<Game> getGameSubsystem();

}
