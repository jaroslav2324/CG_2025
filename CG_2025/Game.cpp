#include "Game.h"
#include "global.h"

#include "PlanetComponent.h"

using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Matrix;
using DirectX::BoundingBox;

void coutVector(Vector3 v) {
	std::cout << v.x << " " << v.y << " " << v.z << std::endl;
}

bool CheckSphereAABBCollision(const Vector3& sphereCenter, float sphereRadius, const BoundingBox& aabb) {
	Vector3 closestPoint;

	Vector3 corners[8];
	aabb.GetCorners(corners);
	closestPoint = corners[0];
	Vector3 minDiff;
	minDiff.x = std::abs(corners[0].x - sphereCenter.x);
	minDiff.y = std::abs(corners[0].y - sphereCenter.y);
	minDiff.z = std::abs(corners[0].z - sphereCenter.z);
	for (auto p : corners) {
		if (std::abs(p.x - sphereCenter.x) < minDiff.x) {
			minDiff.x = std::abs(p.x - sphereCenter.x);
			closestPoint.x = p.x;
		}
		if (std::abs(p.y - sphereCenter.y) < minDiff.y) {
			minDiff.y = std::abs(p.y - sphereCenter.y);
			closestPoint.y = p.y;
		}
		if (std::abs(p.z - sphereCenter.z) < minDiff.z) {
			minDiff.z = std::abs(p.z - sphereCenter.z);
			closestPoint.z = p.z;
		}
	}

	// Вычисляем вектор от центра сферы до ближайшей точки
	Vector3 distanceVector = closestPoint - sphereCenter;

	// Вычисляем квадрат расстояния
	float distanceSquared = distanceVector.LengthSquared();

	// Проверяем, меньше ли квадрат расстояния радиуса сферы
	return distanceSquared <= (sphereRadius * sphereRadius);
}

Game::Game()
{
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

	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.Width = GE::getWindowHandler()->getWinWidth();
	depthTexDesc.Height = GE::getWindowHandler()->getWinHeight();
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.SampleDesc = { 1, 0 };

	// Создание буфера глубины
	res = device->CreateTexture2D(&depthTexDesc, nullptr, depthBuffer.GetAddressOf());

	res = device->CreateTexture2D(&depthTexDesc, nullptr, &depthBuffer);
	res = device->CreateDepthStencilView(depthBuffer.Get(), nullptr, &depthView);

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 0;
	samplerDesc.MaxLOD = INT_MAX;

	device->CreateSamplerState(&samplerDesc, &sampler);


}

void Game::init()
{
#if defined(PONG)
	createPongScene();
#endif
#if defined(PLANETS)
	cretePlanetsScene();
#endif
#if defined (CATAMARI)
	createKatamariScene();
#endif
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

#if defined(PONG)
	updatePongScene(deltaTime);
#endif
#if defined(PLANETS)
	updatePlanetsScene(deltaTime);
#endif
#if defined(CATAMARI)
	updateKatamariScene(deltaTime);
#endif
}

