#include "MeshComponent.h"
#include "Game.h"

MeshComponent::MeshComponent(std::vector<DirectX::XMFLOAT4>&& points,
	std::vector<UINT>&& strides,
	std::vector<UINT>&& offsets,
	std::vector<int>&& indices)
{
	this->points = std::move(points);
	this->strides = std::move(strides);
	this->offsets = std::move(offsets);
	this->indices = indices;
}

MeshComponent::~MeshComponent()
{
	destroyResources();
}

int MeshComponent::init(Game* game,
	std::shared_ptr<ShaderManager> shaderManager,
	std::shared_ptr<BufferManager> bufferManager,
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

	// TODO: move to shader manager
	ID3D11Device* device = game->getDevice();
	device->CreateVertexShader(
		vertexByteCode->GetBufferPointer(),
		vertexByteCode->GetBufferSize(),
		nullptr, &vertexShader);

	device->CreatePixelShader(
		pixelByteCode->GetBufferPointer(),
		pixelByteCode->GetBufferSize(),
		nullptr, &pixelShader);

	layout = bufferManager->createInputLayout_PosF4_ClrF4(vertexByteCode);
	D3D11_BUFFER_DESC vertexBufDesc = bufferManager->getBasicBufferDescription(points);
	D3D11_SUBRESOURCE_DATA vertexData = bufferManager->getDefaultSubresourceData(points);
	vertexBuffer = bufferManager->createBuffer(vertexBufDesc, vertexData);

	D3D11_BUFFER_DESC indexBufDesc = bufferManager->getBasicBufferDescription(indices);
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA indexData = bufferManager->getDefaultSubresourceData(indices);
	indexBuffer = bufferManager->createBuffer(indexBufDesc, indexData);

	CD3D11_RASTERIZER_DESC rastDesc = {};
	rastDesc.CullMode = D3D11_CULL_NONE;
	rastDesc.FillMode = D3D11_FILL_SOLID;

	HRESULT res = device->CreateRasterizerState(&rastDesc, &rastState);
	if (FAILED(res)) {
		throw GraphicsException("rasterizer state creation failed");
	}
}

int MeshComponent::draw()
{
	ID3D11DeviceContext* context = game->getDeviceContext();
	context->RSSetState(rastState);
	context->IASetInputLayout(layout.Get());
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	ID3D11Buffer* rawVertBuffer = vertexBuffer.Get();
	context->IASetVertexBuffers(0, 1, &rawVertBuffer, strides.data(), offsets.data());
	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	context->DrawIndexed(6, 0, 0);
	return 0;
}

int MeshComponent::update()
{
	return 0;
}

void MeshComponent::destroyResources()
{
	releaseIfNotNullptr(&vertexByteCode);
	releaseIfNotNullptr(&pixelByteCode);
	releaseIfNotNullptr(&vertexShader);
	releaseIfNotNullptr(&pixelShader);

	releaseIfNotNullptr(&rastState);
}
