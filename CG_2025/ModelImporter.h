#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <string>

#include "directxtk/SimpleMath.h"

#include "Face.h"
#include "Vertex.h"

class ModelImporter {
public:
	using Vector2 = DirectX::SimpleMath::Vector2;
	using Vector3 = DirectX::SimpleMath::Vector3;
	bool loadOBJ(const std::string& filename,
		std::vector<Vertex>& vertices, std::vector<unsigned int>& indicies);
private:

};