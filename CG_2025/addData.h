#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

#include "Material.h"

struct AdditionalData {
	DirectX::SimpleMath::Matrix transformMatrix;
	DirectX::SimpleMath::Matrix scaleMatrix;
	DirectX::SimpleMath::Matrix rotationMatrix;
	DirectX::SimpleMath::Matrix translationMatrix;
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;
	DirectX::SimpleMath::Vector2 screenCoords;
	DirectX::SimpleMath::Vector2 nearFar;
	Material material;
	DirectX::SimpleMath::Vector4 camPos;
};

enum class LightSourceAdditionalFlags {
	NONE = 0,
	FORCE_DRAW_SCREEN_ALIGNED_SQUAD = 1
};

struct LightSourceAddDataDeferred {
	DirectX::SimpleMath::Matrix viewMatrix;
	DirectX::SimpleMath::Matrix projectionMatrix;
	DirectX::SimpleMath::Matrix inverseViewMatrix;
	DirectX::SimpleMath::Matrix inverseProjectionMatrix;
	DirectX::SimpleMath::Vector4 camPos;
	DirectX::SimpleMath::Vector4 camDir;
	int flags;
	int padding[3];
};

struct ShadowPassLightAddData
{
	DirectX::SimpleMath::Matrix viewMatrix; 
	DirectX::SimpleMath::Matrix projMatrix;
	DirectX::SimpleMath::Vector4 lightSourcePos;
};