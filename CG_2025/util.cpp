#include "util.h"

using DirectX::SimpleMath::Vector2;

bool apprEqual(float f1, float f2, float epsilon /*= 1e-6*/) {
	return (std::abs(f1 - f2) <= epsilon);
}

Vector2 toClipSpaceCoordsFromMathCoords(int winWidth, int winHeight, Vector2 coords)
{
	coords.x = (coords.x / winWidth) * 2.0f - 1.0f;
	coords.y = (coords.y / winHeight) * 2.0f - 1.0f;
	return coords;
}

Vector2 toMathCoordsFromClipSpaceCoords(int winWidth, int winHeight, Vector2 coords)
{
	coords.x = (coords.x + 1.0f) / 2.0f * winWidth;
	coords.y = (coords.y + 1.0f) / 2.0f * winHeight;
	return coords;
}
