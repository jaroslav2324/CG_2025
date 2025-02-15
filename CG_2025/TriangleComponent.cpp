#include "TriangleComponent.h"
#include "Game.h"

TriangleComponent::TriangleComponent(std::vector<DirectX::XMFLOAT4>&& points,
	std::vector<UINT>&& strides,
	std::vector<UINT>&& offsets,
	std::vector<int>&& indices)
{
	this->points = std::move(points);
	this->strides = std::move(strides);
	this->offsets = std::move(offsets);
	this->indices = indices;
}

TriangleComponent::~TriangleComponent()
{
	destroyResources();
}

int TriangleComponent::init(Game* game, 
	std::shared_ptr<ShaderManager> shaderManager,
	const std::wstring& vertShaderPath,
	const std::wstring& pixShaderPath)
{
	this->game = game;

	
	if (!shaderManager->getShader(vertShaderPath)) {
		shaderManager->compileShader(vertShaderPath, "VSMain", "vs_5_0", nullptr);
	};
	vertexByteCode = shaderManager->getShader(vertShaderPath);

	D3D_SHADER_MACRO shaderMacros[] = {
		"TEST", "1", "TCOLOR",
		"float4(0.0f, 1.0f, 0.0f, 1.0f)",
		nullptr, nullptr 
	};

	if (!shaderManager->getShader(pixShaderPath)) {
		shaderManager->compileShader(pixShaderPath, "PSMain", "ps_5_0", shaderMacros);
	};
	pixelByteCode = shaderManager->getShader(pixShaderPath);

	ID3D11Device* device = game->getDevice();
	device->CreateVertexShader(
		vertexByteCode->GetBufferPointer(),
		vertexByteCode->GetBufferSize(),
		nullptr, &vertexShader);

	device->CreatePixelShader(
		pixelByteCode->GetBufferPointer(),
		pixelByteCode->GetBufferSize(),
		nullptr, &pixelShader);

	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
	D3D11_INPUT_ELEMENT_DESC {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		0,
		D3D11_INPUT_PER_VERTEX_DATA,
		0},
	D3D11_INPUT_ELEMENT_DESC {
		"COLOR",
		0,
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0,
		D3D11_APPEND_ALIGNED_ELEMENT,
		D3D11_INPUT_PER_VERTEX_DATA,
		0}
	};

	device->CreateInputLayout(
		inputElements,
		2,
		vertexByteCode->GetBufferPointer(),
		vertexByteCode->GetBufferSize(),
		&layout);

	D3D11_BUFFER_DESC vertexBufDesc = {};
	vertexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufDesc.CPUAccessFlags = 0;
	vertexBufDesc.MiscFlags = 0;
	vertexBufDesc.StructureByteStride = 0;
	vertexBufDesc.ByteWidth = sizeof(DirectX::XMFLOAT4) * std::size(points);

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = points.data();
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&vertexBufDesc, &vertexData, &vertexBuffer);

	D3D11_BUFFER_DESC indexBufDesc = {};
	indexBufDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufDesc.CPUAccessFlags = 0;
	indexBufDesc.MiscFlags = 0;
	indexBufDesc.StructureByteStride = 0;
	indexBufDesc.ByteWidth = sizeof(int) * indices.size();

	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices.data();
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	device->CreateBuffer(&indexBufDesc, &indexData, &indexBuffer);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	HRESULT res = device->CreateRasterizerState(&rastDesc, &rastState);
	if (FAILED(res)) {
		throw GraphicsException("rasterizer state creation failed");
	}
}

int TriangleComponent::draw()
{
	ID3D11DeviceContext* context = game->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout);
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	context->IASetVertexBuffers(0, 1, &vertexBuffer, strides.data(), offsets.data());
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->DrawIndexed(6, 0, 0);
	return 0;
}

int TriangleComponent::update()
{
	return 0;
}

void TriangleComponent::destroyResources()
{
	releaseIfNotNullptr(&vertexByteCode);
	releaseIfNotNullptr(&pixelByteCode);
	releaseIfNotNullptr(&vertexShader);
	releaseIfNotNullptr(&pixelShader);

	releaseIfNotNullptr(&layout);
	releaseIfNotNullptr(&vertexBuffer);
	releaseIfNotNullptr(&indexBuffer);

	releaseIfNotNullptr(&rastState);
}
