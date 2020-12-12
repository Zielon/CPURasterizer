#pragma once

#include <string>
#include <utility>
#include <vector>
#include <memory>

namespace Assets
{
	class Mesh;
	class Texture;
	struct Vertex;
	struct Instance;
}

using MeshBuffer = std::vector<std::unique_ptr<Assets::Mesh>>;
using IndexBuffer = std::vector<uint32_t>;
using VertexBuffer = std::vector<Assets::Vertex>;

namespace Engine
{
	class Scene final
	{
	public:
		Scene(std::vector<Assets::Instance> instances);
		~Scene();

		void AddMesh(const std::string& path);
		void AddTexture(const std::string& path);
		void Complete();
		void CreateBuffers();

		[[nodiscard]] uint32_t GetTriangleCount() const;
		[[nodiscard]] const VertexBuffer& GetVertexBuffer() const { return vertexBuffer; }
		[[nodiscard]] const IndexBuffer& GetIndexBuffer() const { return indexBuffer; }

	private:
		std::vector<Assets::Instance> instances;
		MeshBuffer meshBuffer;
		IndexBuffer indexBuffer;
		VertexBuffer vertexBuffer;
	};
}
