#include "Scene.h"

#include <filesystem>
#include <iostream>

#include "../Assets/Mesh.h"
#include "../Assets/Texture.h"
#include "../Assets/SceneConfigs.h"

namespace Engine
{
	Scene::Scene(std::vector<Assets::Instance> instances) : instances(instances)
	{
		std::cout << "[SCENE] Scene loading has begun" << std::endl;

		for (const auto& entry : instances)
		{
			AddMesh("../Assets/Scenes/" + entry.meshPath);
		}

		Complete();
		CreateBuffers();

		std::cout << "[SCENE] Scene has been loaded" << std::endl;
	}

	Scene::~Scene() {}

	void Scene::AddMesh(const std::string& path)
	{
		std::cout << "[SCENE] Mesh: " << path << " has been requested" << std::endl;
		meshBuffer.emplace_back(new Assets::Mesh(path));
	}

	void Scene::AddTexture(const std::string& path) {}

	void Scene::Complete()
	{
		for (auto& mesh : meshBuffer)
			mesh->Wait();
	}

	void Scene::CreateBuffers()
	{
		uint32_t offset = 0;
		uint32_t id = 0;

		for (int i = 0; i < meshBuffer.size(); ++i)
		{
			auto& mesh = meshBuffer[i];
			auto& instance = instances[i];

			glm::mat4 model(1);

			// Left handed system
			model[0][0] = instance.scale.x;
			model[1][1] = instance.scale.y;
			model[2][2] = instance.scale.z;

			model[3][0] = instance.translation.x;
			model[3][1] = instance.translation.y;
			model[3][2] = instance.translation.z;

			glm::mat4 modelInv = transpose(inverse(model));

			for (auto& vertex : mesh->GetVertices())
			{
				vertex.position = model * glm::vec4(vertex.position, 1.f);
				vertex.normal = modelInv * glm::vec4(vertex.normal, 1.f);
				vertex.id = id++;
				vertexBuffer.push_back(vertex);
			}

			for (auto index : mesh->GetIndecies())
				indexBuffer.push_back(index + offset);

			offset += mesh->GetVerticesSize();
		}
	}

	uint32_t Scene::GetTriangleCount() const
	{
		return indexBuffer.size() / 3;
	}
}
