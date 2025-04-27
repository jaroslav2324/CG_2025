#pragma once

#include <random>
#include <vector>
#include <cmath>
#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

#include "Plane.h"

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

struct AABB;

template <typename DXResourcePtr>
inline void releaseIfNotNullptr(DXResourcePtr* resourcePtr) {
	if (resourcePtr && (*resourcePtr)) {
		(*resourcePtr)->Release();
		(*resourcePtr) = nullptr;
	}
}

bool apprEqual(float f1, float f2, float epsilon = 1e-6);
bool apprEqual(Vector2 v1, Vector2 v2, float epsilon = 1e-6);
bool apprEqual(Vector3 v1, Vector3 v2, float epsilon = 1e-6);
bool apprEqual(Vector4 v1, Vector4 v2, float epsilon = 1e-6);

DirectX::SimpleMath::Vector2 toClipSpaceCoordsFromMathCoords(int winWidth, int winHeight, DirectX::SimpleMath::Vector2 coords);
DirectX::SimpleMath::Vector2 toMathCoordsFromClipSpaceCoords(int winWidth, int winHeight, DirectX::SimpleMath::Vector2 coords);

DirectX::SimpleMath::Vector2 reflectRelativeToNormal(DirectX::SimpleMath::Vector2 reflected, DirectX::SimpleMath::Vector2 normal);

float generateRandomFloat(float min, float max);
int generateRandomInt(int min, int max);

std::vector<DirectX::SimpleMath::Vector4> getFrustumCornersWorldSpace(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);
std::vector<Plane> getFrustumPlanes(const std::vector<DirectX::SimpleMath::Vector4>& corners);

enum FrustumIntersection
{
	INSIDE_FRUSTUM,
	INTERSECTS_FAR_PLANE,
	OUTSIDE_FRUSTUM
};

FrustumIntersection TestAABBFrustum(const AABB& aabb, const std::vector<Plane>& planes);