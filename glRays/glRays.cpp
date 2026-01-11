#include <iostream>
#include <chrono>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "gl_texture.h"
#include "camera.h"
#include "shader.h"
#include "scene.h"
#include "options.h"

const int WIDTH = 1600, HEIGHT = 900;
int window_width, window_height;
int frame_count = 0;
float delta_time = 0.0f;
float last_frame = 0.0f;

static void error_callback(int error_code, const char* description)
{
	std::cerr << "ERROR:GLFW::" << error_code << " -- " << description << std::endl;
}

static void window_close_callback(GLFWwindow* window)
{
	std::cerr << "Closing..." << std::endl;
}

static void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	window_width = width;
	window_height = height;
	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Camera* handler = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (handler)
		handler->key_event(window, key, scancode, action, mods);
}

static void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	Camera* handler = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (handler)
		handler->mouse_move_event(window, xpos, ypos);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	Camera* handler = reinterpret_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (handler)
		handler->mouse_button_event(window, button, action, mods);
}

int main()
{
	// Init GLFW
	GLFWwindow* window;
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(WIDTH, HEIGHT, "glRays", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (!gladLoadGL(glfwGetProcAddress)) {
		std::cerr << "Failed to initialise GLAD" << std::endl;
		return -1;
	}
	std::clog << "OpenGL " << glGetString(GL_VERSION) << std::endl;

	glViewport(0, 0, WIDTH, HEIGHT);
	glfwSetWindowTitle(window, "glRays");

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetErrorCallback(error_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, cursor_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);


	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();


	// Setup texture to render to
	GLTexture tex = GLTexture(WIDTH, HEIGHT);
	tex.create_texture();


	// Setup quad to display
	unsigned int vbo, vao, ebo;
	{
		GLfloat vertices[] = { 
			// pos              // texture coord
			 1.0,  1.0, 0.0,    1.0, 1.0,    // top right
			 1.0, -1.0, 0.0,    1.0, 0.0,    // bottom right
			-1.0, -1.0, 0.0,    0.0, 0.0,    // bottom left
			-1.0,  1.0, 0.0,    0.0, 1.0     // top left
		};
		unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glGenVertexArrays(1, &vao);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}


	// Set up camera
	Camera cam = Camera(window, io);


	// Compile shaders
	ShaderProgram compute_shader = ShaderProgram();
	compute_shader.attach("compute.glsl", GL_COMPUTE_SHADER);
	compute_shader.link();

	ShaderProgram quad_shader = ShaderProgram();
	quad_shader.attach("vertex.glsl", GL_VERTEX_SHADER);
	quad_shader.attach("fragment.glsl", GL_FRAGMENT_SHADER);
	quad_shader.link();


	// Set up sphere buffer
	unsigned int sphere_ubo;
	SceneData scene_data = defaultScene();
	glGenBuffers(1, &sphere_ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, sphere_ubo);
	glBufferData(GL_UNIFORM_BUFFER, scene_data.size, scene_data.objects, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUniformBlockBinding(compute_shader.id, sphere_ubo, 2);
	glBindBufferBase(GL_UNIFORM_BUFFER, 2, sphere_ubo);


	Options options_obj = Options(cam);
	auto start = std::chrono::steady_clock::now();

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		glClear(GL_COLOR_BUFFER_BIT);

		// Process input
		glfwPollEvents();
		{
			auto end = std::chrono::steady_clock::now();
			const std::chrono::duration<float> diff = end - start;
			delta_time = diff.count();
			start = end;

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Render options imgui window and update relevant data
			options_obj.render_options_window(delta_time);
			cam.set_sensitivity(options_obj.camera_sensitivity);
			cam.set_camera_speed(options_obj.camera_speed);

			cam.set_delta_time(delta_time);
		}

		// Run compute shader
		{
			compute_shader.use();
			compute_shader.setInt("u_frame_count", cam.get_frames_still());
			compute_shader.setBool("u_camera_moved", cam.get_moved());
			compute_shader.setFloat("u_fov", options_obj.camera_fov);
			compute_shader.setInt("u_max_bounces", options_obj.rt_max_bounces);
			compute_shader.setInt("u_rays_per_pixel", options_obj.rt_rays_per_pixel);
			compute_shader.setMat4("camera_to_world", cam.get_camera_to_world());
			glDispatchCompute((GLuint)tex.width(), (GLuint)tex.height(), 1);
		}

		// prevent reading until finished writing to image
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Draw results to screen
		{
			glClear(GL_COLOR_BUFFER_BIT);
			quad_shader.use();
			glBindVertexArray(vao);

			tex.bind();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		} 

		// ImGui
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Have to call this after render, otherwise we get ghosting when rotating camera
		cam.update_movement();

		glfwSwapBuffers(window);
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 1;
}

