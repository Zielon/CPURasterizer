#pragma once

#include <map>
#include <string>
#include <vector>
#include <glm/glm.hpp>

#include "Material.h"

namespace Assets
{
	namespace Scene
	{
		struct Instance
		{
			std::string meshPath{};
			std::string material;
			glm::vec3 translation{};
			glm::vec3 scale{};
		};

		struct Camera
		{
			glm::vec3 eye{};
			glm::vec3 lookAt{};
			float fov;
			uint32_t height;
			uint32_t width;
		};

		struct Configuration
		{
			Camera camera;
			std::vector<Instance> instances;
		};

		const static std::map<std::string, Material> MATERIALS =
		{
			{ "WHITE", { { 0.725, 0.71, 0.68 } } },
			{ "GREEN", { { 0.14, 0.45, 0.091 } } },
			{ "RED", { { 0.63, 0.065, 0.05 } } },
			{ "BLUE", { { 0.1, 0.1, 0.8 } } },
			{ "GREY", { { 0.5, 0.5, 0.5 } } }
		};

		const static std::vector<Instance> CORNELL_BOX_MESHES =
		{
			{ "cornell_box/cbox_ceiling.obj", "WHITE", { .278, .5488, .27955 }, { 0.01, 0.01, 0.01 } },
			{ "cornell_box/cbox_floor.obj", "WHITE", { .2756, 0, .2796 }, { 0.01, 0.01, 0.01 } },
			{ "cornell_box/cbox_back.obj", "GREY", { .2764, .2744, .5592 }, { 0.01, 0.01, 0.01 } },
			{ "cornell_box/cbox_smallbox.obj", "BLUE", { .1855, .0825, .169 }, { 0.01, 0.01, 0.01 } },
			{ "cornell_box/cbox_largebox.obj", "WHITE", { .3685, .165, .35125 }, { 0.01, 0.01, 0.01 } },
			{ "cornell_box/cbox_greenwall.obj", "GREEN", { 0, .2744, .2796 }, { 0.01, 0.01, 0.01 } },
			{ "cornell_box/cbox_redwall.obj", "RED", { .5536, .2744, .2796 }, { 0.01, 0.01, 0.01 } }
		};

		const static std::vector<Instance> PANTHER_MESHES =
		{
			{ "panther/panther.obj", "GREY", { 0, 0, 0 }, { 1, 1, 1 } },
			{ "panther/background.obj", "GREY", { 0, 0, 0 }, { 1, 1, 1 } },
		};

		const static std::vector<Instance> COFFEE_CART_MESHES =
		{
			{ "coffee/CoffeeCart_01.obj", "GREY", { 0, 0, 0 }, { 1, 1, 1 } },
			{ "coffee/CoffeeCart_02.obj", "GREY", { 0, 0, 0 }, { 1, 1, 1 } },
		};

		const static std::vector<Configuration> CONFIGS =
		{
			{ { { .276, .275, -.75 }, { .276, .275, .10 }, 40, 800, 800 }, CORNELL_BOX_MESHES },
			{ { { 0, 1.5, 2.5 }, { 0, 1, 0 }, 60, 720, 1280 }, COFFEE_CART_MESHES },
			{ { { -0.3, 0.5, 1.6 }, { -0.1, 1.0, -1 }, 50, 720, 1280 }, PANTHER_MESHES }
		};
	}
}
