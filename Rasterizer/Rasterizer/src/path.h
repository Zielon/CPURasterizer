#pragma once

#include <filesystem>

class Path
{
public:
	static std::filesystem::path Root(std::vector<std::string> segments)
	{
		auto root = std::filesystem::current_path();
		while (root.string().find("CPURasterizer") != std::string::npos)
			root = root.parent_path();

		root /= "CPURasterizer";

		for (const auto segment : segments)
			root /= segment;

		return root;
	}
};