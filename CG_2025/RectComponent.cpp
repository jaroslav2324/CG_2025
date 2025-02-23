#include "RectComponent.h"

using Vector2 = DirectX::SimpleMath::Vector2;


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
	float avgX = 0;
	float avgY = 0;
	for (auto p : this->points) {
		avgX += p.x;
		avgY += p.y;
	}
	avgX /= this->points.size();
	avgY /= this->points.size();
	// TODO: ”брать костыль, сделать нормальный конструктор
	for (auto& p : this->points) {
		p.x -= avgX;
		p.y -= avgY;
	}

	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();
	centerPosition = toMathCoordsFromClipSpaceCoords(winWidth, winHeight, { avgX, avgY });
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
	auto win = GE::getWindowHandler();
	int winWidth = win->getWinWidth();
	int winHeight = win->getWinHeight();
	Vector2 offset = toClipSpaceCoordsFromMathCoords(winWidth, winHeight, centerPosition);
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

int RectComponent::draw()
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

	context->DrawIndexed(6, 0, 0);
	return 0;
}

void RectComponent::initRect(int x, int y, int w, int h)
{
	if (rect)
		return;

	rect = std::make_shared<DirectX::SimpleMath::Rectangle>(
		DirectX::SimpleMath::Rectangle(x, y, w, h)
	);


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

std::shared_ptr<DirectX::SimpleMath::Rectangle> RectComponent::getRect()
{
	return rect;
}


