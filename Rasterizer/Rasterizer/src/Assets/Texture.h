#pragma once

#include <future>

namespace Assets
{
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
		[[nodiscard]] const void* GetPixels() const { return pixels; };

	private:
		std::future<void> loader{};
		std::string path;
		void* pixels;
		int texWidth{};
		int texHeight{};
		int texChannels{};
		int imageSize;
	};
}
