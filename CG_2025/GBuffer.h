#pragma once

#include <wrl/client.h>
#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

using Microsoft::WRL::ComPtr;

class GBuffer {
public:
	void init();
	void setGBufferRenderTargets() const;
	void bindPixelShaderResourceViews(int startSlot) const;
	void unbindPixelShaderResourceViews(int startSlot) const;
	void clearRenderTargets() const;
private:
	ComPtr<ID3D11Texture2D> depthAmbient = nullptr;
	ComPtr<ID3D11Texture2D> diffuseNormal = nullptr;
	ComPtr<ID3D11Texture2D> specExp = nullptr;
	ComPtr<ID3D11ShaderResourceView> depthAmbientSRV = nullptr;
	ComPtr<ID3D11ShaderResourceView> diffuseNormalSRV = nullptr;
	ComPtr<ID3D11ShaderResourceView> specExpSRV = nullptr;
	ComPtr<ID3D11RenderTargetView> depthAmbientRTV = nullptr;
	ComPtr<ID3D11RenderTargetView> diffuseNormalRTV = nullptr;
	ComPtr<ID3D11RenderTargetView> specExpRTV = nullptr;
};