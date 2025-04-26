#include "util.h"
#include "Plane.h"
#include "AABB.h"

using DirectX::SimpleMath::Vector2;
using DirectX::SimpleMath::Vector4;
using DirectX::SimpleMath::Matrix;

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

std::vector<DirectX::SimpleMath::Vector4> getFrustumCornersWorldSpace(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
	const auto viewProj = view * proj;
	const auto inv = viewProj.Invert();

	std::vector<Vector4> frustumCorners;
	frustumCorners.reserve(8);
	for (int z = 0; z < 2; ++z)
	{
		for (int x = 0; x < 2; ++x)
		{
			for (int y = 0; y < 2; ++y)
			{
				const Vector4 pt = Vector4::Transform(Vector4(
					2.0f * x - 1.0f,
					2.0f * y - 1.0f,
					z,
					1.0f
				), inv);
				frustumCorners.push_back(pt / pt.w);
			}
		}
	}

	return frustumCorners;
}

std::vector<Plane> getFrustumPlanes(const std::vector<DirectX::SimpleMath::Vector4>& corners)
{
	std::vector<Plane> planes;

	auto createPlane = [](const DirectX::SimpleMath::Vector4& a, const DirectX::SimpleMath::Vector4& b, const DirectX::SimpleMath::Vector4& c) -> Plane
		{
			Vector3 ab = Vector3(b) - Vector3(a);
			Vector3 ac = Vector3(c) - Vector3(a);
			auto normal = ab.Cross(ac);
			normal.Normalize();
			float d = -normal.Dot(Vector3(a));
			return { normal, d };
		};

	// corners:
	// 0: (-1, -1, 0)
	// 1: (-1, 1, 0)
	// 2: (1, -1, 0)
	// 3: (1, 1, 0)
	// 4: (-1, -1, 1)
	// 5: (-1, 1, 1)
	// 6: (1, -1, 1)
	// 7: (1, 1, 1)

	// Near plane
	planes.push_back(createPlane(corners[0], corners[1], corners[2]));
	// Far plane
	planes.push_back(createPlane(corners[5], corners[4], corners[7]));
	// Left
	planes.push_back(createPlane(corners[4], corners[0], corners[5]));
	// Right
	planes.push_back(createPlane(corners[2], corners[6], corners[3]));
	// Top
	planes.push_back(createPlane(corners[1], corners[5], corners[3]));
	// Bottom
	planes.push_back(createPlane(corners[4], corners[0], corners[6]));

	return planes;
}

FrustumIntersection TestAABBFrustum(const AABB& aabb, const std::vector<Plane>& planes)
{
	bool intersectsFar = false;

	for (int i = 0; i < planes.size(); ++i)
	{
		const auto& plane = planes[i];

		DirectX::SimpleMath::Vector3 positiveVertex = aabb.min;
		if (plane.normal.x >= 0) positiveVertex.x = aabb.max.x;
		if (plane.normal.y >= 0) positiveVertex.y = aabb.max.y;
		if (plane.normal.z >= 0) positiveVertex.z = aabb.max.z;

		if (plane.distance(positiveVertex) < 0)
		{
			if (i == 1) // Far plane
				intersectsFar = true;
			else
				return OUTSIDE_FRUSTUM;
		}
	}

	return intersectsFar ? INTERSECTS_FAR_PLANE : INSIDE_FRUSTUM;
}