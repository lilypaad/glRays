#version 430 core

layout(local_size_x = 4, local_size_y = 4, local_size_z = 1) in;

layout(rgba32f, binding = 0) uniform image2D img_output;

uniform int u_frame_count;
uniform bool u_camera_moved;
uniform float u_fov;
uniform mat4 camera_to_world;
uniform int u_max_bounces;
uniform int u_rays_per_pixel;

const float PI = 3.1415926535897932385;
const float INFINITY = 1.0 / 0.0;
const int n_spheres = 8;

struct Ray
{
	vec3 origin;
	vec3 direction;
};

struct Material
{
	vec3 albedo;
    float roughness;
    vec3 emission_colour;
    float emission_strength;
    vec3 specular_colour;
    float specular_chance;
	vec3 refraction_colour;
    float refraction_chance;
	float refraction_roughness;
    float refractive_idx;
    float std140padding1;
    float std140padding2;
};

struct Sphere
{
	Material material;
	vec3 centre;
	float radius;
};

struct Triangle
{
	Material material;
	vec3 a;
	vec3 b;
	vec3 c;
	vec3 normal;
};

struct HitInfo
{
	Material material;
	vec3 point;
	vec3 normal;
	float dist;
	bool collided;
	bool from_inside;
};

unsigned int rng_state = 0;

layout (std140, binding = 2) uniform scene_buffer
{
	Sphere u_spheres[n_spheres];
};


/*
	Utility functions
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

// Random direction vector
vec3 random_direction()
{
	return normalize(vec3(rand_gauss(), rand_gauss(), rand_gauss()));
}

// Random direction vector in hemisphere based on normal, cosine-weighted distribution
vec3 random_direction_hemisphere_cos(vec3 normal)
{
	return normalize(normal + random_direction());
}

// Calculate a triangle's normal vector
vec3 tri_normal(Triangle tri)
{
	vec3 U = tri.b - tri.a;
	vec3 V = tri.c - tri.a;
	return vec3(
		U.y * V.z - U.z * V.y,
		U.z * V.x - U.x * V.z,
		U.x * V.y - U.y * V.x
	);
}

Material default_material()
{
    Material m;
    m.albedo = vec3(0.0f, 0.0f, 0.0f);
    m.roughness = 0.0f;
    m.emission_colour = vec3(0.0f, 0.0f, 0.0f);
    m.emission_strength = 0.0f;
    m.specular_colour = vec3(0.0f, 0.0f, 0.0f);
    m.specular_chance = 0.0f;
    m.refraction_colour = vec3(0.0f, 0.0f, 0.0f);
    m.refraction_chance = 0.0f;
    m.refraction_roughness = 0.0f;
    m.refractive_idx = 1.0f;
    m.std140padding1 = 0.0f;
    m.std140padding2 = 0.0f;
	return m;
}

/*
	Ray tracing related functions
*/

// Schlick Fresnel approximation
float fresnel_reflect_amount(float n1, float n2, vec3 normal, vec3 incident, float f0, float f90)
{
	float r0 = (n1 - n2) / (n1 + n2);
	r0 *= r0;
	float cos_x = -dot(normal, incident);
	if(n1 > n2) {
		float n = n1 / n2;
		float sin_t2 = n * n * (1.0 - cos_x * cos_x);
		if(sin_t2 > 1.0)
			return f90;
		cos_x = sqrt(1.0 - sin_t2);
	}
	float x = 1.0 - cos_x;
	float ret = r0 + (1.0 - r0) * pow(x, 5.0);

	return mix(f0, f90, ret);
}

