#pragma once

#include <array>
#include <functional>
#include <directxtk/SimpleMath.h>

#include "global.h"
#include "MeshComponent.h"

class RectComponent : public MeshComponent {
public:
	using Vector2 = DirectX::SimpleMath::Vector2;

	RectComponent(std::vector<DirectX::XMFLOAT4>&& points,
		std::vector<UINT>&& strides,
		std::vector<UINT>&& offsets,
		std::vector<int>&& indices);
	RectComponent(std::array<float, 4> points);
	//TODO: add constructior with center, width and height

	int init(Game* game, std::shared_ptr<ShaderManager>,
		std::shared_ptr<BufferManager> bufferManager,
		const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;

	int update(float deltaTime) override;
	int draw() override;
	void initRect(int x, int y, int w, int h);
	void setDirection(Vector2 direction);
	void setPosition(Vector2 pos);
	void setVelocity(float vel);
	Vector2 getDirection() const;
	Vector2 getPosition() const;
	float getVelocity() const;
	std::shared_ptr<DirectX::SimpleMath::Rectangle> getRect();

private:
	// std::function <void(void)> 
	std::shared_ptr<DirectX::SimpleMath::Rectangle> rect = nullptr;
	Vector2 direction = { 1, 0 };
	float velocity = 0;
	Vector2 centerPosition = { 0, 0 };
	DirectX::XMFLOAT4 centerPointOffset = { 0.0f, 0.0f, 0.0f, 0.0f };
	ComPtr <ID3D11Buffer> centerOffsetBuffer = nullptr;
};