void Game::shadowPass()
{
	for (auto& light : lightSources)
	{
		if (light.ls.sourceType != LightSourceType::DIRECTIONAL_LIGHT) continue;

		auto& shadowMap = light.shMap;
		const auto lightPos = light.ls.position;
		Vector3 lightPos3 = Vector3(lightPos);

		D3D11_VIEWPORT shadowViewport = {};
		shadowViewport.Width = static_cast<float>(shadowMap.data.textureSize);
		shadowViewport.Height = static_cast<float>(shadowMap.data.textureSize);
		shadowViewport.MinDepth = 0.0f;
		shadowViewport.MaxDepth = 1.0f;
		shadowViewport.TopLeftX = 0.0f;
		shadowViewport.TopLeftY = 0.0f;

		context->RSSetViewports(1, &shadowViewport);

		for (int cascadeIdx = 0; cascadeIdx < 4; ++cascadeIdx){
			context->ClearDepthStencilView(shadowMap.depthViews[cascadeIdx].Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
			context->OMSetRenderTargets(0, nullptr, (shadowMap.depthViews[cascadeIdx]).Get());
			shadowMap.recalculateMatrices(Vector3(light.ls.direction));
			setShadowViewProj(light.shMap.data.viewMatrix, light.shMap.data.projectionMatrices[cascadeIdx], light.ls);

			for (auto* component : components)
			{
				component->drawShadow();
			}
		}	
	}
}

void Game::setShadowViewProj(Matrix lightView, Matrix lightProj, const LightSourceData& lightData)
{
	shp_lightAddData.lightSourcePos = lightData.position;
	shp_lightAddData.lightSourcePos.w = lightData.shineDistance;
	shp_lightAddData.projMatrix = lightProj.Transpose();
	shp_lightAddData.viewMatrix = lightView.Transpose();
	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawAdditionalLightBuffer = shp_lightAddDataBuffer.Get();
	context->Map(rawAdditionalLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &shp_lightAddData, sizeof(ShadowPassLightAddData));
	context->Unmap(rawAdditionalLightBuffer, 0);
	context->VSSetConstantBuffers(1, 1, &rawAdditionalLightBuffer);
	context->PSSetConstantBuffers(1, 1, &rawAdditionalLightBuffer);
}

void Game::updatePongScene(float deltaTime)
{
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
		std::cout << "Score " << rightPongScore << " : " << leftPongScore << std::endl;
		ball->setVelocity(startBallVelocity);
		ball->setDirection(generateRandomBallDirection());
	}
	if (ballPosition.x > winWidth) {
		ball->setPosition({ winWidth / 2, winHeight / 2 });
		leftRocket->setPosition(coordsStartLeftRocket);
		rightRocket->setPosition(coordsStartRightRocket);
		++rightPongScore;
		std::cout << "Score " << rightPongScore << " : " << leftPongScore << std::endl;
		ball->setVelocity(startBallVelocity);
		ball->setDirection(generateRandomBallDirection());
	}
}

void Game::cretePlanetsScene()
{
	auto win = GE::getWindowHandler();
	auto winWidth = static_cast<float>(win->getWinWidth());
	float winHeight = static_cast<float>(win->getWinHeight());

	createPlanetComponent({ 0.0f, 0.0f, 0.0f }, 10.0);
	PlanetComponent* p3 = (PlanetComponent*)components[0];
	p3->setSphereMesh({ 0.0f, 0.0f, 0.0f }, 50.0, 128, 128);
	p3->setAngularSpeedSelf(0.0f);
	p3->setStarsColor(0.98f);

	createPlanetComponent({ 0.0f, 0.0f, 0.0f }, 0.15);
	PlanetComponent* sun = (PlanetComponent*)components[1];
	sun->setColor({ 1.0f, 1.0f, 0.0f, 1.0f });
	createPlanetComponent({ -0.6f, 0.0f, 0.0f }, 0.05);
	// cyan box
	PlanetComponent* p1 = (PlanetComponent*)components[2];
	p1->setParentPlanet(sun);
	p1->setBoxMesh({ -0.6f, 0.0f, 0.0f }, { 0.1 , 0.1 , 0.1 });
	p1->setAngularSpeedAroundParent(0.01f);
	//p1->setAngularSpeedAroundParent(0.0);

	// cyan sphere
	createPlanetComponent({ -0.2f, 0.0f, 0.0f }, 0.03);
	PlanetComponent* p2 = (PlanetComponent*)components[3];
	p2->setParentPlanet(p1);
	p2->setRotationAroundParentAxis({ 0.0f, 0.0f, 1.0f });
	//p2->setAngularSpeedAroundParent(0.0);

	// red
	createPlanetComponent({ 0.05f, 0.05f, 0.05f }, 0.01);
	PlanetComponent* p4 = (PlanetComponent*)components[4];
	p4->setParentPlanet(p2);
	p4->setColor({ 0.7f, 0.0f, 0.0f, 1.0f });
	Vector3 rp4 = { -1.0, 1.0, 1.0 };
	rp4.Normalize();
	p4->setRotationAroundParentAxis(rp4);

	// purple box
	createPlanetComponent({ -1.0f, 0.0f, 0.0f }, 0.03);
	PlanetComponent* p5 = (PlanetComponent*)components[5];
	p5->setBoxMesh({ -1.0f, 0.0f, 0.0f }, { 0.1 , 0.05 , 0.05 });
	p5->setAngularSpeedAroundParent(-0.05f);
	p5->setRotationAroundParentAxis({ 0.0, 1.0, 0.0 });
	p5->setColor({ 1.0f, 0.0f, 1.0f, 1.0f });
	p5->setParentPlanet(sun);

	// blue box
	createPlanetComponent({ -0.1f, 0.1f, 0.1f }, 0.01);
	PlanetComponent* p6 = (PlanetComponent*)components[6];
	p6->setBoxMesh({ -0.1f, 0.1f, 0.1f }, { 0.05 , 0.05 , 0.05 });
	p6->setAngularSpeedAroundParent(-0.01f);
	p6->setColor({ 0.0f, 0.0f, 0.5f, 1.0f });
	p6->setAngularSpeedSelf(-0.1f);
	Vector3 r6 = { 0.5f, 0.5f, 0.5f };
	r6.Normalize();
	p6->setPlanetAxis(r6);
	Vector3 rp6 = { 0.3f, 0.8f, 0.3f };
	rp6.Normalize();
	p6->setRotationAroundParentAxis(rp6);
	p6->setParentPlanet(p5);

	//purple
	createPlanetComponent({ -0.5f, -0.5f, -0.5f }, 0.01);
	PlanetComponent* p7 = (PlanetComponent*)components[7];
	p7->setColor({ 0.5, 0.2, 0.4, 1.0 });
	Vector3 rp7 = { -0.5f, 0.5f, 0.5f };
	rp7.Normalize();
	p7->setRotationAroundParentAxis(rp7);
	p7->setParentPlanet(sun);
}

