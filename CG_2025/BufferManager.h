#pragma once

#include <vector>

#include <wrl/client.h>
#include <d3d.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class BufferManager {
public:
	ComPtr <ID3D11InputLayout> createInputLayout(
		D3D11_INPUT_ELEMENT_DESC* layoutDesc,
		int countElements,
		ComPtr<ID3DBlob> shaderByteCode);

	ComPtr <ID3D11InputLayout> createInputLayout_PosF4_ClrF4(ComPtr<ID3DBlob> shaderByteCode);
	ComPtr <ID3D11InputLayout> createInputLayout_PosF4_NormF4_TexF4_AddF4(ComPtr<ID3DBlob> shaderByteCode);

	template <typename T>
	ComPtr <ID3D11Buffer> createVertexBuffer(const std::vector<T>& data);
	template <typename T>
	ComPtr <ID3D11Buffer> createIndexBuffer(const std::vector<T>& data);
	template<typename T>
	ComPtr <ID3D11Buffer> createConstDynamicBufferCPUWrite(const std::vector<T>& data);
	ComPtr <ID3D11Buffer> createBuffer(const D3D11_BUFFER_DESC buffDesc,
		const D3D11_SUBRESOURCE_DATA subresourceData);

	template <typename T>
	D3D11_SUBRESOURCE_DATA getDefaultSubresourceData(const std::vector<T>& data) const;

	template <typename T>
	D3D11_BUFFER_DESC getBasicBufferDescription(const std::vector<T>& data) const;
};

template<typename T>
inline ComPtr<ID3D11Buffer> BufferManager::createVertexBuffer(const std::vector<T>& data)
{
	return createBuffer(getBasicBufferDescription(data), getDefaultSubresourceData(data));
}

template<typename T>
inline ComPtr<ID3D11Buffer> BufferManager::createIndexBuffer(const std::vector<T>& data)
{
	D3D11_BUFFER_DESC indexBufDesc = getBasicBufferDescription(data);
	indexBufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	return createBuffer(indexBufDesc, getDefaultSubresourceData(data));
}

template<typename T>
inline ComPtr<ID3D11Buffer> BufferManager::createConstDynamicBufferCPUWrite(const std::vector<T>& data)
{
	auto subresData = getDefaultSubresourceData<T>(data);
	auto buffDesc = getBasicBufferDescription<T>(data);
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	return createBuffer(buffDesc, subresData);
}

template<typename T>
inline D3D11_SUBRESOURCE_DATA BufferManager::getDefaultSubresourceData(const std::vector<T>& data) const
{
	D3D11_SUBRESOURCE_DATA subresourceData = {};
	subresourceData.pSysMem = data.data();
	subresourceData.SysMemPitch = 0;
	subresourceData.SysMemSlicePitch = 0;
	return subresourceData;
}

template<typename T>
inline D3D11_BUFFER_DESC BufferManager::getBasicBufferDescription(const std::vector<T>& data) const
{
	D3D11_BUFFER_DESC buffDesc = {};
	buffDesc.Usage = D3D11_USAGE_DEFAULT;
	buffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buffDesc.CPUAccessFlags = 0;
	buffDesc.MiscFlags = 0;
	buffDesc.StructureByteStride = 0;
	buffDesc.ByteWidth = sizeof(T) * std::size(data);
	return buffDesc;
}
