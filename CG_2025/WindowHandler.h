#pragma once

#include <Windows.h>
#include <WinUser.h>

#include "InputDevice.h"

class Game;

class WindowHandler {
public:
	HWND getWindowHandle();
	const int getWinWidth() const;
	const int getWinHeight() const;
	~WindowHandler();
private:
	HWND windowHandle = nullptr;
	Game* game = nullptr;
	std::shared_ptr<InputDevice> inputDevice = nullptr;

	const int winWidth = 800;
	const int winHeight = 800;
};