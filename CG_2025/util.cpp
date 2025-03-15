#include "util.h"

using DirectX::SimpleMath::Vector2;

bool apprEqual(float f1, float f2, float epsilon /*= 1e-6*/) {
	return (std::abs(f1 - f2) <= epsilon);
}

bool apprEqual(Vector2 v1, Vector2 v2, float epsilon)
{
	return apprEqual(v1.x, v2.x, epsilon) && apprEqual(v1.y, v2.y, epsilon);
}

bool apprEqual(Vector3 v1, Vector3 v2, float epsilon)
{
	return apprEqual(v1.x, v2.x, epsilon) && apprEqual(v1.y, v2.y, epsilon) && apprEqual(v1.z, v2.z, epsilon);
}

bool apprEqual(Vector4 v1, Vector4 v2, float epsilon)
{
	return apprEqual(v1.x, v2.x, epsilon) && apprEqual(v1.y, v2.y, epsilon) && apprEqual(v1.z, v2.z, epsilon) && apprEqual(v1.w, v2.w, epsilon);
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

Vector2 reflectRelativeToNormal(Vector2 reflected, Vector2 normal)
{
	normal.Normalize();
	return reflected - 2 * reflected.Dot(normal) * normal;
}

float generateRandomFloat(float min, float max) {

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(min, max);
	return dist(gen);
}

int generateRandomInt(int min, int max) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(min, max);
	return dist(gen);
}