#include "AABB.h"
#include "LightSource.h"

std::vector<DirectX::SimpleMath::Vector4> generateAABBVertices(const AABB& box)
{
	using namespace DirectX::SimpleMath;
	std::vector<Vector4> vertices(8);

	vertices[0] = Vector4(box.min.x, box.min.y, box.min.z, 0.0f); // 0
	vertices[1] = Vector4(box.max.x, box.min.y, box.min.z, 0.0f); // 1
	vertices[2] = Vector4(box.max.x, box.max.y, box.min.z, 0.0f); // 2
	vertices[3] = Vector4(box.min.x, box.max.y, box.min.z, 0.0f); // 3
	vertices[4] = Vector4(box.min.x, box.min.y, box.max.z, 0.0f); // 4
	vertices[5] = Vector4(box.max.x, box.min.y, box.max.z, 0.0f); // 5
	vertices[6] = Vector4(box.max.x, box.max.y, box.max.z, 0.0f); // 6
	vertices[7] = Vector4(box.min.x, box.max.y, box.max.z, 0.0f); // 7

	return vertices;
}

std::vector<int> generateAABBIndices()
{
	// 12 треугольников по 3 индекса = 36 индексов
	std::vector<int> indices = {
		// Front face
		0, 1, 2,
		2, 3, 0,
		// Back face
		5, 4, 7,
		7, 6, 5,
		// Left face
		4, 0, 3,
		3, 7, 4,
		// Right face
		1, 5, 6,
		6, 2, 1,
		// Top face
		3, 2, 6,
		6, 7, 3,
		// Bottom face
		4, 5, 1,
		1, 0, 4
	};

	return indices;
}

AABB getAABBForPointLight(const LightSourceData& light)
{
	using namespace DirectX::SimpleMath;

	Vector3 pos = Vector3(light.position.x, light.position.y, light.position.z);
	float radius = light.shineDistance;

	AABB box;
	box.min = pos - Vector3(radius, radius, radius);
	box.max = pos + Vector3(radius, radius, radius);
	return box;
}

AABB getAABBForSpotLight(const LightSourceData& light)
{
	using namespace DirectX::SimpleMath;

	Vector3 pos = Vector3(light.position.x, light.position.y, light.position.z);
	Vector3 dir = Vector3(light.direction.x, light.direction.y, light.direction.z);
	dir.Normalize();

	float length = light.shineDistance;
	float radiusAtBase = tanf(light.angle) * length;

	AABB box;
	Vector3 baseCenter = pos + dir * length;

	Vector3 minPoint = Vector3::Min(pos - Vector3(radiusAtBase, radiusAtBase, radiusAtBase),
		baseCenter - Vector3(radiusAtBase, radiusAtBase, radiusAtBase));
	Vector3 maxPoint = Vector3::Max(pos + Vector3(radiusAtBase, radiusAtBase, radiusAtBase),
		baseCenter + Vector3(radiusAtBase, radiusAtBase, radiusAtBase));

	box.min = minPoint;
	box.max = maxPoint;
	return box;
}
