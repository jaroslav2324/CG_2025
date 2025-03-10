#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

struct AdditionalData {
	DirectX::SimpleMath::Matrix transformMatrix;
	DirectX::SimpleMath::Vector2 screenCoords;
	DirectX::SimpleMath::Vector2 unused;

};