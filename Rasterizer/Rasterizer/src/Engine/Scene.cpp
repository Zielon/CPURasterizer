#include "Scene.h"

#include <filesystem>

#include "../Assets/Mesh.h"
#include "../Assets/Texture.h"

namespace Engine
{
	Scene::Scene()
	{
		for (const auto& entry : std::filesystem::directory_iterator("../Assets/Scenes/bunny/"))
			AddMesh(entry.path().string());

		Complete();
		CreateBuffers();
	}

	Scene::~Scene() {}

	void Scene::AddMesh(const std::string& path)
	{
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
