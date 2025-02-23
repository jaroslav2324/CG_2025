#pragma once

#include <d3d.h>
#include <d3d11.h>
#include <directxmath.h>
#include <directxtk/SimpleMath.h>
#include <cmath>

template <typename DXResourcePtr>
inline void releaseIfNotNullptr(DXResourcePtr* resourcePtr) {
	if ((*resourcePtr) != nullptr) {
		(*resourcePtr)->Release();
		(*resourcePtr) = nullptr;
	}
}

bool apprEqual(float f1, float f2, float epsilon = 1e-6);

DirectX::SimpleMath::Vector2 toClipSpaceCoordsFromMathCoords(int winWidth, int winHeight, DirectX::SimpleMath::Vector2 coords);
DirectX::SimpleMath::Vector2 toMathCoordsFromClipSpaceCoords(int winWidth, int winHeight, DirectX::SimpleMath::Vector2 coords);