static bool orbitCameraActive = true;
void Game::updatePlanetsScene(float deltaTime)
{
	auto win = GE::getWindowHandler();
	float winWidth = static_cast<float>(win->getWinWidth());
	float winHeight = static_cast<float>(win->getWinHeight());

	std::shared_ptr<InputDevice> inputDevice = GE::getInputDevice();

	if (inputDevice->IsKeyDown(Keys::D1)) {
		orbitCameraActive = true;
		GE::setCameraForwardVector({ 0.0f, 0.0f, 1.0f });
		GE::setCameraPosition({ 0.0f, 0.0f, -3.0f });
		GE::setCameraUpVector({ 0.0f, 1.0f, 0.0f });
	}
	if (inputDevice->IsKeyDown(Keys::D2)) {
		orbitCameraActive = false;
	}
	if (inputDevice->IsKeyDown(Keys::D3)) {
		GE::setPerspectiveMatrix(45.0 / 180.0 * DirectX::XM_PI);
	}
	if (inputDevice->IsKeyDown(Keys::D4)) {
		GE::setPerspectiveMatrix(90.0 / 180.0 * DirectX::XM_PI);
	}

	if (orbitCameraActive) {
		if (inputDevice->IsKeyDown(Keys::Right))
		{
			Matrix rot = Matrix::CreateRotationY(deltaTime);
			GE::rotateCamera(rot);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), rot));
		}
		if (inputDevice->IsKeyDown(Keys::Left))
		{
			Matrix rot = Matrix::CreateRotationY(-deltaTime);
			GE::rotateCamera(rot);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), rot));
		}
		if (inputDevice->IsKeyDown(Keys::Up)) {
			Vector3 cameraForvardVector = -GE::getCameraPosition();
			cameraForvardVector.Normalize();
			Vector3 cameraRightVector = cameraForvardVector.Cross(GE::getCameraUpVector());
			cameraRightVector.Normalize();
			Matrix rot = Matrix::CreateFromAxisAngle(cameraRightVector, -deltaTime);
			GE::rotateCamera(rot);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), rot));
		}
		if (inputDevice->IsKeyDown(Keys::Down)) {
			Vector3 cameraForvardVector = -GE::getCameraPosition();
			cameraForvardVector.Normalize();
			Vector3 cameraRightVector = cameraForvardVector.Cross(GE::getCameraUpVector());
			cameraRightVector.Normalize();
			Matrix rot = Matrix::CreateFromAxisAngle(cameraRightVector, deltaTime);
			GE::rotateCamera(rot);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), rot));
		}
	}
	else {
		Vector2 mousePos = inputDevice->MousePosition;
		if (!apprEqual(prevMousePosition.x, mousePos.x)) {
			Vector3 camPos = GE::getCameraPosition();
			GE::setCameraPosition({ 0.0f, 0.0f, 0.0f });
			Matrix rot = Matrix::CreateRotationY(deltaTime * (mousePos.x - prevMousePosition.x));
			GE::rotateCamera(rot);
			GE::setCameraPosition(camPos);
		}
		if (!apprEqual(prevMousePosition.y, mousePos.y)) {
			Vector3 camPos = GE::getCameraPosition();
			GE::setCameraPosition({ 0.0f, 0.0f, 0.0f });
			Vector3 cameraRightVector = GE::getCameraForwardVector().Cross(GE::getCameraUpVector());
			cameraRightVector.Normalize();
			Matrix rot = Matrix::CreateFromAxisAngle(cameraRightVector, -deltaTime * (mousePos.y - prevMousePosition.y));
			GE::rotateCamera(rot);
			GE::setCameraPosition(camPos);
		}
		prevMousePosition = mousePos;

		if (inputDevice->IsKeyDown(Keys::W)) {
			Matrix tr = Matrix::CreateTranslation(GE::getCameraForwardVector() * deltaTime);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), tr));
		}
		if (inputDevice->IsKeyDown(Keys::S)) {
			Matrix tr = Matrix::CreateTranslation(-GE::getCameraForwardVector() * deltaTime);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), tr));
		}

		if (inputDevice->IsKeyDown(Keys::D)) {
			Vector3 cameraRightVector = GE::getCameraForwardVector().Cross(GE::getCameraUpVector());
			Matrix tr = Matrix::CreateTranslation(cameraRightVector * deltaTime);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), tr));
		}
		if (inputDevice->IsKeyDown(Keys::A)) {
			Vector3 cameraRightVector = GE::getCameraForwardVector().Cross(GE::getCameraUpVector());
			Matrix tr = Matrix::CreateTranslation(-cameraRightVector * deltaTime);
			GE::setCameraPosition(Vector3::Transform(GE::getCameraPosition(), tr));
		}
	}

}

