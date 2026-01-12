#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex_coord;

out vec4 FragColor;

uniform sampler2D ray_texture;

vec3 less_than(in vec3 f, in float value)
{
	return vec3(
		(f.x < value) ? 1.0f : 0.0f,
		(f.y < value) ? 1.0f : 0.0f,
		(f.z < value) ? 1.0f : 0.0f
	);
}

vec3 linear_to_srgb(vec3 rgba)
{
	vec3 rgb = clamp(rgba.rgb, 0.0f, 1.0f);

	return vec3(
		mix(
			pow(rgb, vec3(1.0f / 2.4f)) * 1.055f - 0.055f,
			rgb * 12.92f,
			less_than(rgb, 0.0031308f)
		)
	);
}

vec3 srgb_to_linear(vec3 rgba)
{
	vec3 rgb = clamp(rgba.rgb, 0.0f, 1.0f);

	return vec3(
		mix(
			pow(((rgb + 0.055f) / 1.055f), vec3(2.4f)),
			rgb / 12.92f,
			less_than(rgb, 0.04045f)
		)
	);
}

vec3 ACESFilm(vec3 x)
{
	float a = 2.51f;
	float b = 0.03f;
	float c = 2.43f;
	float d = 0.59f;
	float e = 0.14f;
	return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0f, 1.0f);
}

void main()
{
	vec3 colour = texture(ray_texture, tex_coord).rgb;
	
	// sRGB mapping, ACESFilm tone mapping
	float exposure = 0.8f;
	colour *= exposure;
	colour = ACESFilm(colour);
	colour = linear_to_srgb(colour);

	// Simple gamma correction
	// colour = pow(colour.rgb, vec3(1.0/2.2));

	FragColor = vec4(colour, 1.0);
}