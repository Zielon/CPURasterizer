#include "Scene.h"

#include <filesystem>

#include "../Assets/Mesh.h"
#include "../Assets/Texture.h"

namespace Engine
{
	Scene::Scene()
	{
		for (const auto& entry : std::filesystem::directory_iterator("../Assets/Scenes/cornell_box/"))
			AddMesh(entry.path().string());
		
		Complete();
	}

	Scene::~Scene() {}

	void Scene::AddMesh(const std::string& path)
	{
		meshes.emplace_back(new Assets::Mesh(path));
	}

	void Scene::AddTexture(const std::string& path) {}

	void Scene::Complete()
	{
		for (auto& mesh : meshes)
			mesh->Wait();
	}
}
