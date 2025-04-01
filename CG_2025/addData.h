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
	DirectX::SimpleMath::Vector2 unused;
	Material material;
	DirectX::SimpleMath::Vector4 camPos;
};