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
	float camera_speed = 2.0f;
	float sensitivity = 1.5f;

	// Camera vectors
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 world_up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 right = glm::normalize(glm::cross(front, world_up));
	glm::vec3 up = glm::normalize(glm::cross(right, front));
	glm::vec2 rotation = glm::vec2(0.0f, 0.0f); // pitch, yaw
	glm::mat4 camera_to_world = glm::mat4(1.0);

	// Info for mouse movement handling
	float lastX, lastY;
	float first_mouse = true;

	struct {
		bool left = false;
		bool right = false;
		bool fwd = false;
		bool bkwd = false;
		bool up = false;
		bool down = false;
	} key_pressed;

public:
	Camera(GLFWwindow* window, float camera_speed=2.0, float sensitivity=1.0) 
	{
		glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

		int w, h;
		glfwGetWindowSize(window, &w, &h);
		lastX = float(w) / 2.0;
		lastY = float(h) / 2.0;
		
		this->camera_speed = camera_speed;
		this->sensitivity = sensitivity;
	}

	glm::vec3 get_position() const { return position; }
	glm::vec2 get_rotation() const { return rotation; }
	glm::vec3 get_front() const { return front; }
	glm::vec3 get_up() const { return up; }
	glm::vec3 get_right() const { return right; }
	glm::mat4x4 get_camera_to_world() const { return camera_to_world; }

	//static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	//{
	//	Camera* cam = reinterpret_cast <Camera*>(glfwGetWindowUserPointer(window));
	//	if (cam)
	//		cam->key_handler(window, key, scancode, action, mods);
	//}

	//static void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
	//{
	//	Camera* cam = reinterpret_cast <Camera*>(glfwGetWindowUserPointer(window));
	//	if (cam)
	//		cam->mouse_pos_handler(window, xpos, ypos);
	//}

	//void update_position()
	//{
	//	float now = glfwGetTime();
	//	delta_time = now - last_frame;
	//	last_frame = now;
	//	float move_speed = camera_speed * delta_time;

	//	if (key_pressed.fwd)
	//		position += front * move_speed;
	//	if (key_pressed.bkwd)
	//		position -= front * move_speed;
	//	if (key_pressed.left)
	//		position -= right * move_speed;
	//	if (key_pressed.right)
	//		position += right * move_speed;
	//	if (key_pressed.up)
	//		position += world_up * move_speed;
	//	if (key_pressed.down)
	//		position -= world_up * move_speed;

	//	update_vectors();
	//}

	//void key_handler(GLFWwindow* window, int key, int scancode, int action, int mods)
	//{
	//	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	//		glfwSetWindowShouldClose(window, GLFW_TRUE);

	//	if (action == GLFW_PRESS)
	//	{
	//		if (key == GLFW_KEY_W) key_pressed.fwd = true;
	//		if (key == GLFW_KEY_S) key_pressed.bkwd = true;
	//		if (key == GLFW_KEY_A) key_pressed.left = true;
	//		if (key == GLFW_KEY_D) key_pressed.right = true;
	//		if (key == GLFW_KEY_SPACE) key_pressed.up = true;
	//		if (key == GLFW_KEY_LEFT_CONTROL) key_pressed.down = true;
	//	}
	//	if (action == GLFW_RELEASE)
	//	{
	//		if (key == GLFW_KEY_W) key_pressed.fwd = false;
	//		if (key == GLFW_KEY_S) key_pressed.bkwd = false;
	//		if (key == GLFW_KEY_A) key_pressed.left = false;
	//		if (key == GLFW_KEY_D) key_pressed.right = false;
	//		if (key == GLFW_KEY_SPACE) key_pressed.up = false;
	//		if (key == GLFW_KEY_LEFT_CONTROL) key_pressed.down = false;
	//	}
	//}

	//void mouse_pos_handler(GLFWwindow* window, double xpos, double ypos)
	//{
	//	if (first_mouse)
	//	{
	//		lastX = xpos;
	//		lastY = ypos;
	//		first_mouse = false;
	//	}

	//	float xoffset = (xpos - lastX);
	//	float yoffset = (lastY - ypos);
	//	lastX = xpos;
	//	lastY = ypos;

	//	rotation.x += yoffset;
	//	rotation.y += xoffset;
	//	if (rotation.x > 90.0f)
	//		rotation.x = 90.0f;
	//	if (rotation.x < -90.0f)
	//		rotation.x = -90.0f;

	//	update_vectors();
	//}

	void processInput(GLFWwindow* window, float delta_time)
	{
		// Keyboard input
		float move = camera_speed * delta_time;

		if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			position += front * move;
		if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			position -= front * move;
		if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			position -= right * move;
		if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			position += right * move;
		if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			position += world_up * move;
		if(glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			position -= world_up * move;
		if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		// Mouse input
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

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

		rotation.x += yoffset;
		rotation.y += xoffset;
		if (rotation.x > 90.0f)
			rotation.x = 90.0f;
		if (rotation.x < -90.0f)
			rotation.x = -90.0f;

		// Update camera vectors
		float pitch = rotation.x;
		float yaw = rotation.y;
		front.x = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
		front.y = sin(glm::radians(pitch));
		front.z = -cos(glm::radians(pitch)) * cos(glm::radians(yaw));
		front = normalize(front);
		right = glm::normalize(glm::cross(front, world_up));
		up = glm::normalize(glm::cross(right, front));

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
		camera_to_world = m_translation * m_rotation_y * m_rotation_x; 
	}

};