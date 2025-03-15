#pragma once

#include <vector>

struct Face {
	std::vector<int> vertexIndices;
	std::vector<int> textureIndices;
	std::vector<int> normalIndices;
};