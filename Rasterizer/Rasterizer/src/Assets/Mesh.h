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

		[[nodiscard]] std::vector<Vertex>& GetVertices()
		{
			return vertices;
		}

		[[nodiscard]] uint32_t GetVerticesSize() const
		{
			return vertices.size();
		}

		[[nodiscard]] const std::vector<uint32_t>& GetIndecies() const
		{
			return indices;
		}

		[[nodiscard]] uint32_t GetIndeciesSize() const
		{
			return indices.size();
		}

	private:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::future<void> loader{};

		void Load(const std::string& path);
	};
}

