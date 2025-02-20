#pragma once

#include <cmath>

template <typename DXResourcePtr>
inline void releaseIfNotNullptr(DXResourcePtr* resourcePtr) {
	if ((*resourcePtr) != nullptr) {
		(*resourcePtr)->Release();
		(*resourcePtr) = nullptr;
	}
}

bool apprEqual(float f1, float f2, float epsilon = 1e-6);