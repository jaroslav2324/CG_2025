#pragma once

#include <iostream>
#include <unordered_map>
#include <string>
#include <format>
#include <filesystem>

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "util.h"
#include "appExceptions.h"

class ShaderManager {
public:
	explicit ShaderManager(ID3D11Device* device) : device(device) {};
	void compileShader(const std::wstring& filePath, const std::string& entryPoint,
					const std::string& target, const D3D_SHADER_MACRO* macros);
	ID3DBlob* getShader(const std::wstring& filePath);
	std::vector<D3D_SHADER_MACRO> getShaderMacros(const std::wstring& filePath);
	~ShaderManager();
private:
	ID3D11Device* device = nullptr;
	std::unordered_map<std::wstring, ID3DBlob*> shadersMap;
	std::unordered_map<std::wstring, std::vector<D3D_SHADER_MACRO>> shaderMacros;
};