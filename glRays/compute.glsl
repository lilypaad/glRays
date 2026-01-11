#version 430 core

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

uniform int u_frame_count;
uniform bool u_camera_moved;
uniform float u_fov;
uniform mat4 camera_to_world;
uniform int u_max_bounces;
uniform int u_rays_per_pixel;

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
	vec3 colour;                  // offset 0   // align 16  // total 16
    float std140padding1;         // offset 12  // align 4   // total 16
    vec3 emission_colour;         // offset 16  // align 16  // total 32
    float std140padding2;         // offset 28  // align 4   // total 32
    vec3 specular_colour;         // offset 32  // align 16  // total 48
    float std140padding3;         // offset 44  // align 4   // total 48
    float specular_probability;   // offset 48  // align 4   // total 52
    float emission_strength;      // offset 52  // align 4   // total 56
    float smoothness;             // offset 56  // align 4   // total 60
    float std140padding4;         // offset 60  // align 4   // total 64
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

struct Triangle
{
	vec3 a;
	vec3 b;
	vec3 c;
	vec3 normal;
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
}

vec3 environment_light(Ray ray)
{
	vec3 colour_ground = vec3(0.5, 0.5, 0.5);
	vec3 colour_sky_horizon = vec3(1.0, 1.0, 1.0);
	vec3 colour_sky_zenith = vec3(0.5, 0.7, 1.0);
	vec3 sun_direction = normalize(vec3(2.5, 1, -1));
	float sun_focus = 50.0;
	float sun_intensity = 40.0;

	float gradient_interp = pow(smoothstep(0.0, 0.4, ray.direction.y), 0.35);
	float ground_to_sky = smoothstep(-0.01, 0.0, ray.direction.y);
	vec3 gradient = mix(colour_sky_horizon, colour_sky_zenith, gradient_interp);
	float sun = pow(max(0, dot(ray.direction, sun_direction)), sun_focus) * sun_intensity;

	vec3 composite = mix(colour_ground, gradient, ground_to_sky) + sun * float(ground_to_sky >= 1);
	return composite;
}

HitInfo hit_triangle(Triangle tri, Ray ray)
{
	HitInfo hit;
	hit.collided = false;

	vec3 AB = tri.b - tri.a;
	vec3 AC = tri.c - tri.a;
	vec3 normal = cross(AB, AC);
	float det = -dot(ray.direction, normal);
	float invdet = 1.0 / det;
	vec3 AO = ray.origin - tri.a;
	vec3 DAO = cross(AO, ray.direction);
	float u = dot(AC, DAO) * invdet;
	float v = -dot(AB, DAO) * invdet;
	float w = 1 - u - v;
	float t = dot(AO, normal) * invdet;

	bool did_hit = (det >= 1e-6 && t >= 0.0 && u >= 0.0 && v >= 0.0 && (u+v) <= 1.0);

	hit.collided = did_hit;
	hit.point = ray.origin + ray.direction * t;
	hit.normal = normalize(tri.normal * w + tri.normal * u + tri.normal * v);
	hit.dist = t;
	return hit;
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
	Material default_colour = { 
		vec3(0, 0, 0), 
		0.0, 
		vec3(0, 0, 0), 
		0.0, 
		vec3(0, 0, 0), 
		0.0, 
		0.0, 0.0, 0.0, 0.0
	};

	HitInfo closest;
	closest.dist = INFINITY;
	closest.material = default_colour;
	closest.collided = false;

	for (int i = 0; i < n_spheres; i++) {
		Sphere sphere = u_spheres[i];
		HitInfo hit = hit_sphere(sphere.centre, sphere.radius, ray);

		if (hit.collided && hit.dist < closest.dist) {
			closest = hit;
			closest.material = sphere.material;
		}
	}

	// FIXME -- test tri, iterate this over all mesh info
	Triangle tri = { 
		vec3(-1.0, 0.0, -1.0),
		vec3( 0.0, 2.0,  1.0),
		vec3( 1.0, 0.0,  1.0),
		vec3(0,0,0)
	};
	vec3 U = tri.b - tri.a;
	vec3 V = tri.c - tri.a;
	tri.normal = vec3(
		U.y * V.z - U.z * V.y,
		U.z * V.x - U.x * V.z,
		U.x * V.y - U.y * V.x
	);
	Material tempmat = {
		vec3(1.0, 1.0, 1.0),
		0.0,
		vec3(1.0, 1.0, 1.0),
		0.0,
		vec3(1.0, 1.0, 1.0),
		0.0,
		0.0, 0.0, 0.0, 0.0
	};

	HitInfo hittri = hit_triangle(tri, ray);
	if(hittri.collided && hittri.dist < closest.dist) {
		closest = hittri;
		closest.material = tempmat;
	}
	// FIXME -- test tri


	return closest;
}

vec3 trace(Ray ray)
{
	vec3 incoming_light = vec3(0.0, 0.0, 0.0);
	vec3 ray_colour = vec3(1.0, 1.0, 1.0);

	for(int i = 0; i <= u_max_bounces; i++)
	{
		HitInfo hit = ray_collision(ray);
		if(hit.collided)
		{
			ray.origin = hit.point;
			ray.direction = normalize(hit.normal + random_direction());

			vec3 emitted_light = hit.material.emission_colour * hit.material.emission_strength;
			incoming_light += emitted_light * ray_colour ;
			ray_colour *= hit.material.colour;
		}
		else
		{
			incoming_light += environment_light(ray) * ray_colour;
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
		(2 * ((float(pix_coords.x) + 0.5) / dims.x) - 1) * tan(u_fov / 2 * PI / 180) * aspect_ratio,
		(2 * ((float(pix_coords.y) + 0.5) / dims.y) - 1) * tan(u_fov / 2 * PI / 180),
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

	for (int i = 0; i < u_rays_per_pixel; i++)
		total_light += trace(r);
	total_light = total_light / u_rays_per_pixel;

	float weight = 1.0f / float(u_frame_count + 1);
	vec3 pixel_col = accumulated_colour.rgb * (1 - weight) + total_light * weight;
	vec4 pixel = vec4(pixel_col, 1.0);
	imageStore(img_output, pix_coords, pixel);

	barrier();
}
