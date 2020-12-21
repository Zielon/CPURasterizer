#include "Scene.h"

#include <iostream>
#include <future>
#include <utility>

#include "Camera.h"
#include "../HDR/HDRloader.h"

#include "../Assets/Vertex.h"
#include "../Assets/Material.h"
#include "../Assets/Light.h"
#include "../Assets/Texture.h"
#include "../Assets/Mesh.h"

#include "../Loader/Loader.h"

namespace Engine
{
	Scene::Scene(std::string config): config(std::move(config))
	{
		const auto start = std::chrono::high_resolution_clock::now();

		Load();
		Wait();
		CreateBuffers();
		Print();

		const auto stop = std::chrono::high_resolution_clock::now();
		const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

		std::cout << "[SCENE] Loading time: " << duration.count() << " milliseconds" << std::endl;
	}

	void Scene::Wait()
	{
		for (auto& texture : textures)
			texture->Wait();

		for (const auto& mesh : meshes)
			mesh->Wait();

		if (hdrLoader.valid())
			hdrLoader.get();

		std::cout << "[SCENE] All assets have been loaded!" << std::endl;
	}

	void Scene::Print() const
	{
		std::cout << "[SCENE] Scene has been loaded!" << std::endl;
		std::cout << "	# meshes:    " << meshes.size() << std::endl;
		std::cout << "	# instances: " << meshInstances.size() << std::endl;
		std::cout << "	# textures:  " << textures.size() << std::endl;
		std::cout << "	# lights:    " << lights.size() << std::endl;
		std::cout << "	# materials: " << materials.size() << std::endl;
	}

	void Scene::Load()
	{
		if (!LoadSceneFromFile(config, *this, options))
			throw std::runtime_error("[ERROR] File does not exist. See README for more details!");
	}

	void Scene::CreateBuffers()
	{
		uint32_t offset = 0;
		uint32_t id = 0;

		for (const auto& meshInstance : meshInstances)
		{
			auto& mesh = meshes[meshInstance.meshId];
			glm::mat4 modelMatrix = meshInstance.modelTransform;
			glm::mat4 modelTransInvMatrix = transpose(inverse(modelMatrix));

			for (auto& vertex : mesh->GetVertices())
			{
				vertex.position = modelMatrix * glm::vec4(vertex.position, 1.f);
				vertex.normal = normalize(modelTransInvMatrix * glm::vec4(vertex.normal, 1.f));
				vertex.materialId = meshInstance.materialId;
				vertex.id = id++;
				vertices.push_back(vertex);
			}

			for (auto index : mesh->GetIndecies())
				indices.push_back(index + offset);

			offset += mesh->GetVerticesSize();
		}

		indeciesSize = indices.size();
		verticesSize = vertices.size();
	}

	void Scene::AddCamera(glm::vec3 pos, glm::vec3 lookAt, float fov, float aspect)
	{
		camera.reset(new Camera(pos, lookAt, fov, aspect, options.resolution));
	}

	void Scene::AddHDR(const std::string& path)
	{
		hdrLoader = std::async(std::launch::async, [this, path]()
		{
			const auto file = root + path;
			auto* hdr = Assets::HDRLoader::Load(file.c_str());

			if (hdr == nullptr)
				std::cerr << "[ERROR] Unable to load HDR!" << std::endl;
			else
			{
				std::cout << "[HDR TEXTURE] " + file + " has been loaded!" << std::endl;
				hdrData.reset(hdr);
				hdrResolution = hdr->width * hdr->height;
			}
		});
	}

	int Scene::AddMeshInstance(Assets::MeshInstance meshInstance)
	{
		int id = meshInstances.size();
		meshInstances.push_back(meshInstance);
		return id;
	}

	int Scene::AddMesh(const std::string& path)
	{
		int id;

		if (meshMap.find(path) != meshMap.end())
		{
			id = meshMap[path];
		}
		else
		{
			const auto file = root + path;
			id = meshes.size();
			meshes.emplace_back(new Assets::Mesh(file));
			meshMap[path] = id;
			std::cout << "[MESH] " + file + " has been requested!" << std::endl;
		}

		return id;
	}

	int Scene::AddTexture(const std::string& path)
	{
		int id;

		if (textureMap.find(path) != textureMap.end())
		{
			id = meshMap[path];
		}
		else
		{
			const auto file = root + path;
			id = textures.size();
			textures.emplace_back(new Assets::Texture(file));
			textureMap[path] = id;
			std::cout << "[TEXTURE] " + file + " has been requested!" << std::endl;
		}

		return id;
	}

	int Scene::AddMaterial(Assets::Material material)
	{
		int id = materials.size();
		materials.push_back(material);
		return id;
	}

	int Scene::AddLight(Assets::Light light)
	{
		int id = lights.size();
		lights.push_back(light);
		return id;
	}

	Scene::~Scene()
	{
		std::cout << "[SCENE] Scene " << config << " has been unloaded." << std::endl;
	}
}
