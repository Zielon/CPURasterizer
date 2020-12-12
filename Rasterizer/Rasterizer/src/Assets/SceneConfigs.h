#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace Assets
{
	struct Instance
	{
		std::string meshPath{};
		std::string texturePath{};
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

	struct SceneConfig
	{
		Camera camera;
		std::vector<Instance> instances;
	};

	const static std::vector<Instance> CORNELL_BOX_MESHES =
	{
		{ "cornell_box/cbox_ceiling.obj", "", { .278, .5488, .27955 }, { 0.01, 0.01, 0.01 } },
		{ "cornell_box/cbox_floor.obj", "", { .2756, 0, .2796 }, { 0.01, 0.01, 0.01 } },
		{ "cornell_box/cbox_back.obj", "", { .2764, .2744, .5592 }, { 0.01, 0.01, 0.01 } },
		{ "cornell_box/cbox_smallbox.obj", "", { .1855, .0825, .169 }, { 0.01, 0.01, 0.01 } },
		{ "cornell_box/cbox_largebox.obj", "", { .3685, .165, .35125 }, { 0.01, 0.01, 0.01 } },
		{ "cornell_box/cbox_greenwall.obj", "", { 0, .2744, .2796 }, { 0.01, 0.01, 0.01 } },
		{ "cornell_box/cbox_redwall.obj", "", { .5536, .2744, .2796 }, { 0.01, 0.01, 0.01 } }
	};

	const static std::vector<SceneConfig> CONFIG =
	{
		{ { { .276, .275, -.75 }, { .276, .275, .10 }, 40, 800, 800 }, CORNELL_BOX_MESHES }
	};
}
