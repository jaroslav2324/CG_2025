#pragma once

#include <vector>

#include <wrl/client.h>
#include <d3d.h>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;

class BufferManager {
public:
	explicit BufferManager(ID3D11Device* device) : device(device) {};

	ComPtr <ID3D11InputLayout> createInputLayout(
		D3D11_INPUT_ELEMENT_DESC* layoutDesc,
		int countElements,
		ComPtr<ID3DBlob> shaderByteCode);

	ComPtr <ID3D11InputLayout> createInputLayout_PosF4_ClrF4(ComPtr<ID3DBlob> shaderByteCode);

	ComPtr <ID3D11Buffer> createBuffer(const D3D11_BUFFER_DESC buffDesc,
		const D3D11_SUBRESOURCE_DATA subresourceData);

	template <typename T>
	D3D11_SUBRESOURCE_DATA getDefaultSubresourceData(const std::vector<T>& data) const;

	template <typename T>
	D3D11_BUFFER_DESC getBasicBufferDescription(const std::vector<T>& data) const;
private:
	ID3D11Device* device = nullptr;

};

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
