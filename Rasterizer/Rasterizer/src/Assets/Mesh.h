#pragma once

#include <future>
#include <string>

#include "Vertex.h"

namespace Assets
{
	class Mesh
	{
	public:
		Mesh(const std::string& path);

		void Wait();

		void SetMaterial(const uint32_t& material) { materialId = material; }
		[[nodiscard]] uint32_t GetMaterial() const { return materialId; }
		[[nodiscard]] std::vector<Vertex>& GetVertices() { return vertices; }
		[[nodiscard]] uint32_t GetVerticesSize() const { return vertices.size(); }
		[[nodiscard]] const std::vector<uint32_t>& GetIndecies() const { return indices; }
		[[nodiscard]] uint32_t GetIndeciesSize() const { return indices.size(); }

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::future<void> loader{};
		uint32_t materialId;

		void Load(const std::string& path);
	};

	class MeshInstance
	{
	public:
		MeshInstance(int meshId, glm::mat4 transformation, int matId)
			: modelTransform(transformation), materialId(matId), meshId(meshId) { }

		~MeshInstance() = default;

		glm::mat4 modelTransform = glm::mat4(1.f);
		int materialId;
		int meshId;
	};
}
