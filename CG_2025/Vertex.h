#pragma once

#include "directxtk/SimpleMath.h"
#include "util.h"

struct Vertex {
	DirectX::SimpleMath::Vector4 pos;
	DirectX::SimpleMath::Vector4 norm;
	DirectX::SimpleMath::Vector4 tex;
	bool operator==(const Vertex& rhs) const {
		return apprEqual(pos, rhs.pos); //&& apprEqual(norm, rhs.norm) && apprEqual(tex, rhs.tex);
	}
};

