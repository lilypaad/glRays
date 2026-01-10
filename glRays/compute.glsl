#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

// layout(set = 0, binding = 0) buffer Buffer
// {
// 	uint count;
// 	Sphere sphere[];
// }

uniform vec3 camera_loc;
uniform vec2 camera_rot;
uniform vec3 camera_dir;
uniform vec3 camera_up;
uniform vec3 camera_right;
uniform mat4 camera_to_world;

const float PI = 3.1415926535897932385;

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Sphere
{
	vec3 center;
	float r;
};

struct HitInfo
{
	vec3 point;
	vec3 normal;
	float dist;
	bool collided;
};

vec3 at(Ray r, float t)
{
	return r.origin + t * r.direction;
}

float hit_sphere(vec3 center, float radius, Ray r)
{
	vec3 oc = center - r.origin;
	float a = dot(r.direction, r.direction);
	float h = dot(r.direction, oc);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = h*h - a*c;

	if(discriminant < 0)
		return -1.0;
	else
		return (h - sqrt(discriminant)) / a;
}

vec3 unit_vector(vec3 v)
{
	float magnitude = sqrt(pow(v.x, 2.0f) + pow(v.y, 2.0f) + pow(v.z, 2.0f));
	return v / magnitude;
}

vec4 ray_color(Ray ray)
{
	float t = hit_sphere(vec3(0, 0, -5), 1.0, ray);
	if(t > 0.0) {
		vec3 N = normalize(at(ray, t) - camera_loc);
		return vec4(0.5 * vec3(N.x+1, N.y+1, N.z+1), 1.0);
	}

	float a = 0.5 * (ray.direction.y + 1.0);
	return vec4((1.0-a) * vec3(1.0, 1.0, 1.0) + a*vec3(0.5, 0.7, 1.0), 1.0);
}

void main() 
{
	ivec2 pix_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dimensions = imageSize(img_output);

	float aspect_ratio = float(dimensions.x) / float(dimensions.y);
	float fov = 70.0;

	// Translate pixels from raster space -> NDC space -> screen space -> camera space
	// Run-down of the math can be found here:
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
	vec3 pixel_camera = vec3(
		(2 * ((float(pix_coords.x) + 0.5) / dimensions.x) - 1) * tan(fov / 2 * PI / 180) * aspect_ratio,
		(2 * ((float(pix_coords.y) + 0.5) / dimensions.y) - 1) * tan(fov / 2 * PI / 180),
		-1.0
	);

	// Camera space -> world space
	vec3 ray_origin = vec3(camera_to_world * vec4(vec3(0.0), 1.0));
	vec3 P_world = vec3(camera_to_world * vec4(pixel_camera, 1.0));
	vec3 ray_direction = normalize(P_world - ray_origin);
	
	// TEST PATTERN: xy pix coords (bottom left black, top right R+G
	// vec4 pixel = vec4(pix_coords / vec2(800, 600), 0, 1);

	// TEST PATTER: ray direction vector -> rgb
	// vec4 pixel = vec4(ray_direction, 1);

	Ray r = Ray(ray_origin, ray_direction); 
	vec4 pixel = ray_color(r);

	imageStore(img_output, pix_coords, pixel);
}
