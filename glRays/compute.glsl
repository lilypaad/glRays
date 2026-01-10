#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

uniform mat4 camera_to_world;
uniform int u_frame_count;
uniform bool u_camera_moved;

const float PI = 3.1415926535897932385;
const float INFINITY = 1.0 / 0.0;
const int n_spheres = 4;
const int max_bounces = 64;
const int rays_per_pixel = 1;

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Material
{
	vec3 colour;
    float emission_strength;
    vec3 emission_colour;
	float std140padding;
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

unsigned int rng_state = 0;

layout (std140, binding = 2) uniform scene_buffer
{
	Sphere u_spheres[n_spheres];
};


/*
	Begin implementation
*/

float rand()
{
	rng_state = rng_state * 747796405 + 2891336453;
	unsigned int result = ((rng_state >> ((rng_state >> 28) + 4)) ^ rng_state) * 277803737;
	result = (result >> 22) ^ result;
	return result / 4294967295.0;
}

float rand_gauss()
{
	float theta = 2 * PI * rand();
	float rho = sqrt(-2 * log(rand()));
	return rho * cos(theta);
}

vec3 random_direction()
{
	return normalize(vec3(rand_gauss(), rand_gauss(), rand_gauss()));
}

vec3 random_direction_hemisphere(vec3 normal)
{
	vec3 dir = random_direction();
	if(dot(normal, dir) < 0)
		dir = -dir;
	return dir;

	//return dir * sign(dot(normal, dir));
}

HitInfo hit_sphere(vec3 centre, float radius, Ray ray)
{
	HitInfo hit;
	hit.collided = false;

	vec3 oc = ray.origin - centre;
	float a = dot(ray.direction, ray.direction);
	float b = 2 * dot(oc, ray.direction);
	float c = dot(oc, oc) - radius * radius;
	float discriminant = b*b - 4*a*c;

	if(discriminant >= 0) {
		float dst = (-b - sqrt(discriminant)) / (2*a);
		if (dst >= 0) {
			hit.collided = true;
			hit.dist = dst;
			hit.point = ray.origin + ray.direction * dst;
			hit.normal = normalize(hit.point - centre);
		}
	}
	return hit;
}

HitInfo ray_collision(Ray ray)
{
	Material default_colour = { vec3(0, 0, 0), 0.0, vec3(0, 0, 0), 0.0 };

	HitInfo closest;
	closest.dist = INFINITY;
	closest.material = default_colour;

	// maybe remove later
	//closest.point = vec3(0, 0, 0);
	//closest.normal = vec3(0,0,0);
	closest.collided = false;

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

vec3 trace(Ray ray)
{
	vec3 incoming_light = vec3(0.0, 0.0, 0.0);
	vec3 ray_colour = vec3(1.0, 1.0, 1.0);

	for(int i = 0; i <= max_bounces; i++)
	{
		HitInfo hit = ray_collision(ray);
		if(hit.collided)
		{
			ray.origin = hit.point;
			ray.direction = random_direction_hemisphere(hit.normal);

			vec3 emitted_light = hit.material.emission_colour * hit.material.emission_strength;
			incoming_light += emitted_light * ray_colour ;
			ray_colour = ray_colour * hit.material.colour;
		}
		else
		{
			break;
		}
	}

	return incoming_light;
}

void main() 
{
	ivec2 pix_coords = ivec2(gl_GlobalInvocationID.xy);
	ivec2 dims = imageSize(img_output);

	// Initialise rng seed
	rng_state = (pix_coords.y * dims.x * dims.y + pix_coords.x) + u_frame_count * 719393;

	float aspect_ratio = float(dims.x) / float(dims.y);
	float fov = 70.0;

	// Translate pixels from raster space -> NDC space -> screen space -> camera space
	// Run-down of the math can be found here:
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays.html
	vec3 pixel_camera = vec3(
		(2 * ((float(pix_coords.x) + 0.5) / dims.x) - 1) * tan(fov / 2 * PI / 180) * aspect_ratio,
		(2 * ((float(pix_coords.y) + 0.5) / dims.y) - 1) * tan(fov / 2 * PI / 180),
		-1.0
	);

	// Camera space -> world space
	vec3 ray_origin = vec3(camera_to_world * vec4(0.0, 0.0, 0.0, 1.0));
	vec3 P_world = vec3(camera_to_world * vec4(pixel_camera, 1.0));
	vec3 ray_direction = normalize(P_world - ray_origin);
	Ray r = Ray(ray_origin, ray_direction); 
	
	// TEST PATTERN: xy pix coords (bottom left black, top right R+G
	// vec4 pixel = vec4(pix_coords / vec2(800, 600), 0, 1);

	// TEST PATTERN: ray direction vector -> rgb
	// vec4 pixel = vec4(ray_direction, 1);

	// TEST PATTERN: gaussian noise
	// vec4 pixel = vec4(normalize(vec3(rand_gauss(), rand_gauss(), rand_gauss())), 1.0);


	if(u_camera_moved)
		imageStore(img_output, pix_coords, vec4(0.0, 0.0, 0.0, 1.0));

	vec4 accumulated_colour = imageLoad(img_output, pix_coords);

	vec3 total_light = vec3(0.0);

	for (int i = 0; i < rays_per_pixel; i++)
		total_light += trace(r);
	total_light = total_light / rays_per_pixel;

	float weight = 1.0f / float(u_frame_count + 1);
	vec3 pixel_col = accumulated_colour.rgb * (1 - weight) + total_light * weight;
	vec4 pixel = vec4(pixel_col, 1.0);
	imageStore(img_output, pix_coords, pixel);

	barrier();
}
