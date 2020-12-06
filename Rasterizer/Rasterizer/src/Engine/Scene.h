#pragma once

#include <string>
#include <vector>
#include <memory>

namespace Assets
{
	class Mesh;
	class Texture;
	class Vertex;
}

namespace Engine
{
	class Scene final
	{
	public:
		Scene();
		~Scene();

		void AddMesh(const std::string& path);
		void AddTexture(const std::string& path);
		void Complete();
		void CreateBuffers();

	private:
		std::vector<std::unique_ptr<Assets::Mesh>> meshes;

		std::vector<Assets::Vertex> vertexBuffer;
		std::vector<uint32_t> indexBuffer;
	};
}
