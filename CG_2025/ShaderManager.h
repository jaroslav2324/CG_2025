#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <format>
#include <filesystem>

#include "wrl/client.h"
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "util.h"
#include "appExceptions.h"

using Microsoft::WRL::ComPtr;

class ShaderManager {
public:
	void compileShader(const std::wstring& filePath, const std::string& entryPoint,
		const std::string& target, const D3D_SHADER_MACRO* macros);
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