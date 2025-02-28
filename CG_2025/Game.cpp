#include "Game.h"
#include "global.h"

using DirectX::SimpleMath::Vector2;

Game::Game()
{
	// TODO: move to renderer
	auto winHandler = GE::getWindowHandler();
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
	createRectComponent(coordsStartLeftRocket, 25, 150);
	// right rocket
	createRectComponent(coordsStartRightRocket, 25, 150);
	// ball
	components.push_back(new RectComponent({ 400, 400 }, 25, 25));
	RectComponent* ball = (RectComponent*)components[2];
	ball->setDrawableCircle();
	ball->init(
		L"./shaders/pongShader.hlsl",
		L"./shaders/pixelShader.hlsl");
	RectComponent* leftRocket = (RectComponent*)components[0];
	RectComponent* rightRocket = (RectComponent*)components[1];

	Vector2 v = generateRandomBallDirection();
	ball->setDirection(v);
	ball->setVelocity(startBallVelocity);
	ball->setCollisionCallback(ball, [this, leftRocket, rightRocket](RectComponent* rectComponent, RectComponent* rect2, Vector2 collisionNormal) {
		if (rectComponent->collided()) {
			return;
		}
		if (rect2 == leftRocket && reflectedFromLeftRocket) {
			return;
		}
		if (rect2 == rightRocket && !reflectedFromLeftRocket) {
			return;
		}

		if (rect2 == leftRocket) {
			reflectedFromLeftRocket = true;
		}
		if (rect2 == rightRocket) {
			reflectedFromLeftRocket = false;
		}

		// std::cout << "collided" << std::endl;
		Vector2 direction = rectComponent->getDirection();
		Vector2 reflectedDirection;
		if (direction.Dot(collisionNormal) < 0) {
			reflectedDirection = reflectRelativeToNormal(direction, collisionNormal);
		}
		else {
			reflectedDirection = direction;
			reflectedDirection.y *= 3;
		}
		reflectedDirection.Normalize();
		rectComponent->setDirection(reflectedDirection);
		rectComponent->setVelocity(rectComponent->getVelocity() + 100);
		rectComponent->setCollided(true);
		});
}

void Game::update(float deltaTime)
{
	for (const auto gameComponent : components) {
		gameComponent->update(deltaTime);
	}

	const int pongRocketSpeedCoeff = 400.0f;
	auto win = GE::getWindowHandler();
	auto winWidth = static_cast<float>(win->getWinWidth());
	float winHeight = static_cast<float>(win->getWinHeight());

	RectComponent* leftRocket = (RectComponent*)components[0];
	RectComponent* rightRocket = (RectComponent*)components[1];
	RectComponent* ball = (RectComponent*)components[2];

	std::shared_ptr<InputDevice> inputDevice = GE::getInputDevice();
	if (inputDevice->IsKeyDown(Keys::W)) {
		Vector2 pos = leftRocket->getPosition();
		pos.y = std::clamp(pos.y + deltaTime * pongRocketSpeedCoeff, 75.0f, winHeight - 75.0f);
		leftRocket->setPosition(pos);
	}
	if (inputDevice->IsKeyDown(Keys::S)) {
		Vector2 pos = leftRocket->getPosition();
		pos.y = std::clamp(pos.y - deltaTime * pongRocketSpeedCoeff, 75.0f, winHeight - 75.0f);
		leftRocket->setPosition(pos);
	}
	if (inputDevice->IsKeyDown(Keys::D)) {
		Vector2 pos = leftRocket->getPosition();
		pos.x = std::clamp(pos.x + deltaTime * pongRocketSpeedCoeff, 100.0f, winWidth / 2);
		leftRocket->setPosition(pos);
	}

	if (inputDevice->IsKeyDown(Keys::A)) {
		Vector2 pos = leftRocket->getPosition();
		pos.x = std::clamp(pos.x - deltaTime * pongRocketSpeedCoeff, 100.0f, winWidth / 2);
		leftRocket->setPosition(pos);
	}

	if (inputDevice->IsKeyDown(Keys::Up)) {
		Vector2 pos = rightRocket->getPosition();
		pos.y = std::clamp(pos.y + deltaTime * pongRocketSpeedCoeff, 75.0f, winHeight - 75.0f);
		rightRocket->setPosition(pos);
	}
	if (inputDevice->IsKeyDown(Keys::Down)) {
		Vector2 pos = rightRocket->getPosition();
		pos.y = std::clamp(pos.y - deltaTime * pongRocketSpeedCoeff, 75.0f, winHeight - 75.0f);
		rightRocket->setPosition(pos);
	}

	Vector2 ballPosition = ball->getPosition();
	if (ballPosition.y < 0) {
		Vector2 reflectionNormal = { 0, 1 };
		ball->setDirection(reflectRelativeToNormal(ball->getDirection(), reflectionNormal));
	}
	if (ballPosition.y > winHeight) {
		Vector2 reflectionNormal = { 0, -1 };
		ball->setDirection(reflectRelativeToNormal(ball->getDirection(), reflectionNormal));
	}
	if (ballPosition.x < 0) {
		ball->setPosition({ winWidth / 2, winHeight / 2 });
		leftRocket->setPosition(coordsStartLeftRocket);
		rightRocket->setPosition(coordsStartRightRocket);
		++leftPongScore;
		std::cout << "Score " << leftPongScore << " : " << rightPongScore << std::endl;
		ball->setVelocity(startBallVelocity);
		ball->setDirection(generateRandomBallDirection());
	}
	if (ballPosition.x > winWidth) {
		ball->setPosition({ winWidth / 2, winHeight / 2 });
		leftRocket->setPosition(coordsStartLeftRocket);
		rightRocket->setPosition(coordsStartRightRocket);
		++rightPongScore;
		std::cout << "Score " << leftPongScore << " : " << rightPongScore << std::endl;
		ball->setVelocity(startBallVelocity);
		ball->setDirection(generateRandomBallDirection());
	}
}

void Game::run()
{
	std::chrono::time_point<std::chrono::steady_clock> prevTime = prevTime = std::chrono::steady_clock::now();
	static float totalTime;
	static unsigned int frameCount;
	auto physicsSubsystem = GE::getPhysicsSubsystem();

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

		auto	curTime = std::chrono::steady_clock::now();
		float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - prevTime).count() / 1000000.0f;
		prevTime = curTime;

		totalTime += deltaTime;
		frameCount++;

		physicsSubsystem->updatePhysics(deltaTime);
		update(deltaTime);
		draw();

		if (totalTime > 1.0f) {
			float fps = frameCount / totalTime;

			totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(hWnd, text);

			frameCount = 0;
		}
	}
}

int Game::draw()
{
	auto winHandler = GE::getWindowHandler();

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

DirectX::SimpleMath::Vector2 Game::generateRandomBallDirection()
{
	float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

	DirectX::SimpleMath::Vector2 out;
	if (r < 0.5f) {
		out.x = -1;
		reflectedFromLeftRocket = false;
	}
	else {
		out.x = 1;
		reflectedFromLeftRocket = true;
	}
	r = (static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
	out.y = r;
	out.Normalize();
	return out;
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