void Game::createKatamariScene()
{
	std::vector<std::string> objectsPaths = {
		"./models/box.obj"
	};

	CatamariBall* ball = createCatamariBallComponent({ 0, 0, 0 }, 0.2);
	const int numObjects = 10;
	for (int i = 0; i < numObjects; i++) {
		float x = generateRandomFloat(-5.0f, 5.0f);
		float z = generateRandomFloat(-5.0f, 5.0f);
		CatamariBox* obj = createCatamariBoxComponent({ x, 0, z }, { 0.1, 0.1, 0.1 });
	}

	CatamariBox* floor = new CatamariBox({ 0.0, -1.2f, 0.0 }, { 1.0, 1.0, 1.0 });
	floor->setAttached(ball, true);
	floor->setModelPath("./models/ground.obj");
	floor->setTexturePath(L"./models/bt.dds");
	components.push_back(floor);
	components[components.size() - 1]->init(L"./shaders/texVertexShader.hlsl",
		L"./shaders/texPixelShader.hlsl");


	GE::setCameraPosition(ball->getPosition() + Vector3(0, ball->getPosition().y + 2, -2));
	GE::setCameraForwardVector(Vector3(0, -1, 1));
	GE::setCameraUpVector(Vector3(0, 1, 1));

	std::shared_ptr<InputDevice> inputDevice = GE::getInputDevice();
	prevMousePosition = inputDevice->MousePosition;
	GE::setPerspectiveMatrix(1.4f);

	lightSources.reserve(10);
	for (int i = 0; i < 4; i++) {
		lightSources.emplace_back(LightSource());
	}
	
	LightSourceData& ls = lightSources[0].ls;
	ls.sourceType = LightSourceType::AMBIENT_LIGHT;
	ls.intensity = 0.5f;
	++countLightSources;

	LightSourceData& ls2 = lightSources[1].ls;
	ls2.sourceType = LightSourceType::DIRECTIONAL_LIGHT;
	Vector3 dir2 = Vector3(0.3f, -1.0f, 0.3f);
	dir2.Normalize();
	ls2.direction = Vector4(dir2);
	ls2.position = Vector4(-1.0f, 3.0f, -1.0f, 1.0f);
	ls2.rgb = Vector4(0.75f, 0.75f, 0.75f, 1.0f);
	ls2.intensity = 0.5f;
	++countLightSources;

	LightSourceData& ls3 = lightSources[2].ls;
	ls3.sourceType = LightSourceType::POINT_LIGHT;
	ls3.position = Vector4(-3.0f, 2.0f, 2.0f, 1.0f);
	ls3.rgb = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	ls3.shineDistance = 5.0f;
	ls3.intensity = 5.0f;
	++countLightSources;
	lightSources[2].mesh = createLightSourceComponent(Vector3(ls3.position), 0.07f, Vector3(ls3.rgb));

	LightSourceData& ls4 = lightSources[3].ls;
	ls4.sourceType = LightSourceType::SPOT_LIGHT;
	ls4.position = Vector4(0.0f, 2.0f, 0.5f, 1.0f);
	Vector3 dir4 = Vector3(0.0f, -1.0f, -0.2f);
	dir4.Normalize();
	ls4.direction = Vector4(dir4);
	ls4.rgb = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	ls4.shineDistance = 6.0f;
	ls4.intensity = 5.0f;
	ls4.angle = 15.0 / 360.0 * DirectX::XM_2PI;
	++countLightSources;
	lightSources[3].mesh = createLightSourceComponent(Vector3(ls4.position), 0.07f, Vector3(ls4.rgb));

	for (int i = 0; i < countLightSources; i++) {
		lightSources[i].init();
	}

	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = &shp_lightAddData;
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;

	D3D11_BUFFER_DESC buffDesc = {};
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;
	buffDesc.ByteWidth = sizeof(shp_lightAddData);

	auto bufferManager = GE::getBufferManager();
	shp_lightAddDataBuffer = bufferManager->createBuffer(buffDesc, subresourceData);
}

