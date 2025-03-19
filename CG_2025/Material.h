#pragma once

#include "directxtk/SimpleMath.h"

struct Material {
	using Vector4 = DirectX::SimpleMath::Vector4;
	Vector4 ambient;
	Vector4 diffuse;
	Vector4 speculiar;
	Vector4 exponent;
};

Material getBlackPlasticMaterial();
Material getBrassMaterial();
Material getPolishedSilverMaterial();
