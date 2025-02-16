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

void GE::initGraphicsEngine()
{
	initWindowHandler();
	initInputDevice(winHandler);
}

void GE::initWindowHandler()
{
	if (winHandler) {
		return;
	}
	winHandler = std::make_shared<WindowHandler>();
}

void GE::initInputDevice(std::shared_ptr<WindowHandler> winHandler)
{
	if (inputDevice) {
		return;
	}
	inputDevice = std::make_shared<InputDevice>(winHandler);
}
