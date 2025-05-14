#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <format>
#include <filesystem>
#include <type_traits>

#include "wrl/client.h"
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "util.h"
#include "appExceptions.h"

using Microsoft::WRL::ComPtr;

template<typename T>
constexpr bool always_false = false;

class ShaderManager {
public:
	void compileShader(const std::wstring& filePath, const std::string& entryPoint,
		const std::string& target, const D3D_SHADER_MACRO* macros);
	template <typename shader_t>
	ComPtr<shader_t> compileCreateShader(ID3D11Device* device, const std::wstring& filePath, const std::string& entryPoint,
		const std::string& target, const D3D_SHADER_MACRO* macros, ID3DBlob** outByteCode = nullptr);
	ComPtr<ID3D11PixelShader> compileCreatePixelShader(const std::wstring& filePath, const std::string& entryPoint,
		const std::string& target, const D3D_SHADER_MACRO* macros, ID3DBlob** outByteCode = nullptr);
	ComPtr<ID3D11VertexShader> compileCreateVertexShader(const std::wstring& filePath, const std::string& entryPoint,
		const std::string& target, const D3D_SHADER_MACRO* macros, ID3DBlob** outByteCode = nullptr);
	ComPtr<ID3D11ComputeShader> compileCreateComputeShader(const std::wstring& filePath, const std::string& entryPoint,
		const std::string& target, const D3D_SHADER_MACRO* macros, ID3DBlob** outByteCode = nullptr);
	ID3DBlob* getShader(const std::wstring& filePath);
	std::vector<D3D_SHADER_MACRO> getShaderMacros(const std::wstring& filePath);
	~ShaderManager();
private:
	std::unordered_map<std::wstring, ID3DBlob*> shadersMap;
	std::unordered_map<std::wstring, std::vector<D3D_SHADER_MACRO>> shaderMacros;
};

template <typename shader_t>
ComPtr<shader_t>
ShaderManager::compileCreateShader(ID3D11Device* device, const std::wstring& filePath, const std::string& entryPoint, const std::string& target, const D3D_SHADER_MACRO* macros, ID3DBlob** outByteCode /*= nullptr*/)
{
	ID3DBlob* ShaderByteCode = getShader(filePath);
	if (!ShaderByteCode) {
		compileShader(filePath, entryPoint, target, macros);
		ShaderByteCode = getShader(filePath);
	}

	shader_t* rawShader;
	if constexpr (std::is_same_v<shader_t, ID3D11PixelShader>) {
		device->CreatePixelShader(
			ShaderByteCode->GetBufferPointer(),
			ShaderByteCode->GetBufferSize(),
			nullptr, &rawShader);
	}
	else if constexpr (std::is_same_v<shader_t, ID3D11VertexShader>) {
		device->CreateVertexShader(
			ShaderByteCode->GetBufferPointer(),
			ShaderByteCode->GetBufferSize(),
			nullptr, &rawShader);
	}
	else if constexpr (std::is_same_v<shader_t, ID3D11ComputeShader>) {
		device->CreateComputeShader(
			ShaderByteCode->GetBufferPointer(),
			ShaderByteCode->GetBufferSize(),
			nullptr, &rawShader);
	}
	else if constexpr (std::is_same_v<shader_t, ID3D11GeometryShader>) {
		device->CreateGeometryShader(
			ShaderByteCode->GetBufferPointer(),
			ShaderByteCode->GetBufferSize(),
			nullptr, &rawShader);
	}
	else {
		static_assert(always_false<shader_t>, "Unknown shader type in compileCreateShader");
	}
	ComPtr<shader_t> shader = rawShader;
	if (outByteCode) {
		*outByteCode = ShaderByteCode;
	}
	return shader;
}
