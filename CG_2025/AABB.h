#pragma once

#include <directxtk/SimpleMath.h>

struct LightSourceData;

struct AABB
{
	DirectX::SimpleMath::Vector3 min;
	DirectX::SimpleMath::Vector3 max;
};

std::vector<DirectX::SimpleMath::Vector4> generateAABBVertices(const AABB& box);
std::vector<int> generateAABBIndices();

AABB getAABBForPointLight(const LightSourceData& light);
AABB getAABBForSpotLight(const LightSourceData& light);