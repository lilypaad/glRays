#include <iostream>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include "gl_texture.h"
#include "camera.h"
#include "shader.h"
#include "scene.h"

const int WIDTH = 800, HEIGHT = 600;
int window_width, window_height;
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


	// Setup texture to render to
	GLTexture tex = GLTexture(WIDTH, HEIGHT);
	tex.create_texture();


	// Setup quad to display
	GLfloat vertices[] = { 
		// pos              // texture coord
		 1.0,  1.0, 0.0,    1.0, 1.0,    // top right
		 1.0, -1.0, 0.0,    1.0, 0.0,    // bottom right
		-1.0, -1.0, 0.0,    0.0, 0.0,    // bottom left
		-1.0,  1.0, 0.0,    0.0, 1.0     // top left
	};
	unsigned int indices[] = { 0, 1, 3, 1, 2, 3 };

	unsigned int vbo, vao, ebo;
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



	// Set up camera
	Camera cam = Camera(window);


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
	glBufferData(GL_UNIFORM_BUFFER, scene_data.size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glUniformBlockBinding(compute_shader.id, sphere_ubo, 1);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, sphere_ubo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, scene_data.size, scene_data.objects);





	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetErrorCallback(error_callback);
	glfwSetWindowCloseCallback(window, window_close_callback);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);


	// Keep track of these for input
	float delta_time = 0.0f;
	float last_frame = 0.0f;

	// Main loop
	while (!glfwWindowShouldClose(window)) {
		// Process input
		{
			float now = glfwGetTime();
			delta_time = now - last_frame;
			last_frame = now;
			glfwPollEvents();
			cam.processInput(window, delta_time);
		}

		// Show frame time
		float now = glfwGetTime();
		delta_time = now - last_frame;
		last_frame = now;
		glfwSetWindowTitle(window, std::format(
			"glRays | {:.4f}ms | pos x={:.2f} y={:.2f} z={:.2f} | dir x={:.2f} y={:.2f} z={:.2f} | pitch {:.2f} yaw {:.2f}", 
			delta_time, 
			cam.get_position().x, cam.get_position().y, cam.get_position().z,
			cam.get_front().x, cam.get_front().y, cam.get_front().z,
			cam.get_rotation().x, cam.get_rotation().y
		).c_str());

		{
			compute_shader.setVec3("camera_loc", cam.get_position());
			compute_shader.setVec2("camera_rot", cam.get_rotation());
			compute_shader.setVec3("camera_dir", cam.get_front());
			compute_shader.setVec3("camera_up", cam.get_up());
			compute_shader.setVec3("camera_right", cam.get_right());
			compute_shader.setMat4("camera_to_world", cam.get_camera_to_world());
			compute_shader.use();
			glDispatchCompute((GLuint)tex.width(), (GLuint)tex.height(), 1);
		}

		// prevent reading until finished writing to image
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Actual draw call
		{
			glClear(GL_COLOR_BUFFER_BIT);
			quad_shader.use();
			glBindVertexArray(vao);

			tex.bind();

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		} 

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 1;
}

