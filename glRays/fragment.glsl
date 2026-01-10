#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord;

out vec4 FragColor;

uniform sampler2D ray_texture;

void main()
{
	FragColor = texture(ray_texture, tex_coord);
}