void Game::updateKatamariScene(float deltaTime)
{
	auto win = GE::getWindowHandler();
	float winWidth = static_cast<float>(win->getWinWidth());
	float winHeight = static_cast<float>(win->getWinHeight());

	std::shared_ptr<InputDevice> inputDevice = GE::getInputDevice();
	Vector2 mousePos = inputDevice->MousePosition;
	if (apprEqual(prevMousePosition.Length(), 0.0f)) {
		prevMousePosition = mousePos;
	}

	CatamariBall* ball = (CatamariBall*)components[0];

	if (!apprEqual(prevMousePosition.x, mousePos.x, 1e-6)) {
		float angle = deltaTime * (mousePos.x - prevMousePosition.x);
		Matrix rot = Matrix::CreateRotationY(angle);
		Vector3 relCamPos = GE::getCameraPosition() - ball->getPosition();
		Vector3 tmpUp = Vector3::Transform(GE::getCameraUpVector(), rot);
		tmpUp.Normalize();
		Vector3 tmpForward = Vector3::Transform(GE::getCameraForwardVector(), rot);
		tmpForward.Normalize();
		Vector3 tmpCamPos = Vector3::Transform(relCamPos, rot);
		tmpCamPos += ball->getPosition();
		GE::setCameraPosition(tmpCamPos);
		GE::setCameraUpVector(tmpUp);
		GE::setCameraForwardVector(tmpForward);
	}
	if (!apprEqual(prevMousePosition.y, mousePos.y, 1e-6)) {
		float angle = deltaTime * (mousePos.y - prevMousePosition.y);
		Vector3 cameraRightVector = GE::getCameraForwardVector().Cross(GE::getCameraUpVector());
		cameraRightVector.Normalize();
		Matrix rot = Matrix::CreateFromAxisAngle(cameraRightVector, -angle);
		//GE::rotateCameraAroundCenter(ball->getPosition(), rot);
		Vector3 relCamPos = GE::getCameraPosition() - ball->getPosition();
		Vector3 tmpUp = Vector3::Transform(GE::getCameraUpVector(), rot);
		tmpUp.Normalize();
		Vector3 tmpForward = Vector3::Transform(GE::getCameraForwardVector(), rot);
		tmpForward.Normalize();
		Vector3 tmpCamPos = Vector3::Transform(relCamPos, rot);
		tmpCamPos += ball->getPosition();
		GE::setCameraPosition(tmpCamPos);
		GE::setCameraUpVector(tmpUp);
		GE::setCameraForwardVector(tmpForward);
	}
	prevMousePosition = mousePos;

	if (inputDevice->IsKeyDown(Keys::W) || inputDevice->IsKeyDown(Keys::S)) {
		Vector3 shiftDirection = GE::getCameraForwardVector();
		//std::cout << shiftDirection.x << " " << shiftDirection.y << " " << shiftDirection.z << std::endl;
		shiftDirection.y = 0;
		shiftDirection.Normalize();
		if (inputDevice->IsKeyDown(Keys::S)) {
			shiftDirection *= -1;
		}
		Vector3 shiftVec = deltaTime * shiftDirection;
		Vector3 rotationAxis = -shiftDirection.Cross(Vector3(0, 1, 0));
		rotationAxis.Normalize();
		//coutVector(GE::getCameraPosition());
		GE::setCameraPosition(GE::getCameraPosition() + shiftVec);
		float distance = shiftVec.Length();
		float rotationAngle = distance / ball->getRadius();
		ball->moveBall(shiftVec, rotationAxis, rotationAngle);

		//Vector3 ballPos = ball->getPosition();
		//for (auto& ls: lightSources) {
		//	Vector3 lsPos(ls.ls.position);
		//	lsPos -= ballPos;
		//	Matrix m = Matrix::CreateFromAxisAngle(rotationAxis, rotationAngle);
		//	lsPos = Vector3::Transform(lsPos, m);
		//	lsPos += ballPos;
		//	ls.ls.position = Vector4(lsPos);
		//	ls.mesh->setPosition(lsPos);
		//}
	}

	for (int i = 0; i < components.size(); i++) {
		CatamariBox* box = (CatamariBox*)components[i];
		if (!box->isAttached()) {

		}
	}

	// check collision
	float radius = ball->getRadius();
	Vector3 center = ball->getPosition();
	for (int i = 1; i < components.size(); i++) {
		auto box = (CatamariBox*)components[i];
		//if (!box->isAttached() && CheckSphereAABBCollision(center, radius, box->getAABB())) {
		// box->setAttached(ball);
		//	ball->attachObject(box);
		//	std::cout << "box attached" << std::endl;
		//}
	}

	for (int i = 1; i < components.size(); i++) {
		for (int j = i; j < components.size(); j++) {
			auto box1 = (CatamariBox*)components[i];
			auto box2 = (CatamariBox*)components[j];
			if (box1->isAttached() && box2->isAttached()) {
				continue;
			}
			if (!box1->isAttached() && !box2->isAttached()) {
				continue;
			}

			//if (box1->getAABB().Intersects(box2->getAABB())) {
			//	if (box1->isAttached()) {
			//		box2->setAttached(ball);
			//		ball->attachObject(box2);
			//	}
			//	else {
			//		box1->setAttached(ball);
			//		ball->attachObject(box1);
			//	}
			//}
		}
	}

}

