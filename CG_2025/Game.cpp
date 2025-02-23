#include "Game.h"
#include "global.h"

using DirectX::SimpleMath::Vector2;

Game::Game()
{
	// TODO: move to renderer
	winHandler = GE::getWindowHandler();
	HWND hWnd = winHandler->getWindowHandle();

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };

	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = winHandler->getWinWidth();
	swapDesc.BufferDesc.Height = winHandler->getWinHeight();
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	auto res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&device,
		nullptr,
		&context);

	if (FAILED(res))
	{
		throw GraphicsException("Device and swap chain creation failed");
	}

	res = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backTex);	// __uuidof(ID3D11Texture2D)
	if (FAILED(res))
	{
		throw GraphicsException("Getting dafault back buffer failed");
	}

	res = device->CreateRenderTargetView(backTex, nullptr, &rtv);
	if (FAILED(res))
	{
		throw GraphicsException("Creating render target view for back buffer failed");
	}
}

void Game::init()
{
}

void Game::createPongScene()
{
	// left rocket
	createRectComponent({ 100, 400 }, 25, 150);
	// right rocket
	createRectComponent({ 700, 400 }, 25, 150);
	// ball
	createRectComponent({ 400, 400 }, 25, 25);
}

void Game::update(float deltaTime)
{
	for (const auto gameComponent : components) {
		gameComponent->update(deltaTime);
	}

	const int pongRocketSpeedCoeff = 200.0f;
	auto win = GE::getWindowHandler();
	float winHeight = win->getWinHeight();

	RectComponent* leftRocket = (RectComponent*)components[0];
	RectComponent* rightRocket = (RectComponent*)components[1];

	std::shared_ptr<InputDevice> inputDevice = GE::getInputDevice();
	if (inputDevice->IsKeyDown(Keys::W)) {
		Vector2 pos = leftRocket->getPosition();
		pos.y = std::clamp(pos.y + deltaTime * pongRocketSpeedCoeff, 0.0f, winHeight);
		leftRocket->setPosition(pos);
	}
	if (inputDevice->IsKeyDown(Keys::S)) {
		Vector2 pos = leftRocket->getPosition();
		pos.y = std::clamp(pos.y - deltaTime * pongRocketSpeedCoeff, 0.0f, winHeight);
		leftRocket->setPosition(pos);
	}
	if (inputDevice->IsKeyDown(Keys::Up)) {
		Vector2 pos = rightRocket->getPosition();
		pos.y = std::clamp(pos.y + deltaTime * pongRocketSpeedCoeff, 0.0f, winHeight);
		rightRocket->setPosition(pos);
	}
	if (inputDevice->IsKeyDown(Keys::Down)) {
		Vector2 pos = rightRocket->getPosition();
		pos.y = std::clamp(pos.y - deltaTime * pongRocketSpeedCoeff, 0.0f, winHeight);
		rightRocket->setPosition(pos);
	}
}

void Game::run()
{
	MSG msg = {};
	bool isExitRequested = false;
	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);

			// If windows signals to end the application then exit out.
			if (msg.message == WM_QUIT) {
				isExitRequested = true;
			}
			DispatchMessage(&msg);
		}

		// TODO: move delta time here
		update(0.0167f);
		draw();
	}
}

int Game::draw()
{
	std::chrono::time_point<std::chrono::steady_clock> prevTime = std::chrono::steady_clock::now();
	static float totalTime;
	static unsigned int frameCount;

	context->ClearState();

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(winHandler->getWinWidth());
	viewport.Height = static_cast<float>(winHandler->getWinHeight());
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(rtv, color);
	context->OMSetRenderTargets(1, &rtv, nullptr);


	for (const auto gameComponent : components) {
		gameComponent->draw();
	}

	context->OMSetRenderTargets(0, nullptr, nullptr);
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);

	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
	prevTime = curTime;

	totalTime += deltaTime;
	frameCount++;

	// TODO: move this to update, move fps counting from here
	for (const auto gameComponent : components) {
		gameComponent->update(deltaTime);
	}

	if (totalTime > 1.0f) {
		float fps = frameCount / totalTime;

		totalTime -= 1.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(hWnd, text);

		frameCount = 0;
	}

	return 0;
}

int Game::createMeshComponent(std::vector<DirectX::XMFLOAT4>&& points,
	std::vector<UINT>&& strides,
	std::vector<UINT>&& offsets,
	std::vector<int>&& indices
)
{
	components.push_back(new MeshComponent(
		std::move(points),
		std::move(strides),
		std::move(offsets),
		std::move(indices)
	));
	components[components.size() - 1]->init(
		L"./shaders/vertexShader.hlsl",
		L"./shaders/pixelShader.hlsl");
	return 0;
}

int Game::createRectComponent(Vector2 centerPoint, float width, float height)
{
	components.push_back(new RectComponent(centerPoint, width, height));
	components[components.size() - 1]->init(
		L"./shaders/pongShader.hlsl",
		L"./shaders/pixelShader.hlsl");
	return 0;
}

Game::~Game()
{
	destroyResources();
}

void Game::destroyResources()
{
	releaseIfNotNullptr(&swapChain);
	releaseIfNotNullptr(&context);
	releaseIfNotNullptr(&device);
	releaseIfNotNullptr(&backTex);
	releaseIfNotNullptr(&rtv);

	for (auto component : components) {
		component->destroyResources();
		delete component;
	}

}

ID3D11Device* Game::getDevice()
{
	return device;
}

ID3D11DeviceContext* Game::getDeviceContext()
{
	return context;
}
