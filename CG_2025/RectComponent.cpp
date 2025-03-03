#include "RectComponent.h"

using Vector2 = DirectX::SimpleMath::Vector2;

// TODO: удалить это чудо
RectComponent::RectComponent(
	std::vector<DirectX::XMFLOAT4>&& points,
	std::vector<UINT>&& strides,
	std::vector<UINT>&& offsets,
	std::vector<int>&& indices
) :
	MeshComponent(
		std::forward<std::vector<DirectX::XMFLOAT4>>(points),
		std::forward<std::vector<UINT>>(strides),
		std::forward<std::vector<UINT>>(offsets),
		std::forward<std::vector<int>>(indices))
{


	GE::getPhysicsSubsystem()->registerRect(this);

	float avgX = 0;
	float avgY = 0;
	for (auto p : this->points) {
		avgX += p.x;
		avgY += p.y;
	}
	avgX /= this->points.size();
	avgY /= this->points.size();
	// TODO: Убрать костыль, сделать нормальный конструктор
	for (auto& p : this->points) {
		p.x -= avgX;
		p.y -= avgY;
	}

	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();
	centerPosition = toMathCoordsFromClipSpaceCoords(winWidth, winHeight, { avgX, avgY });
	startCenterPosition = centerPosition;
}

RectComponent::RectComponent(Vector2 centerPoint, float width, float height) :
	MeshComponent()
{
	GE::getPhysicsSubsystem()->registerRect(this);
	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();

	rect = { centerPoint.x - width / 2, -(centerPoint.y - winHeight) - height / 2, width, height };
	this->width = width;
	this->height = height;

	const int shiftsX[] = { 1, 1, -1, -1 };
	const int shiftsY[] = { 1, -1, -1, 1 };
	for (int i = 0; i < 4; i++) {
		Vector2 p = centerPoint;
		p.x += width / 2 * shiftsX[i];
		p.y += height / 2 * shiftsY[i];
		p = toClipSpaceCoordsFromMathCoords(winWidth, winHeight, p);
		this->points.push_back({ p.x, p.y, 0.0f, 1.0f });
		// color
		this->points.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
	}
	centerPosition = centerPoint;
	startCenterPosition = centerPoint;
	strides = { 32 };
	offsets = { 0 };
	indices = { 0,1,2, 2,3,0 };
}


int RectComponent::init(const std::wstring& vertShaderPath, const std::wstring& pixShaderPath)
{
	MeshComponent::init(vertShaderPath, pixShaderPath);
	auto bufferManager = GE::getBufferManager();
	centerOffsetBuffer = bufferManager->createConstDynamicBufferCPUWrite(std::vector < DirectX::XMFLOAT4>{centerPointOffset});
	return 0;
}

int RectComponent::update(float deltaTime)
{
	auto nowTime = std::chrono::steady_clock::now();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(nowTime - lastCollisionTime).count();
	if (milliseconds > msUntilCollisionFlagReset) {
		collidedFlag = false;
	}

	auto win = GE::getWindowHandler();
	float winWidth = win->getWinWidth();
	float winHeight = win->getWinHeight();

	rect = { centerPosition.x - width / 2, -(centerPosition.y - winHeight) - height / 2, width, height };

	Vector2 offset = centerPosition - startCenterPosition;
	offset += Vector2{ winWidth / 2, winHeight / 2 };
	offset = toClipSpaceCoordsFromMathCoords(winWidth, winHeight, offset);
	centerPointOffset.x = offset.x;
	centerPointOffset.y = offset.y;

	D3D11_MAPPED_SUBRESOURCE res = {};
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	ID3D11Buffer* rawOffsetBuffer = centerOffsetBuffer.Get();
	context->Map(rawOffsetBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res);
	auto dataPtr = reinterpret_cast<float*>(res.pData);
	memcpy(dataPtr, &centerPointOffset, sizeof(centerPointOffset));
	context->Unmap(rawOffsetBuffer, 0);
	return 0;
}

int RectComponent::draw(float deltaTime)
{
	ID3D11DeviceContext* context = GE::getGameSubsystem()->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	ID3D11Buffer* rawOffsetBuffer = centerOffsetBuffer.Get();
	context->VSSetConstantBuffers(0, 1, &rawOffsetBuffer);
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->DrawIndexed(indices.size(), 0, 0);
	return 0;
}

void RectComponent::setDirection(Vector2 direction)
{
	this->direction = direction;
}

void RectComponent::setPosition(Vector2 pos)
{
	this->centerPosition = pos;
}

void RectComponent::setVelocity(float vel)
{
	velocity = vel;
}

void RectComponent::setCollisionCallback(RectComponent* rectComponent,
	std::function<void(RectComponent*, RectComponent*, DirectX::SimpleMath::Vector2)>&& callback)
{
	collisionCallbackSet = true;
	collisionCallback = std::move(callback);
}

void RectComponent::callCollisionCallback(RectComponent* rectComponent, RectComponent* rect2, Vector2 collisionNormal) const
{
	if (collisionCallbackSet) {
		collisionCallback(rectComponent, rect2, collisionNormal);
	}
}

Vector2 RectComponent::getDirection() const
{
	return direction;
}

Vector2 RectComponent::getPosition() const
{
	return centerPosition;
}

float RectComponent::getVelocity() const
{
	return velocity;
}

DirectX::SimpleMath::Rectangle RectComponent::getRect()
{
	return rect.toRectangle();
}

bool RectComponent::collided()
{
	return collidedFlag;
}

void RectComponent::setCollided(bool flag)
{
	lastCollisionTime = std::chrono::steady_clock::now();
	collidedFlag = flag;
}

void RectComponent::setDrawableCircle()
{
	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();

	points.clear();
	indices.clear();
	offsets.clear();
	strides.clear();

	Vector2 c = toClipSpaceCoordsFromMathCoords(winWidth, winHeight, startCenterPosition);
	points.push_back({ c.x, c.y, 0.0f, 1.0f });
	points.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });

	int segments = 32;
	float radius = width / 2;
	float angleStep = DirectX::XM_2PI / segments;
	for (int i = 0; i <= segments; i++) {
		float angle = i * angleStep;
		Vector2 p = {
			startCenterPosition.x + radius * cosf(angle),
			startCenterPosition.y + radius * sinf(angle)
		};
		p = toClipSpaceCoordsFromMathCoords(winWidth, winHeight, p);
		points.push_back({ p.x, p.y, 0.0f, 1.0f });
		points.push_back({ 1.0f, 1.0f, 1.0f, 1.0f });
	}

	for (int i = 1; i < segments - 1; i++) {
		indices.push_back(0);
		indices.push_back(i);
		indices.push_back(i + 1);
	}
	indices.push_back(0);
	indices.push_back(31);
	indices.push_back(1);

	strides = { 32 };
	offsets = { 0 };
}


