#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

uniform vec3 camera_loc;
uniform vec2 camera_rot;
uniform vec3 camera_dir;
uniform vec3 camera_up;
uniform vec3 camera_right;
uniform mat4 camera_to_world;

const float PI = 3.1415926535897932385;
const float INFINITY = 1.0 / 0.0;
const int n_spheres = 4;

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Material
{
	vec4 colour;
    vec3 emission_colour;
    float emission_strength;
};

struct Sphere
{
	Material material;
	vec3 centre;
	float radius;
};

struct HitInfo
{
	Material material;
	vec3 point;
	vec3 normal;
	float dist;
	bool collided;
};

layout (std140, binding = 1) uniform scene_buffer
{
	Sphere u_spheres[n_spheres];
};

HitInfo hit_sphere(vec3 centre, float radius, Ray r)
{
	HitInfo hit;
	hit.collided = false;

	vec3 oc = centre - r.origin;
	float a = dot(r.direction, r.direction);
	float h = dot(r.direction, oc);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = h*h - a*c;

	if(discriminant >= 0) {
		float dst = (h - sqrt(discriminant)) / a;
		if (dst >= 0) {
			hit.collided = true;
			hit.dist = dst;
			hit.point = r.origin + r.direction * dst;
			hit.normal = normalize(hit.point - centre);
		}
	}
	return hit;
}

HitInfo ray_collision(Ray ray)
{
	Material default_colour = {
		vec4(0.2, 0.2, 0.7, 1),
		vec3(0, 0, 0),
		0.0
	};

	HitInfo closest;
	closest.dist = INFINITY;
	closest.material = default_colour;

	for (int i = 0; i < n_spheres; i++) {
		Sphere sphere = u_spheres[i];
		HitInfo hit = hit_sphere(sphere.centre, sphere.radius, ray);

		if (hit.collided && hit.dist < closest.dist) {
			closest = hit;
			closest.material = sphere.material;
		}
	}

	return closest;
}

vec4 ray_color(Ray ray)
{
	// HitInfo hit = hit_sphere(vec3(0, 0, -5), 1.0, ray);
	// return vec4(float(hit.collided) * vec3(1.0), 1.0);

	return ray_collision(ray).material.colour;
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

	// TEST PATTERN: ray direction vector -> rgb
	// vec4 pixel = vec4(ray_direction, 1);

	Ray r = Ray(ray_origin, ray_direction); 
	vec4 pixel = ray_color(r);

	imageStore(img_output, pix_coords, pixel);
}
