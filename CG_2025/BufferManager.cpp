#include "BufferManager.h"
#include "global.h"

ComPtr<ID3D11InputLayout> BufferManager::createInputLayout_PosF4_ClrF4(ComPtr<ID3DBlob> shaderByteCode)
{
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

	auto device = GE::getGameSubsystem()->getDevice();
	ComPtr<ID3D11InputLayout> layout;
	device->CreateInputLayout(
		inputElements,
		2,
		shaderByteCode->GetBufferPointer(),
		shaderByteCode->GetBufferSize(),
		&layout);

	return layout;
}

ComPtr<ID3D11Buffer> BufferManager::createBuffer(const D3D11_BUFFER_DESC buffDesc,
	const D3D11_SUBRESOURCE_DATA subresourceData)
{
	ID3D11Buffer* buffer = nullptr;
	auto device = GE::getGameSubsystem()->getDevice();
	device->CreateBuffer(&buffDesc, &subresourceData, &buffer);
	return ComPtr<ID3D11Buffer>(buffer);
}
