#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>

#include "../Assets/SceneConfigs.h"

namespace Assets
{
	class Mesh;
	class Texture;
	struct Vertex;
	struct Instance;
	struct Material;
}

template <class T>
using Buffer = std::vector<T>;

using MeshBuffer = Buffer<std::unique_ptr<Assets::Mesh>>;
using IndexBuffer = Buffer<uint32_t>;
using VertexBuffer = Buffer<Assets::Vertex>;
using TextureBuffer = Buffer<std::unique_ptr<Assets::Texture>>;

namespace Engine
{
	class Scene final
	{
	public:
		Scene(Buffer<Assets::Scene::Instance> instances);
		~Scene();

		void AddMesh(const std::string& path, uint32_t materialId);
		int AddTexture(const std::string& path);
		void Complete();
		void CreateBuffers();

		[[nodiscard]] uint32_t GetTriangleCount() const;
		[[nodiscard]] const VertexBuffer& GetVertexBuffer() const { return vertexBuffer; }
		[[nodiscard]] const IndexBuffer& GetIndexBuffer() const { return indexBuffer; }
		[[nodiscard]] const Buffer<Assets::Material>& GetMaterials() const { return materials; }
		[[nodiscard]] const TextureBuffer& GetTextures() const { return textureBuffer; }

	private:
		Buffer<Assets::Scene::Instance> instances;
		TextureBuffer textureBuffer;
		Buffer<Assets::Material> materials;
		MeshBuffer meshBuffer;
		IndexBuffer indexBuffer;
		VertexBuffer vertexBuffer;
	};
}
