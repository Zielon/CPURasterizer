#pragma once

#include <glm/glm.hpp>

namespace Engine
{
	enum class Action
	{
		LEFT,
		RIGHT,
		UP,
		DOWN
	};

	class Camera
	{
	public:
		Camera(glm::vec3 eye, glm::vec3 lookAt, float fov, float aspect, glm::ivec2 resolution);
		~Camera() = default;

		void OnKeyChanged(int key, int scanCode, int action, int mod);
		bool OnCursorPositionChanged(double xpos, double ypos);
		void OnMouseButtonChanged(int button, int action, int mods);
		bool OnBeforeRender();
		void static TimeDeltaUpdate();

		[[nodiscard]] glm::mat4 GetViewMatrix() const;
		[[nodiscard]] glm::mat4 GetProjectionMatrix() const;
		[[nodiscard]] glm::mat4 GetRasterMatrix() const;
		[[nodiscard]] glm::vec3 GetDirection() const;
		[[nodiscard]] glm::vec3 GetPosition() const;
		[[nodiscard]] uint32_t GetHeight() const { return height; }
		[[nodiscard]] uint32_t GetWidth() const { return width; }

	private:
		void Update();

		void Move(Action action);

		glm::vec3 position;
		glm::vec3 front{};
		glm::vec3 worldUp{};
		glm::vec3 right{};
		glm::vec3 up{};

		glm::vec3 pivot{};

		glm::mat4 raster;

		float lastX{};
		float lastY{};
		uint32_t width;
		uint32_t height;

		bool isFirstMouseEvent = false;
		bool isMousePressed = false;
		bool isCameraLeft = false;
		bool isCameraRight = false;
		bool isCameraUp = false;
		bool isCameraDown = false;

		float pitch, yaw, fov, focalDist, aperture, radius, aspect;

		static float DELTA_TIME;
		static float LAST_FRAME_TIME;
	};
}
