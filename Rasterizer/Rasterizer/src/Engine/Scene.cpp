#include "Scene.h"

#include <filesystem>
#include <iostream>

#include "../Assets/Mesh.h"
#include "../Assets/Texture.h"
#include "../Assets/Material.h"
#include "../Assets/SceneConfigs.h"

namespace Engine
{
	Scene::Scene(Buffer<Assets::Scene::Instance> instances) : instances(instances)
	{
		std::cout << "[SCENE] Scene loading has begun" << std::endl;

		for (const auto& entry : instances)
		{
			Assets::Material material = Assets::Scene::MATERIALS.at(entry.material);
			material.textureId = AddTexture(material.texturePath);
			AddMesh(entry.meshPath, materials.size());
			materials.push_back(material);
		}

		Complete();
		CreateBuffers();

		std::cout << "[SCENE] Scene has been loaded" << std::endl;
	}

	Scene::~Scene() = default;

	void Scene::AddMesh(const std::string& path, uint32_t materialId)
	{
		std::cout << "[SCENE] Mesh: " << path << " has been requested" << std::endl;
		meshBuffer.emplace_back(new Assets::Mesh("../Assets/Scenes/" + path));
		meshBuffer[meshBuffer.size() - 1]->SetMaterial(materialId);
	}

	int Scene::AddTexture(const std::string& path)
	{
		if (path.empty())
			return -1;

		int id = textureBuffer.size();
		textureBuffer.emplace_back(new Assets::Texture("../Assets/Textures/" + path));
		return id;
	}

	void Scene::Complete()
	{
		for (auto& mesh : meshBuffer)
			mesh->Wait();

		for (auto& texture : textureBuffer)
			texture->Wait();
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
				vertex.normal = normalize(modelInv * glm::vec4(vertex.normal, 1.f));
				vertex.id = id++;
				vertex.materialId = mesh->GetMaterial();
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
