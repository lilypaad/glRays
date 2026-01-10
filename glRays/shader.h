#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include <string>
#include <format>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class ShaderProgram
{
	std::vector<GLuint> shaders;

public:
	unsigned int id;

	ShaderProgram();

	void attach(const char* path, GLenum type);
	void link();

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setVec3(const std::string& name, glm::vec3 value) const;
	void setVec2(const std::string& name, glm::vec2 value) const;
	void setMat4(const std::string& name, glm::mat4x4 value) const;
};