void Game::run()
{
	std::chrono::time_point<std::chrono::steady_clock> prevTime = prevTime = std::chrono::steady_clock::now();
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
		draw(deltaTime);

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

int Game::draw(float deltaTime)
{
	auto winHandler = GE::getWindowHandler();

	context->ClearState();

	auto renderSubsystem = GE::getRenderSubsystem();

	shadowPass();

	context->ClearDepthStencilView(depthView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	renderSubsystem->render(deltaTime);

	for (const auto& ls : lightSources) {
		if (ls.mesh) {
			ls.mesh->draw(deltaTime);
		}
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

int Game::createPlanetComponent(DirectX::SimpleMath::Vector3 position, float radius)
{
	PlanetComponent* planet = new PlanetComponent(position, radius, {1.0f, 1.0f, 1.0f});
	components.push_back(planet);
	components[components.size() - 1]->init(
		L"./shaders/planetShader.hlsl",
		L"./shaders/pixelShader.hlsl");
	return 0;
}

CatamariBall* Game::createCatamariBallComponent(DirectX::SimpleMath::Vector3 position, float radius, int stacks, int slices)
{
	CatamariBall* ball = new CatamariBall(position, radius, stacks, slices);
	components.push_back(ball);
	components[components.size() - 1]->init(L"./shaders/texVertexShader.hlsl",
		L"./shaders/texPixelShader.hlsl");
	return ball;
}

CatamariBox* Game::createCatamariBoxComponent(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 size)
{
	CatamariBox* box = new CatamariBox(position, size);
	components.push_back(box);
	components[components.size() - 1]->init(L"./shaders/texVertexShader.hlsl",
		L"./shaders/texPixelShader.hlsl");
	return box;
}

PlanetComponent* Game::createLightSourceComponent(DirectX::SimpleMath::Vector3 position, float radius, Vector3 color)
{
	auto ls = new PlanetComponent(position, radius,color);
	ls->init(
		L"./shaders/lightSourceVertShader.hlsl",
		L"./shaders/lightSourcePixShader.hlsl");
	return ls;
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

float Game::getTotalTime()
{
	return totalTime;
}

ID3D11Device* Game::getDevice()
{
	return device;
}

ComPtr<ID3D11DepthStencilView> Game::getDepthView()
{
	return depthView;
}

ID3D11DeviceContext* Game::getDeviceContext()
{
	return context;
}

const ComPtr<ID3D11SamplerState> Game::getSamplerState()
{
	return sampler;
}
