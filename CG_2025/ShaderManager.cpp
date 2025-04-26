#include "ShaderManager.h"
#include "global.h"

void ShaderManager::compileShader(const std::wstring& filePath, const std::string& entryPoint,
	const std::string& target, const D3D_SHADER_MACRO* macros) {

	std::wstring canonicalFilePath = std::filesystem::canonical(filePath).wstring();

	ID3DBlob* shaderBlob;
	ID3DBlob* errorBlob;
	HRESULT hr = D3DCompileFromFile(
		canonicalFilePath.c_str(), macros, nullptr,
		entryPoint.c_str(), target.c_str(),
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0,
		&shaderBlob, &errorBlob
	);

	if (FAILED(hr)) {
		const char* compileErrors = nullptr;
		// If the shader failed to compile it should have written something to the error message.
		if (errorBlob) {
			compileErrors = (char*)(errorBlob->GetBufferPointer());
		}
		// If there was  nothing in the error message then it simply could not find the shader file itself.
		else
		{
			compileErrors = "No shader file found";
		}
		throw ShaderException(compileErrors);

	}

	shadersMap[canonicalFilePath] = shaderBlob;
	std::vector<D3D_SHADER_MACRO>& savedShaderMacros = shaderMacros[canonicalFilePath];
	while (macros != nullptr && macros->Definition != nullptr) {
		savedShaderMacros.push_back(*macros);
		++macros;
	}
}

ComPtr<ID3D11PixelShader> ShaderManager::compileCreatePixelShader(
	const std::wstring& filePath, const std::string& entryPoint,
	const std::string& target, const D3D_SHADER_MACRO* macros, ID3DBlob** outByteCode /*= nullptr*/)
{
	ID3DBlob* pixelByteCode = getShader(filePath);
	if (!pixelByteCode) {
		compileShader(filePath, entryPoint, target, macros);
		pixelByteCode = getShader(filePath);
	}

	ID3D11Device* device = GE::getGameSubsystem()->getDevice();

	ID3D11PixelShader* rawPixelShader ;
	device->CreatePixelShader(
		pixelByteCode->GetBufferPointer(),
		pixelByteCode->GetBufferSize(),
		nullptr, &rawPixelShader);
	ComPtr<ID3D11PixelShader> pixelShader = rawPixelShader;
	if (outByteCode) {
		*outByteCode = pixelByteCode;
	}
	return pixelShader;
}

ComPtr<ID3D11VertexShader> ShaderManager::compileCreateVertexShader(
	const std::wstring& filePath, const std::string& entryPoint,
	const std::string& target, const D3D_SHADER_MACRO* macros, ID3DBlob** outByteCode /*= nullptr*/)
{
	ID3DBlob* vertexByteCode = getShader(filePath);
	if (!vertexByteCode) {
		compileShader(filePath, entryPoint, target, macros);
		vertexByteCode = getShader(filePath);
	}

	ID3D11Device* device = GE::getGameSubsystem()->getDevice();

	ID3D11VertexShader* rawPixelShader;
	device->CreateVertexShader(
		vertexByteCode->GetBufferPointer(),
		vertexByteCode->GetBufferSize(),
		nullptr, &rawPixelShader);
	ComPtr<ID3D11VertexShader> vertexShader = rawPixelShader;
	if (outByteCode) {
		*outByteCode = vertexByteCode;
	}
	return vertexShader;
}

ID3DBlob* ShaderManager::getShader(const std::wstring& filePath)
{
	std::wstring canonicalFilePath = std::filesystem::canonical(filePath).wstring();
	if (!shadersMap.contains(canonicalFilePath)) {
		return nullptr;
	}
	return shadersMap[canonicalFilePath];
}

std::vector<D3D_SHADER_MACRO> ShaderManager::getShaderMacros(const std::wstring& filePath)
{
	std::wstring canonicalFilePath = std::filesystem::canonical(filePath).wstring();
	if (!shadersMap.contains(canonicalFilePath)) {
		return std::vector<D3D_SHADER_MACRO>();
	}
	return shaderMacros[canonicalFilePath];
}

ShaderManager::~ShaderManager()
{
	for (auto it = shadersMap.begin(); it != shadersMap.end(); it++) {
		releaseIfNotNullptr(&it->second);
	}
}
