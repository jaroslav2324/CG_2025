#pragma once

#include "directxtk/SimpleMath.h"

struct Plane
{
	Plane(const DirectX::SimpleMath::Vector3 normal, float d): d(d), normal(normal) {};
	DirectX::SimpleMath::Vector3 normal;
	float d;

	float distance(const DirectX::SimpleMath::Vector3& point) const
	{
		return normal.Dot(point) + d;
	}
};