#pragma once

#include <filesystem>

class Path
{
public:
	static std::filesystem::path Root(std::vector<std::string> segments)
	{
		auto root = std::filesystem::current_path();
		auto current = root.parent_path().filename();
		while (current.string().find("CPURasterizer") == std::string::npos)
		{
			root = root.parent_path();
			current = root.parent_path().filename();
		}

		root = root.parent_path();

		for (const auto segment : segments)
			root /= segment;

		return root;
	}
};