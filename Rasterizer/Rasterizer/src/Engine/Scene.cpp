#include "Scene.h"

#include <filesystem>
#include <iostream>

#include "../Assets/Mesh.h"
#include "../Assets/Texture.h"

namespace Engine
{
	Scene::Scene()
	{
		std::cout << "[SCENE] Scene loading has begun" << std::endl;

		for (const auto& entry : std::filesystem::directory_iterator("../Assets/Scenes/bunny/"))
		{
			AddMesh(entry.path().string());
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

		for (auto& mesh : meshBuffer)
		{
			for (auto& vertex : mesh->GetVertices())
			{
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
