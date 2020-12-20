#pragma once

#include <map>
#include <memory>
#include <vector>
#include <string>

#define GLM_FORCE_SWIZZLE 
#include <future>
#include <glm/glm.hpp>

#include "../Assets/Vertex.h"
#include "../Loader/Loader.h"
#include "../Assets/Light.h"
#include "../Loader/RenderOptions.h"

namespace Assets
{
	class MeshInstance;
	class Mesh;
	class Texture;
	class Material;
	class HDRData;
	struct Light;
}

namespace Engine
{
	class Scene final : public Loader::SceneBase
	{
	public:
		Scene(std::string config);
		~Scene();

		void AddCamera(glm::vec3 pos, glm::vec3 lookAt, float fov, float aspect) override;
		void AddHDR(const std::string& path) override;
		int AddMesh(const std::string& path) override;
		int AddTexture(const std::string& path) override;
		int AddMaterial(Assets::Material material) override;
		int AddLight(Assets::Light light) override;
		int AddMeshInstance(class Assets::MeshInstance meshInstance) override;
		void CreateBuffers();

		[[nodiscard]] const std::vector<std::unique_ptr<Assets::Mesh>>& GetMeshes() const { return meshes; }
		[[nodiscard]] const std::vector<Assets::MeshInstance>& GetMeshInstances() const { return meshInstances; }
		[[nodiscard]] class Camera& GetCamera() const { return *camera; }
		[[nodiscard]] bool UseHDR() const { return options.useEnvMap; }
		[[nodiscard]] uint32_t GetLightsSize() const { return lights.size(); }
		[[nodiscard]] uint32_t GetIndexSize() const { return indeciesSize; }
		[[nodiscard]] uint32_t GetVertexSize() const { return verticesSize; }
		[[nodiscard]] uint32_t GetHDRResolution() const { return hdrResolution; }
		[[nodiscard]] Loader::RenderOptions GetRendererOptions() const { return options; }
		[[nodiscard]] uint32_t GetTriangleCount() const { return indeciesSize / 3; };
		[[nodiscard]] const std::vector<uint32_t>& GetIndexBuffer() const { return indices; }
		[[nodiscard]] const std::vector<Assets::Vertex>& GetVertexBuffer() const { return vertices; }
		[[nodiscard]] const std::vector<Assets::Material>& GetMaterials() const { return materials; }
		[[nodiscard]] const std::vector<Assets::Light>& GetLights() const { return lights; }

	private:
		std::string config;
		const std::string root = "../Assets/Scenes/";
		uint32_t verticesSize{};
		uint32_t indeciesSize{};
		std::unique_ptr<class Camera> camera;
		std::unique_ptr<Assets::HDRData> hdrData;

		// Assets
		std::map<std::string, int> meshMap;
		std::map<std::string, int> textureMap;

		std::vector<std::unique_ptr<Assets::Mesh>> meshes;
		std::vector<std::unique_ptr<Assets::Texture>> textures;

		std::vector<uint32_t> indices;
		std::vector<Assets::Vertex> vertices;
		std::vector<Assets::MeshInstance> meshInstances;
		std::vector<Assets::Material> materials;
		std::vector<Assets::Light> lights;

		std::future<void> hdrLoader{};
		float hdrResolution{};

		Loader::RenderOptions options;

		void Wait();
		void Load();
		void Print() const;
	};
}
