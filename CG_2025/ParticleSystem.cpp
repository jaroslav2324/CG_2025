#include "ParticleSystem.h"

#include "global.h"

const UINT BITONIC_BLOCK_SIZE = 512;
const UINT TRANSPOSE_BLOCK_SIZE = 16;
const UINT MATRIX_WIDTH = BITONIC_BLOCK_SIZE;

void ParticleSystem::init()
{
	auto device = GE::getGameSubsystem()->getDevice();
	auto shaderManager = GE::getShaderManager();
	auto bufferManager = GE::getBufferManager();

	bitonicSortShader = shaderManager->compileCreateShader<ID3D11ComputeShader>(device, L"./shaders/sortBitonicShader.hlsl", "BitonicSort", "cs_5_0", nullptr);
	bitonicTransposeShader = shaderManager->compileCreateShader<ID3D11ComputeShader>(device, L"./shaders/sortTransposeShader.hlsl", "MatrixTranspose", "cs_5_0", nullptr);
	auto bufDesc = bufferManager->getBasicBufferDescription(cbSortData);
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	auto subresData = bufferManager->getDefaultSubresourceData(cbSortData);
	constSortBuf = bufferManager->createBuffer(bufDesc, subresData);
}

void ParticleSystem::swapSortListBuffers()
{
	ComPtr<ID3D11Buffer> sortListBufferTmp = sortListBuffer;
	ComPtr<ID3D11UnorderedAccessView> sortListBufferUAVTmp = sortListBufferUAV;
	ComPtr<ID3D11ShaderResourceView> sortListBufferSRVTmp = sortListBufferSRV;
	sortListBuffer = sortListBuffer2;
	sortListBufferUAV = sortListBufferUAV2;
	sortListBufferSRV = sortListBufferSRV2;
	sortListBuffer2 = sortListBufferTmp;
	sortListBufferUAV2 = sortListBufferUAVTmp;
	sortListBufferSRV2 = sortListBufferSRVTmp;
}

//--------------------------------------------------------------------------------------
// Helper to set the compute shader constants
//--------------------------------------------------------------------------------------
void ParticleSystem::setConstants(UINT iLevel, UINT iLevelMask, UINT iWidth, UINT iHeight)
{
	auto context = GE::getGameSubsystem()->getDeviceContext();
	CBSort cb = { iLevel, iLevelMask, iWidth, iHeight };
	context->UpdateSubresource(constSortBuf.Get(), 0, nullptr, &cb, 0, 0);
	context->CSSetConstantBuffers(0, 1, constSortBuf.GetAddressOf());
}

//--------------------------------------------------------------------------------------
// GPU Bitonic Sort
//--------------------------------------------------------------------------------------
void ParticleSystem::sort()
{
	ID3D11UnorderedAccessView* pViewnullptr = nullptr;
	auto context = GE::getGameSubsystem()->getDeviceContext();
	const UINT MATRIX_HEIGHT = maxParticles / BITONIC_BLOCK_SIZE;

	//swapSortListBuffers();

	// Sort the data
	// First sort the rows for the levels <= to the block size
	for (UINT level = 2; level <= BITONIC_BLOCK_SIZE; level = level * 2)
	{
		setConstants(level, level, MATRIX_HEIGHT, MATRIX_WIDTH);

		// Sort the row data
		context->CSSetUnorderedAccessViews(0, 1, sortListBufferUAV.GetAddressOf(), nullptr);
		context->CSSetShader(bitonicSortShader.Get(), nullptr, 0);
		context->Dispatch(maxParticles / BITONIC_BLOCK_SIZE, 1, 1);
	}

	// Then sort the rows and columns for the levels > than the block size
	// Transpose. Sort the Columns. Transpose. Sort the Rows.
	for (UINT level = (BITONIC_BLOCK_SIZE * 2); level <= maxParticles; level = level * 2)
	{
		setConstants(level / BITONIC_BLOCK_SIZE, (level & ~maxParticles) / BITONIC_BLOCK_SIZE, MATRIX_WIDTH, MATRIX_HEIGHT);

		// Transpose the data from buffer 1 into buffer 2
		context->CSSetUnorderedAccessViews(0, 1, sortListBufferUAV.GetAddressOf(), nullptr);
		context->CSSetUnorderedAccessViews(1, 1, sortListBufferUAV2.GetAddressOf(), nullptr);
		context->CSSetShader(bitonicTransposeShader.Get(), nullptr, 0);
		context->Dispatch(MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, 1);
		context->CSSetUnorderedAccessViews(1, 1, &pViewnullptr, nullptr);
		//std::cout << MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE << std::endl;

		// Sort the transposed column data
		context->CSSetUnorderedAccessViews(0, 1, sortListBufferUAV2.GetAddressOf(), nullptr);
		context->CSSetShader(bitonicSortShader.Get(), nullptr, 0);
		context->Dispatch(maxParticles / BITONIC_BLOCK_SIZE, 1, 1);

		setConstants(BITONIC_BLOCK_SIZE, level, MATRIX_HEIGHT, MATRIX_WIDTH);

		// Transpose the data from buffer 2 back into buffer 1
		context->CSSetUnorderedAccessViews(0, 1, sortListBufferUAV2.GetAddressOf(), nullptr);
		context->CSSetUnorderedAccessViews(1, 1, sortListBufferUAV.GetAddressOf(), nullptr);
		context->CSSetShader(bitonicTransposeShader.Get(), nullptr, 0);
		context->Dispatch(MATRIX_HEIGHT / TRANSPOSE_BLOCK_SIZE, MATRIX_WIDTH / TRANSPOSE_BLOCK_SIZE, 1);
		context->CSSetUnorderedAccessViews(1, 1, &pViewnullptr, nullptr);

		// Sort the row data
		context->CSSetUnorderedAccessViews(0, 1, sortListBufferUAV.GetAddressOf(), nullptr);
		context->CSSetShader(bitonicSortShader.Get(), nullptr, 0);
		context->Dispatch(maxParticles / BITONIC_BLOCK_SIZE, 1, 1);

		context->CSSetUnorderedAccessViews(0, 1, &pViewnullptr, nullptr);
	}

	context->CSSetUnorderedAccessViews(0, 1, &pViewnullptr, nullptr);
}
