#include "shader.h"

ShaderProgram::ShaderProgram()
{
	this->id = glCreateProgram();
}

void ShaderProgram::attach(const char* path, GLenum type)
{
	// Read GLSL file
	std::string shader_src;
	std::ifstream file;
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		file.open(path);
		std::stringstream stream;
		stream << file.rdbuf();
		file.close();
		shader_src = stream.str();
	}
	catch (std::ifstream::failure e) {
		std::cerr << std::format("ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ '{}'\n{}", path, e.what()) << std::endl;
	}
	const char* shader_str = shader_src.c_str();

	// Compile the shader
	unsigned int shader;
	int success;
	char infoLog[512];

	shader = glCreateShader(type);
	glShaderSource(shader, 1, &shader_str, NULL);
	glCompileShader(shader);

	// Check for errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	this->shaders.push_back(shader);

	glAttachShader(this->id, shader);
}

void ShaderProgram::link()
{
	int success;
	char infoLog[512];
	glLinkProgram(this->id);
	glGetProgramiv(this->id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->id, 512, NULL, infoLog);
		std::cout << "ERROR:SHADER::PROGRAM_LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete the shaders as they're now linked into the program
	for(GLuint shader : this->shaders) {
		glDeleteShader(shader);
	}
}

void ShaderProgram::use()
{
	glUseProgram(this->id);
}

void ShaderProgram::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), (int)value);
}

void ShaderProgram::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void ShaderProgram::setFloat(const std::string& name, float value) const
{
	glUniform1i(glGetUniformLocation(this->id, name.c_str()), value);
}

void ShaderProgram::setVec3(const std::string& name, glm::vec3 value) const
{
	glUseProgram(this->id);
	glUniform3fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setVec2(const std::string& name, glm::vec2 value) const
{
	glUseProgram(this->id);
	glUniform2fv(glGetUniformLocation(this->id, name.c_str()), 1, glm::value_ptr(value));
}

void ShaderProgram::setMat4(const std::string& name, glm::mat4x4 value) const
{
	glUseProgram(this->id);
	glUniformMatrix4fv(glGetUniformLocation(this->id, name.c_str()), 1, false, glm::value_ptr(value));
}
