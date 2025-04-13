#pragma once

#include <random>
#include <vector>
#include <cmath>
#include <d3d.h>
#include <d3d11.h>
#include <directxtk/SimpleMath.h>

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector3;
using DirectX::SimpleMath::Vector4;

template <typename DXResourcePtr>
inline void releaseIfNotNullptr(DXResourcePtr* resourcePtr) {
	if ((*resourcePtr) != nullptr) {
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