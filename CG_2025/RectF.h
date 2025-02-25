#pragma once

#include <directxtk/SimpleMath.h>

struct RectF {
	float x;
	float y;
	float w;
	float h;
	DirectX::SimpleMath::Rectangle toRectangle();

};