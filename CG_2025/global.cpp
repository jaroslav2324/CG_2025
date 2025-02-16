#include "global.h"

const std::shared_ptr<WindowHandler> ge::getWindowHandler()
{
	assert(winHandler);
	return winHandler;
}

const std::shared_ptr<InputDevice> ge::getInputDevice()
{
	assert(inputDevice);
	return inputDevice;
}

void ge::initGraphicsEngine()
{
	initWindowHandler();
	initInputDevice(winHandler);
}

void ge::initWindowHandler()
{
	if (winHandler) {
		return;
	}
	winHandler = std::make_shared<WindowHandler>();
}

void ge::initInputDevice(std::shared_ptr<WindowHandler> winHandler)
{
	if (inputDevice) {
		return;
	}
	inputDevice = std::make_shared<InputDevice>(winHandler);
}
