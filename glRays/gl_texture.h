#include <iostream>

class GLTexture
{
	int w;
	int h;
	GLuint id;

public:
	GLTexture(int width, int height) : w(width), h(height), id(-1) {}

	~GLTexture()
	{
		std::clog << "Deleting texture " << id << std::endl;
		glDeleteTextures(1, &id);
	}

	int width() const{ return this->w; }
	int height() const{ return this->h; }
	
	void create_texture()
	{
		glGenTextures(1, &id);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->w, this->h, 0, GL_RGBA, GL_FLOAT, NULL);
		glBindImageTexture(0, id, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	}

	void bind(GLenum tex_unit = GL_TEXTURE0)
	{
		glActiveTexture(tex_unit);
		glBindTexture(GL_TEXTURE_2D, id);
	}
};