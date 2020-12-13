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
}

template <class T>
using Buffer = std::vector<T>;

using MeshBuffer = Buffer<std::unique_ptr<Assets::Mesh>>;
using IndexBuffer = Buffer<uint32_t>;
using VertexBuffer = Buffer<Assets::Vertex>;

namespace Engine
{
	class Scene final
	{
	public:
		Scene(Buffer<Assets::Scene::Instance> instances);
		~Scene();

		void AddMesh(const std::string& path);
		void AddTexture(const std::string& path);
		void Complete();
		void CreateBuffers();

		[[nodiscard]] uint32_t GetTriangleCount() const;
		[[nodiscard]] const VertexBuffer& GetVertexBuffer() const { return vertexBuffer; }
		[[nodiscard]] const IndexBuffer& GetIndexBuffer() const { return indexBuffer; }

	private:
		Buffer<Assets::Scene::Instance> instances;
		MeshBuffer meshBuffer;
		IndexBuffer indexBuffer;
		VertexBuffer vertexBuffer;
	};
}
