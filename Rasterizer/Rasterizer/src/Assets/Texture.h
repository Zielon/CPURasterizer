#pragma once

#include <future>
#include <vector>

namespace Assets
{
	enum TextureType { Metallic, Albedo, Normal, Height };

	class Texture
	{
	public:
		Texture(const std::string& path);

		Texture(const Texture&) = delete;
		Texture(Texture&&) noexcept;

		Texture& operator =(Texture&&) noexcept;
		Texture& operator =(const Texture&) = delete;

		~Texture();

		void Wait();

		[[nodiscard]] int GetWidth() const { return texWidth; };
		[[nodiscard]] int GetHeight() const { return texHeight; };
		[[nodiscard]] int GetImageSize() const { return imageSize; };
		[[nodiscard]] int GetChannels() const { return texChannels; };
		[[nodiscard]] const std::vector<float>& GetPixels() const { return pixels; };

	private:
		std::future<void> loader{};
		std::string path;
		std::vector<float> pixels;
		int texWidth{};
		int texHeight{};
		int texChannels{};
		int imageSize;
	};
}
