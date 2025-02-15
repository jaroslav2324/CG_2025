#pragma once

#include <Windows.h>
#include <WinUser.h>

class WindowHandler {
public:
	HWND getWindowHandler(int winWidth, int winHeight);
	~WindowHandler();
private:
	HWND windowHandler = nullptr;
};