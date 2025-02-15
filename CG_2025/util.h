#pragma once

template <typename DXResourcePtr>
inline void releaseIfNotNullptr(DXResourcePtr* resourcePtr) {
	if ((*resourcePtr) != nullptr) {
		(*resourcePtr)->Release();
		(*resourcePtr) = nullptr;
	}
}