#include "Material.h"

using DirectX::SimpleMath::Vector4;

Material getBlackPlasticMaterial()
{
    Material mat;
    mat.ambient = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
    mat.diffuse = Vector4(0.01f, 0.01f, 0.01f, 1.0f);
    mat.speculiar = Vector4(0.5f, 0.5f, 0.5f, 1.0f);;
    mat.exponent.x = 32.0f;
    return mat;
}

Material getBrassMaterial()
{
	Material mat;
	mat.ambient = Vector4(0.329f, 0.224f, 0.027f, 1.0f);
	mat.diffuse = Vector4(0.78f, 0.568f, 0.118f, 1.0f);
	mat.speculiar = Vector4(0.992f, 0.941f, 0.808f, 1.0f);
	mat.exponent.x = 27.9f;
	return mat;
}

Material getPolishedSilverMaterial()
{
	Material mat;
	mat.ambient = Vector4(0.231f, 0.231f, 0.231f, 1.0f);
	mat.diffuse = Vector4(0.277f, 0.277f, 0.277f, 1.0f);
	mat.speculiar = Vector4(0.774f, 0.774f, 0.774f, 1.0f);;
	mat.exponent.x = 89.6f;
	return mat;
}
