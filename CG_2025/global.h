#pragma once

#include <wrl/client.h>

#include "WindowHandler.h"
#include "InputDevice.h"

using Microsoft::WRL::ComPtr;

namespace ge {
	static std::shared_ptr<InputDevice> inputDevice; // defaults to nullptr
	static std::shared_ptr<WindowHandler> winHandler; // defaults to nullptr

	void initGraphicsEngine();
	void initWindowHandler();
	void initInputDevice(std::shared_ptr<WindowHandler> winHandler);

	const std::shared_ptr<WindowHandler> getWindowHandler();
	const std::shared_ptr<InputDevice> getInputDevice();

}
