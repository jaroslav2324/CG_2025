// MySuper3DApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>
#include <random>

#include "global.h"
#include "Game.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

int main()
{
	std::srand(time(NULL));
	GE::initGraphicsEngine();

	std::shared_ptr<Game> game = GE::getGameSubsystem();
	game->init();
	game->run();

	return 0;
}

