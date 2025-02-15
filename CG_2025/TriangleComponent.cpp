#include "TriangleComponent.h"
#include "Game.h"

TriangleComponent::TriangleComponent(std::vector<DirectX::XMFLOAT4>&& points,
	std::vector<UINT>&& strides,
	std::vector<UINT>&& offsets,
	const std::wstring& relVertShaderPath,
	const std::wstring& relPixShaderPath,
	std::vector<int>&& indices)
{
	this->points = std::move(points);
	this->strides = std::move(strides);
	this->offsets = std::move(offsets);
	this->relVertShaderPath = relVertShaderPath;
	this->relPixShaderPath = relPixShaderPath;
	this->indices = indices;
}

TriangleComponent::~TriangleComponent()
{
	destroyResources();
}

int TriangleComponent::init(Game* game)
{
	this->game = game;

	ID3DBlob* errorVertexCode = nullptr;
	auto res = D3DCompileFromFile(relVertShaderPath.c_str(),
		nullptr /*macros*/,
		nullptr /*include*/,
		"VSMain",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexByteCode,
		&errorVertexCode);

	processShaderCompileResult(res, errorVertexCode);

	D3D_SHADER_MACRO Shader_Macros[] = {
		"TEST", "1", "TCOLOR",
		"float4(0.0f, 1.0f, 0.0f, 1.0f)",
		nullptr, nullptr 
	};

	ID3DBlob* errorPixelCode;
	res = D3DCompileFromFile(relPixShaderPath.c_str(),
		Shader_Macros /*macros*/,
		nullptr /*include*/,
		"PSMain", "ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelByteCode,
		&errorPixelCode
	);

	processShaderCompileResult(res, errorPixelCode);

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

	res = device->CreateRasterizerState(&rastDesc, &rastState);
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

void TriangleComponent::processShaderCompileResult(HRESULT res, ID3DBlob* errorCode) const
{
	if (FAILED(res)) {

		// If the shader failed to compile it should have written something to the error message.
		if (errorCode) {
			const char* compileErrors = (char*)(errorCode->GetBufferPointer());
			throw ShaderException(compileErrors);
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			throw ShaderException("No shader file found");
		}
	}
}
