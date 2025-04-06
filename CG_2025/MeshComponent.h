#pragma once

#include <iostream>
#include <vector>
#include <format>

#include <d3d.h>
#include <d3d11.h>
#include <directxmath.h>
#include <directxtk/SimpleMath.h>
#include <directxtk/Model.h>
#include <directxtk/Effects.h>
#include <directxtk/CommonStates.h>

#include "util.h"
#include "GameComponent.h"
#include "appExceptions.h"

class MeshComponent : public GameComponent {
public:
	MeshComponent();
	MeshComponent(std::vector<DirectX::XMFLOAT4>&& points,
		std::vector<UINT>&& strides,
		std::vector<UINT>&& offsets,
		std::vector<int>&& indices);
	~MeshComponent() override;

	int init(const std::wstring& vertShaderPath,
		const std::wstring& pixShaderPath) override;
	int draw(float deltaTime) override;
	int drawShadow() override;
	int update(float deltaTime) override;
	void destroyResources() override;
	void createShadowVertexShader(const std::wstring& path);
	void createShadowPixelShader(const std::wstring& path);
protected:
	//TODO: ComPtr
	ID3DBlob* vertexByteCode = nullptr;
	ID3DBlob* pixelByteCode = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	ComPtr<ID3D11InputLayout> layout = nullptr;

	std::vector<DirectX::XMFLOAT4> points;
	std::vector<int> indices;

	ComPtr <ID3D11Buffer> vertexBuffer = nullptr;
	ComPtr <ID3D11Buffer> indexBuffer = nullptr;

	std::vector<UINT> strides;
	std::vector<UINT> offsets;
	ID3D11RasterizerState* rastState = nullptr;

	ID3D11VertexShader* vertexShadowShader = nullptr;
	ID3DBlob* vertexShadowByteCode = nullptr;
	ID3D11PixelShader* pixelShadowShader = nullptr;
	ID3DBlob* pixelShadowByteCode = nullptr;
};