vec3 environment_light(Ray ray)
{
	vec3 colour_ground = vec3(0.5, 0.5, 0.5);
	vec3 colour_sky_horizon = vec3(1.0, 1.0, 1.0);
	vec3 colour_sky_zenith = vec3(0.5, 0.7, 1.0);
	vec3 sun_direction = normalize(vec3(0.5, 1, -1));
	float sun_focus = 50.0;
	float sun_intensity = 20.0;

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

HitInfo hit_sphere(vec3 centre, float radius, float t_min, float t_max, Ray ray)
{
	HitInfo hit;
	hit.collided = false;
	hit.from_inside = false;

	vec3 oc = centre - ray.origin;
	float a = dot(ray.direction, ray.direction);
	float h = dot(ray.direction, oc);
	float c = dot(oc, oc) - radius * radius;

	float discriminant = h * h - a * c;

	if (discriminant < 0.0f)
		return hit; // missed

	// Try to find a root within interval (t_min, t_max)
	float root = (h - sqrt(discriminant)) / a ;
	if (root <= t_min || t_max <= root) {
		root = (h + sqrt(discriminant)) / a ;
		if (root <= t_min || t_max <= root)
			return hit; // outside of acceptable range for t
	}

	hit.collided = true;
	hit.dist = root;
	hit.point = ray.origin + ray.direction * root;
	hit.normal = (hit.point - centre) / radius;
	hit.from_inside = dot(ray.direction, hit.normal) > 0.001f;
	hit.normal = hit.from_inside ? -hit.normal : hit.normal;

	return hit;
}

HitInfo ray_collision(Ray ray)
{
	HitInfo closest;
	closest.dist = INFINITY;
	closest.material = default_material();
	closest.collided = false;
	closest.from_inside = false;

	for (int i = 0; i < n_spheres; i++) {
		Sphere sphere = u_spheres[i];
		HitInfo hit = hit_sphere(sphere.centre, sphere.radius, 0.001f, INFINITY, ray);

		if (hit.collided && hit.dist < closest.dist) {
			closest = hit;
			closest.material = sphere.material;
		}
	}

	// FIXME -- test tri, iterate this over all mesh info
	vec3 white = vec3(1.0, 1.0, 1.0);
	vec3 red = vec3(1.0, 0.0, 0.0);
	vec3 green = vec3(0.0, 1.0, 0.0);
	Material white_wall = default_material();
	white_wall.albedo = vec3(1.0, 1.0, 1.0);
	Material red_wall = default_material();
	red_wall.albedo = vec3(1, 0, 0);
	Material green_wall = default_material();
	green_wall.albedo = vec3(0, 1, 0);
	Material light = default_material();
	light.albedo = vec3(1, 1, 1);
	light.emission_colour = vec3(1, 1, 1);
	light.emission_strength = 10.0;

	const int n_tris = 14;
	Triangle u_tris[n_tris];

	{
		// floor
		Triangle tri0 = { 
			white_wall,
			vec3(-1.0,  0.0, -2.0),
			vec3(-1.0,  0.0,  0.0),
			vec3( 1.0,  0.0,  0.0),
			vec3(0,0,0)
		};
		tri0.normal = tri_normal(tri0);
		u_tris[0] = tri0;
		Triangle tri1 = { 
			white_wall,
			vec3(-1.0,  0.0, -2.0),
			vec3( 1.0,  0.0,  0.0),
			vec3( 1.0,  0.0, -2.0),
			vec3(0,0,0)
		};
		tri1.normal = tri_normal(tri1);
		u_tris[1] = tri1;

		// left wall
		Triangle tri2 = { 
			red_wall,
			vec3(-1.0,  0.0,  0.0),
			vec3(-1.0,  0.0, -2.0),
			vec3(-1.0,  2.0,  0.0),
			vec3(0,0,0)
		};
		tri2.normal = tri_normal(tri2);
		u_tris[2] = tri2;
		Triangle tri3 = { 
			red_wall,
			vec3(-1.0,  2.0, -2.0),
			vec3(-1.0,  2.0,  0.0),
			vec3(-1.0,  0.0, -2.0),
			vec3(0,0,0)
		};
		tri3.normal = tri_normal(tri3);
		u_tris[3] = tri3;

		// right wall
		Triangle tri4 = { 
			green_wall,
			vec3( 1.0,  0.0, -2.0),
			vec3( 1.0,  0.0,  0.0),
			vec3( 1.0,  2.0,  0.0),
			vec3(0,0,0)
		};
		tri4.normal = tri_normal(tri4);
		u_tris[4] = tri4;
		Triangle tri5 = { 
			green_wall,
			vec3( 1.0,  2.0,  0.0),
			vec3( 1.0,  2.0, -2.0),
			vec3( 1.0,  0.0, -2.0),
			vec3(0,0,0)
		};
		tri5.normal = tri_normal(tri5);
		u_tris[5] = tri5;

		// back wall
		Triangle tri6 = { 
			white_wall,
			vec3(-1.0,  2.0, -2.0),
			vec3(-1.0,  0.0, -2.0),
			vec3( 1.0,  0.0, -2.0),
			vec3(0,0,0)
		};
		tri6.normal = tri_normal(tri6);
		u_tris[6] = tri6;
		Triangle tri7 = { 
			white_wall,
			vec3( 1.0,  2.0, -2.0),
			vec3(-1.0,  2.0, -2.0),
			vec3( 1.0,  0.0, -2.0),
			vec3(0,0,0)
		};
		tri7.normal = tri_normal(tri7);
		u_tris[7] = tri7;

		// ceiling
		Triangle tri8 = { 
			white_wall,
			vec3(-1.0,  2.0,  0.0),
			vec3(-1.0,  2.0, -2.0),
			vec3( 1.0,  2.0,  0.0),
			vec3(0,0,0)
		};
		tri8.normal = tri_normal(tri8);
		u_tris[8] = tri8;
		Triangle tri9 = { 
			white_wall,
			vec3( 1.0,  2.0,  0.0),
			vec3(-1.0,  2.0, -2.0),
			vec3( 1.0,  2.0, -2.0),
			vec3(0,0,0)
		};
		tri9.normal = tri_normal(tri9);
		u_tris[9] = tri9;

		// light
		Triangle tri10 = { 
			light,
			vec3( 0.25,  1.99, -0.5),
			vec3(-0.25,  1.99, -1.0),
			vec3( 0.25,  1.99, -1.0),
			vec3(0,0,0)
		};
		tri10.normal = tri_normal(tri10);
		u_tris[10] = tri10;
		Triangle tri11 = { 
			light,
			vec3(-0.25,  1.99, -0.5),
			vec3(-0.25,  1.99, -1.0),
			vec3( 0.25,  1.99, -0.5),
			vec3(0,0,0)
		};
		tri11.normal = tri_normal(tri11);
		u_tris[11] = tri11;

		// front wall (typically looking through this wall)
		Triangle tri12 = { 
			white_wall,
			vec3(-1.0,  0.0,  0.0),
			vec3(-1.0,  2.0,  0.0),
			vec3( 1.0,  0.0,  0.0),
			vec3(0,0,0)
		};
		tri12.normal = tri_normal(tri12);
		u_tris[12] = tri12;
		Triangle tri13 = { 
			white_wall,
			vec3(-1.0,  2.0,  0.0),
			vec3( 1.0,  2.0,  0.0),
			vec3( 1.0,  0.0,  0.0),
			vec3(0,0,0)
		};
		tri13.normal = tri_normal(tri13);
		u_tris[13] = tri13;
	}

	for (int i = 0; i < n_tris; i++) {
		Triangle tri = u_tris[i];
		HitInfo hittri = hit_triangle(tri, ray);

		if (hittri.collided && hittri.dist < closest.dist) {
			closest = hittri;
			closest.material = tri.material;
		}
	}

	// FIXME -- test tri


	return closest;
}

vec3 trace(Ray ray)
{
	vec3 incoming_light = vec3(0.0f, 0.0f, 0.0f);
	vec3 ray_colour = vec3(1.0f, 1.0f, 1.0f);

	for(int i = 0; i <= u_max_bounces; i++)
	{
		HitInfo hit = ray_collision(ray);

		if(hit.collided)
		{
			if (hit.from_inside)
				ray_colour *= exp(-hit.material.refraction_colour * hit.dist);

			// Fresnel reflections
			float spec_chance = hit.material.specular_chance;
			float ref_chance = hit.material.refraction_chance;
			float diff_chance = max(0.0f, 1.0f - spec_chance - ref_chance);
			float ray_prob = 1.0f;
			if (spec_chance > 0.0f) {
				spec_chance = fresnel_reflect_amount(
					hit.from_inside ? hit.material.refractive_idx : 1.0,
					!hit.from_inside ? hit.material.refractive_idx : 1.0,
					ray.direction, hit.normal, hit.material.specular_chance, 1.0f
				);
				float chance_multiplier = (1.0f - spec_chance) / (1.0f - hit.material.specular_chance);
				ref_chance *= chance_multiplier;
				diff_chance *= chance_multiplier;
			}

			// Determine if we're doing specular reflection, diffuse reflection, or refraction
			float rng_roll = rand();
			float is_specular = 0.0f;
			float is_refract = 0.0f;

			if (spec_chance > 0.0f && rng_roll < spec_chance) {
				is_specular = 1.0f;
				ray_prob = spec_chance;
			}
			else if (ref_chance > 0.0f && rng_roll < spec_chance + ref_chance) {
				is_refract = 1.0f;
				ray_prob = ref_chance;
			}
			else {
				ray_prob = 1.0f - spec_chance - ref_chance;
			}
			ray_prob = max(ray_prob, 0.001f); // avoid divide by 0

			// Update bounce ray pos based on if this is a refraction or not
			if (is_refract == 1.0f)
				ray.origin -= hit.normal * 0.001f;
			else
				ray.origin += hit.normal * 0.001f;

			// Generate the new bounce ray
			// Diffuse uses a cosine-weighted random direction in hemisphere
			// 100% smooth specular uses a perfect reflection
			// Rough specular lerps between smooth specular and diffuse
			ray.origin = hit.point;
			vec3 diffuse_ray_dir = random_direction_hemisphere_cos(hit.normal);
			vec3 specular_ray_dir = reflect(ray.direction, hit.normal);
			float ri = hit.from_inside ? hit.material.refractive_idx : 1.0f / hit.material.refractive_idx;
			vec3 refract_ray_dir = refract(ray.direction, hit.normal, ri);
			specular_ray_dir = mix(specular_ray_dir, diffuse_ray_dir, hit.material.roughness * hit.material.roughness);
			refract_ray_dir = mix(refract_ray_dir, -diffuse_ray_dir, hit.material.refraction_roughness * hit.material.refraction_roughness);
			ray.direction = mix(diffuse_ray_dir, specular_ray_dir, is_specular);
			ray.direction = mix(ray.direction, refract_ray_dir, is_refract);

			vec3 emitted_light = hit.material.emission_colour * hit.material.emission_strength;
			incoming_light += emitted_light * ray_colour;
			if (is_refract == 0.0f)
				ray_colour *= mix(hit.material.albedo, hit.material.specular_colour, is_specular);
			ray_colour /= ray_prob;

			// Russian Roulette -- rays with low brightness have high 
			// probability to terminate early. Surviving rays are boosted
			// to compensate for the reduced amount of samples.
			float p = max(ray_colour.r, max(ray_colour.g, ray_colour.b));
			if(rand() > p)
				break;
			ray_colour *= 1.0f / p;
		}
		else
		{
			// Ray didn't hit, use whatever global environment lighting we've defined
			// incoming_light += environment_light(ray) * ray_colour;
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
		(2 * ((float(pix_coords.x) + rand()) / dims.x) - 1) * tan(u_fov / 2 * PI / 180) * aspect_ratio,
		(2 * ((float(pix_coords.y) + rand()) / dims.y) - 1) * tan(u_fov / 2 * PI / 180),
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
	vec3 pixel_col = mix(accumulated_colour.rgb, total_light, weight);
	vec4 pixel = vec4(pixel_col, 1.0);

	imageStore(img_output, pix_coords, pixel);

	barrier();
}
