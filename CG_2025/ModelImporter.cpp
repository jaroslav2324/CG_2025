#include <locale>
#include <filesystem>
#include "tiny_obj_loader.h"

#include "ModelImporter.h"

static std::map<std::string, ModelData> models;

bool ModelImporter::loadOBJ(const std::string& filename, std::vector<Vertex>& vertices, std::vector<unsigned int>& indicies)
{
	std::string canonicalFilePath = std::filesystem::canonical(filename).string();
	if (models.contains(canonicalFilePath)) {
		vertices = models[canonicalFilePath].vertices;
		indicies = models[canonicalFilePath].indicies;
		return true;
	}

	tinyobj::ObjReader reader;
	tinyobj::ObjReaderConfig config;
	config.triangulate = true;
	if (!reader.ParseFromFile(canonicalFilePath, config)) {
		throw std::exception();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();

	//std::cout << sizeof(Vertex);
	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++) {
		size_t index_offset = 0;

		// Loop over faces (polygons)
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]); // Количество вершин в грани (обычно 3)

			std::vector<unsigned int> faceIndices;

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++) {
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

				// Получаем координаты вершины
				Vertex vertex;
				vertex.pos.x = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
				vertex.pos.y = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
				vertex.pos.z = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

				// Если есть нормали
				if (idx.normal_index >= 0) {
					vertex.norm.x = attrib.normals[3 * size_t(idx.normal_index) + 0];
					vertex.norm.y = attrib.normals[3 * size_t(idx.normal_index) + 1];
					vertex.norm.z = attrib.normals[3 * size_t(idx.normal_index) + 2];
				}

				// Если есть текстурные координаты
				if (idx.texcoord_index >= 0) {
					vertex.tex.x = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
					vertex.tex.y = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
				}

				// Проверяем, есть ли уже такая вершина в массиве
				auto it = std::find(vertices.begin(), vertices.end(), vertex);
				if (it != vertices.end()) {
					// Вершина уже есть, добавляем её индекс
					//std::cout << vertex.pos.x << " " << vertex.pos.y << " " << vertex.pos.z << std::endl;
					faceIndices.push_back(static_cast<unsigned int>(std::distance(vertices.begin(), it)));
				}
				else {
					// Добавляем новую вершину
					//std::cout << vertex.pos.x << " " << vertex.pos.y << " " << vertex.pos.z << std::endl;
					vertices.push_back(vertex);
					faceIndices.push_back(static_cast<unsigned int>(vertices.size() - 1));
				}
			}

			// Добавляем индексы для треугольников (если грань не треугольная — триангулируем)
			for (size_t i = 1; i + 1 < faceIndices.size(); i++) {
				//std::cout << faceIndices[0] << " " << faceIndices[1] << " " << faceIndices[2] << std::endl;
				indicies.push_back(faceIndices[0]);
				indicies.push_back(faceIndices[i]);
				indicies.push_back(faceIndices[i + 1]);
			}

			index_offset += fv;
		}
	}
	models.try_emplace(canonicalFilePath, ModelData());
	models[canonicalFilePath].vertices = vertices;
	models[canonicalFilePath].indicies = indicies;
	return true;
}
