#pragma once

#include <iostream>
#include <vector>
#include <format>

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include "util.h"
#include "GameComponent.h"
#include "appExceptions.h"

class TriangleComponent final: public GameComponent {
public:
	TriangleComponent(std::vector<DirectX::XMFLOAT4>&& points,
		std::vector<UINT>&& strides,
		std::vector<UINT>&& offsets,
		const std::wstring& relVertShaderPath,
		const std::wstring& relPixShaderPath,
		std::vector<int>&& indices);
	~TriangleComponent() override;

	int init(Game* game) override;
	int draw() override;
	int update() override;
	void destroyResources() override;
	
private:
	ID3DBlob* vertexByteCode = nullptr;
	ID3DBlob* pixelByteCode = nullptr;
	ID3D11VertexShader* vertexShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	ID3D11InputLayout* layout = nullptr;

	std::vector<DirectX::XMFLOAT4> points;
	std::vector<int> indices;
	std::wstring relVertShaderPath;
	std::wstring relPixShaderPath;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	std::vector<UINT> strides;
	std::vector<UINT> offsets;
	ID3D11RasterizerState* rastState = nullptr;

	void processShaderCompileResult(HRESULT res, ID3DBlob* errorCode) const;
};