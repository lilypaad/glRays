#include <iostream>
#include <format>

#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>

class Camera
{
	float delta_time = 0.1f;
	float camera_speed = 1.0f;
	float sensitivity = 1.0f;

	// Camera vectors
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::normalize(glm::cross(front, world_up));
	glm::vec3 up = glm::normalize(glm::cross(right, front));
	float pitch = 0.0, yaw = 0.0;
	glm::mat4 camera_to_world = glm::mat4(1.0);

	// Info for input handling
	float lastX, lastY;
	float first_mouse = true;

	struct {
		bool forward = false;
		bool back = false;
		bool left = false;
		bool right = false;
		bool up = false;
		bool down = false;
	} active_keys;

	bool cam_moved = false;
	int frames_still = 0; // how many frames the camera has been held still

public:
	Camera(GLFWwindow* window)
	{
		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

		int w, h;
		glfwGetWindowSize(window, &w, &h);
		lastX = float(w) / 2.0;
		lastY = float(h) / 2.0;
	}

	glm::vec3 get_position() const { return position; }
	glm::vec3 get_front() const { return front; }
	glm::vec3 get_up() const { return up; }
	glm::vec3 get_right() const { return right; }
	float get_pitch() const { return pitch; }
	float get_yaw() const { return yaw; }
	bool get_moved() const { return cam_moved; }
	int get_frames_still() const { return frames_still; }
	float get_sensitivity() const { return sensitivity; }
	float get_camera_speed() const { return camera_speed; }

	glm::mat4x4 get_camera_to_world() 
	{
		// Update camera-to-world transform matrix
		glm::mat4x4 m_translation = {
			1.0, 0.0, 0.0, 0.0,
			0.0, 1.0, 0.0, 0.0,
			0.0, 0.0, 1.0, 0.0,
			position.x, position.y, position.z, 1.0
		};
		glm::mat4x4 m_rotation_x = {
			1.0, 0.0, 0.0, 0.0,
			0.0, cos(glm::radians(pitch)), sin(glm::radians(pitch)), 0.0,
			0.0, -sin(glm::radians(pitch)), cos(glm::radians(pitch)), 0.0,
			0.0, 0.0, 0.0, 1.0
		};
		glm::mat4x4 m_rotation_y = {
			cos(glm::radians(yaw)), 0.0, sin(glm::radians(yaw)), 0.0,
			0.0, 1.0, 0.0, 0.0,
			-sin(glm::radians(yaw)), 0.0, cos(glm::radians(yaw)), 0.0,
			0.0, 0.0, 0.0, 1.0
		};

		return m_translation * m_rotation_y * m_rotation_x; 
	}

	void set_delta_time(float time) { this->delta_time = time; }
	void set_sensitivity(float sensitivity) { this->sensitivity = sensitivity; }
	void set_camera_speed(float speed) { this->camera_speed = speed; }

	void update_movement()
	{
		float dist = camera_speed * delta_time;

		if (active_keys.forward) {
			position += front * dist;
			cam_moved = true;
			frames_still = 0;
		}
		else if (active_keys.back) {
			position -= front * dist;
			cam_moved = true;
			frames_still = 0;
		}
		else if (active_keys.left) {
			position -= right * dist;
			cam_moved = true;
			frames_still = 0;
		}
		else if (active_keys.right) {
			position += right * dist;
			cam_moved = true;
			frames_still = 0;
		}
		else if (active_keys.up) {
			position += up * dist;
			cam_moved = true;
			frames_still = 0;
		}
		else if (active_keys.down) {
			position -= up * dist;
			cam_moved = true;
			frames_still = 0;
		}
		else {
			cam_moved = false;
			frames_still++;
		}

		update_vectors();
	}

	void key_event(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (action == GLFW_PRESS)
		{
			switch (key)
			{
				case GLFW_KEY_W: active_keys.forward = true; break;
				case GLFW_KEY_S: active_keys.back = true; break;
				case GLFW_KEY_A: active_keys.left = true; break;
				case GLFW_KEY_D: active_keys.right = true; break;
				case GLFW_KEY_SPACE: active_keys.up = true; break;
				case GLFW_KEY_LEFT_CONTROL: active_keys.down = true; break;
				case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
			}
		}

		if (action == GLFW_RELEASE)
		{
			switch (key)
			{
				case GLFW_KEY_W: active_keys.forward = false; break;
				case GLFW_KEY_S: active_keys.back = false; break;
				case GLFW_KEY_A: active_keys.left = false; break;
				case GLFW_KEY_D: active_keys.right = false; break;
				case GLFW_KEY_SPACE: active_keys.up = false; break;
				case GLFW_KEY_LEFT_CONTROL: active_keys.down = false; break;
			}
		}
	}

	void mouse_event(GLFWwindow* window, double xpos, double ypos)
	{
		if (first_mouse)
		{
			lastX = xpos;
			lastY = ypos;
			first_mouse = false;
		}

		float xoffset = (xpos - lastX);
		float yoffset = (lastY - ypos);
		lastX = xpos;
		lastY = ypos;

		pitch += yoffset * sensitivity;
		yaw += xoffset * sensitivity;
		if (pitch > 89.9f)
			pitch = 89.9f;
		if (pitch < -89.9f)
			pitch = -89.9f;

		update_vectors();

		frames_still = 0;
		cam_moved = true;
	}

	void update_vectors()
	{
		// Update camera vectors
		front.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = -cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front = normalize(front);
		right = glm::normalize(glm::cross(front, world_up));
		up = glm::normalize(glm::cross(right, front));
	}
};