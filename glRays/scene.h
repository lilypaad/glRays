#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec3 albedo;
    float roughness;
    glm::vec3 emission_colour;
    float emission_strength;
    glm::vec3 specular_colour;
    float specular_chance;
    glm::vec3 refraction_colour;
    float refraction_chance;
    float refraction_roughness;
    float refractive_idx;
    float std140padding1;
    float std140padding2;
};

struct Sphere
{
	Material material;
	glm::vec3 centre;
	float radius;
};

struct SceneData
{
	const void* objects;
	size_t size;
	int n_objects;
};

Material default_material()
{
    Material m;
    m.albedo = glm::vec3(0.0f, 0.0f, 0.0f);
    m.roughness = 0.0f;
    m.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    m.emission_strength = 0.0f;
    m.specular_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    m.specular_chance = 0.0f;
    m.refraction_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    m.refraction_chance = 0.0f;
    m.refraction_roughness = 0.0f;
    m.refractive_idx = 1.0f;

    return m;
}

Material refractive(float rough, float chance=1.0f)
{
    Material m;
    m.albedo = glm::vec3(0.9f, 0.25f, 0.25f);
    m.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    m.emission_strength = 0.0f;
    m.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    m.specular_chance = 0.02f;
    m.roughness = rough;
    m.refraction_colour = glm::vec3(0.0f, 5.0f, 10.0f);
    m.refraction_chance = 1.0f;
    m.refraction_roughness = rough;
    m.refractive_idx = 1.5f;
    return m;
}

Material reflective(float rough, float prob=1.0f)
{
    Material m;
    m.albedo = glm::vec3(1.0f, 1.0f, 1.0f);
    m.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    m.emission_strength = 0.0f;
    m.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    m.specular_chance = prob;
    m.roughness = rough;
    m.refraction_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    m.refraction_chance = 0.0f;
    m.refraction_roughness = 0.0f;
    m.refractive_idx = 1.0f;
    return m;
}

SceneData cornell_box_diffuse() 
{
	const int NUM_SPHERES = 8;

    Sphere* spheres = new Sphere[NUM_SPHERES];

    Sphere sphere0;
    sphere0.centre = glm::vec3(-0.6f, 1.0f, -1.0f);
    sphere0.radius = 0.12f;
    sphere0.material = default_material();
    sphere0.material.albedo = glm::vec3(1.0f, 0.0f, 0.0);

    Sphere sphere1;
    sphere1.centre = glm::vec3(-0.3f, 1.0f, -1.0f);
    sphere1.radius = 0.12f;
    sphere1.material = default_material();
    sphere1.material.albedo = glm::vec3(0.0f, 1.0f, 0.0f);

    Sphere sphere2;
    sphere2.centre = glm::vec3(0.0f, 1.0f, -1.0f);
    sphere2.radius = 0.12f;
    sphere2.material = default_material();
	sphere2.material.albedo = glm::vec3(0.0f, 0.0f, 1.0f);

    Sphere sphere3;
    sphere3.centre = glm::vec3(0.3f, 1.0f, -1.0f);
    sphere3.radius = 0.12f;
    sphere3.material = default_material();
	sphere3.material.albedo = glm::vec3(0.0f, 1.0f, 0.0f);

    Sphere sphere4;
    sphere4.centre = glm::vec3(0.6f, 1.0f, -1.0f);
    sphere4.radius = 0.12f;
    sphere4.material = default_material();
	sphere4.material.albedo = glm::vec3(0.0f, 1.0f, 0.0f);

    Sphere sphere5;
    sphere5.centre = glm::vec3(-0.7f, 0.29f, -0.7f);
    sphere5.radius = 0.3f;
    sphere5.material = default_material();
    sphere5.material.albedo = glm::vec3(1.0f, 1.0f, 0.6f);

    Sphere sphere6;
    sphere6.centre = glm::vec3(0.0f, 0.3f, -0.7f);
    sphere6.radius = 0.3f;
    sphere6.material = default_material();
    sphere6.material.albedo = glm::vec3(1.0f, 0.6f, 0.6f);

    Sphere sphere7;
    sphere7.centre = glm::vec3(0.7f, 0.3f, -0.7f);
    sphere7.radius = 0.3f;
    sphere7.material = default_material();

    spheres[0] = sphere0;
    spheres[1] = sphere1;
    spheres[2] = sphere2;
    spheres[3] = sphere3;
    spheres[4] = sphere4;
    spheres[5] = sphere5;
    spheres[6] = sphere6;
    spheres[7] = sphere7;

    SceneData sceneData;
    sceneData.objects = spheres;
    sceneData.n_objects = NUM_SPHERES;
    sceneData.size = sceneData.n_objects * sizeof(Sphere);

	return sceneData;
}

SceneData cornell_box_metallic() 
{
	const int NUM_SPHERES = 8;

    Sphere* spheres = new Sphere[NUM_SPHERES];

    Sphere sphere0;
    sphere0.centre = glm::vec3(-0.6f, 1.0f, -1.0f);
    sphere0.radius = 0.12f;
    sphere0.material = reflective(0.0f);

    Sphere sphere1;
    sphere1.centre = glm::vec3(-0.3f, 1.0f, -1.0f);
    sphere1.radius = 0.12f;
    sphere1.material = reflective(0.25f);

    Sphere sphere2;
    sphere2.centre = glm::vec3(0.0f, 1.0f, -1.0f);
    sphere2.radius = 0.12f;
    sphere2.material = reflective(0.5f);

    Sphere sphere3;
    sphere3.centre = glm::vec3(0.3f, 1.0f, -1.0f);
    sphere3.radius = 0.12f;
    sphere3.material = reflective(0.75f);

    Sphere sphere4;
    sphere4.centre = glm::vec3(0.6f, 1.0f, -1.0f);
    sphere4.radius = 0.12f;
    sphere4.material = reflective(1.0f);

    Sphere sphere5;
    sphere5.centre = glm::vec3(-0.7f, 0.29f, -0.7f);
    sphere5.radius = 0.3f;
    sphere5.material = default_material();
    sphere5.material.albedo = glm::vec3(1.0f, 1.0f, 0.6f);
    sphere5.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere5.material.emission_strength = 0.0f;
    sphere5.material.specular_colour = glm::vec3(0.9f, 0.9f, 0.9f);
    sphere5.material.roughness = 0.2f;
    sphere5.material.specular_chance = 0.1f;

    Sphere sphere6;
    sphere6.centre = glm::vec3(0.0f, 0.3f, -0.7f);
    sphere6.radius = 0.3f;
    sphere6.material = default_material();
    sphere6.material.albedo = glm::vec3(1.0f, 0.6f, 0.6f);
    sphere6.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere6.material.emission_strength = 0.0f;
    sphere6.material.specular_colour = glm::vec3(0.9f, 0.9f, 0.9f);
    sphere6.material.roughness = 0.2f;
    sphere6.material.specular_chance = 0.3f;

    Sphere sphere7;
    sphere7.centre = glm::vec3(0.7f, 0.3f, -0.7f);
    sphere7.radius = 0.3f;
    sphere7.material = default_material();
    sphere7.material.albedo = glm::vec3(0.0f, 0.0f, 1.0f);
    sphere7.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere7.material.emission_strength = 0.0f;
    sphere7.material.specular_colour = glm::vec3(1.0f, 0.0f, 0.0f);
    sphere7.material.roughness = 0.5f;
    sphere7.material.specular_chance = 0.5f;

    spheres[0] = sphere0;
    spheres[1] = sphere1;
    spheres[2] = sphere2;
    spheres[3] = sphere3;
    spheres[4] = sphere4;
    spheres[5] = sphere5;
    spheres[6] = sphere6;
    spheres[7] = sphere7;

    SceneData sceneData;
    sceneData.objects = spheres;
    sceneData.n_objects = NUM_SPHERES;
    sceneData.size = sceneData.n_objects * sizeof(Sphere);

	return sceneData;
}

SceneData cornell_box_glass() 
{
	//const int NUM_SPHERES = 1;

	//Sphere* spheres = new Sphere[NUM_SPHERES];

	//Sphere sphere0;
	//sphere0.centre = glm::vec3(0.0f, 1.0f, -1.0f);
	//sphere0.radius = 0.5f;
	//sphere0.material = refractive(0.0f);
	//sphere0.material.refraction_colour = glm::vec3(0.0f, 0.0f, 0.0f);
	//spheres[0] = sphere0;

	const int NUM_SPHERES = 8;

    Sphere* spheres = new Sphere[NUM_SPHERES];

    Sphere sphere0;
    sphere0.centre = glm::vec3(-0.6f, 1.0f, -0.2f);
    sphere0.radius = 0.12f;
    sphere0.material = refractive(0.0f);

    Sphere sphere1;
    sphere1.centre = glm::vec3(-0.3f, 1.0f, -0.2f);
    sphere1.radius = 0.12f;
    sphere1.material = refractive(0.25f);

    Sphere sphere2;
    sphere2.centre = glm::vec3(0.0f, 1.0f, -0.2f);
    sphere2.radius = 0.12f;
    sphere2.material = refractive(0.5f);

    Sphere sphere3;
    sphere3.centre = glm::vec3(0.3f, 1.0f, -0.2f);
    sphere3.radius = 0.12f;
    sphere3.material = refractive(0.75f);

    Sphere sphere4;
    sphere4.centre = glm::vec3(0.6f, 1.0f, -0.2f);
    sphere4.radius = 0.12f;
    sphere4.material = refractive(1.0f);

    Sphere sphere5;
    sphere5.centre = glm::vec3(-0.7f, 0.9f, -1.0f);
    sphere5.radius = 0.3f;
    sphere5.material = default_material();
    sphere5.material.albedo = glm::vec3(1.0f, 1.0f, 0.6f);
    sphere5.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere5.material.emission_strength = 0.0f;
    sphere5.material.specular_colour = glm::vec3(0.9f, 0.9f, 0.9f);
    sphere5.material.roughness = 0.2f;
    sphere5.material.specular_chance = 0.1f;

    Sphere sphere6;
    sphere6.centre = glm::vec3(0.0f, 1.0f, -1.0f);
    sphere6.radius = 0.3f;
    sphere6.material = default_material();
    sphere6.material.albedo = glm::vec3(1.0f, 0.6f, 0.6f);
    sphere6.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere6.material.emission_strength = 0.0f;
    sphere6.material.specular_colour = glm::vec3(0.9f, 0.9f, 0.9f);
    sphere6.material.roughness = 0.2f;
    sphere6.material.specular_chance = 0.3f;

    Sphere sphere7;
    sphere7.centre = glm::vec3(0.7f, 0.9f, -1.0f);
    sphere7.radius = 0.3f;
    sphere7.material = default_material();
    sphere7.material.albedo = glm::vec3(0.0f, 0.0f, 1.0f);
    sphere7.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere7.material.emission_strength = 0.0f;
    sphere7.material.specular_colour = glm::vec3(1.0f, 0.0f, 0.0f);
    sphere7.material.roughness = 0.5f;
    sphere7.material.specular_chance = 0.5f;

    spheres[0] = sphere0;
    spheres[1] = sphere1;
    spheres[2] = sphere2;
    spheres[3] = sphere3;
    spheres[4] = sphere4;
    spheres[5] = sphere5;
    spheres[6] = sphere6;
    spheres[7] = sphere7;

    SceneData sceneData;
    sceneData.objects = spheres;
    sceneData.n_objects = NUM_SPHERES;
    sceneData.size = sceneData.n_objects * sizeof(Sphere);

	return sceneData;
}

SceneData default_scene() {
	const int NUM_SPHERES = 10;

    Sphere* spheres = new Sphere[NUM_SPHERES];

    Sphere sphere0;
    sphere0.centre = glm::vec3(0.0f, -100.5f, 0.0f);
    sphere0.radius = 100.0f;
    sphere0.material.albedo = glm::vec3(0.807f, 0.2588f, 0.2588f);
    sphere0.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere0.material.emission_strength = 0.0f;
    sphere0.material.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere0.material.roughness = 1.0f;
    sphere0.material.specular_chance = 0.0f;

    Sphere sphere1;
    sphere1.centre = glm::vec3(0.0f, 13.0f, -2.0f);
    sphere1.radius = 10.0f;
    sphere1.material.albedo = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere1.material.emission_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere1.material.emission_strength = 1.0f;
    sphere1.material.specular_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere1.material.roughness = 1.0f;
    sphere1.material.specular_chance = 1.0f;

    Sphere sphere2;
    sphere2.centre = glm::vec3(-1.0f, 0.5f, -3.0f);
    sphere2.radius = 0.2f;
    sphere2.material = reflective(0.0f);

    Sphere sphere3;
    sphere3.centre = glm::vec3(-0.5f, 0.5f, -3.0f);
    sphere3.radius = 0.2f;
    sphere3.material = reflective(0.25f);

    Sphere sphere4;
    sphere4.centre = glm::vec3(0.0f, 0.5f, -3.0f);
    sphere4.radius = 0.2f;
    sphere4.material = reflective(0.5f);

    Sphere sphere5;
    sphere5.centre = glm::vec3(0.5f, 0.5f, -3.0f);
    sphere5.radius = 0.2f;
    sphere5.material = reflective(0.75f);

    Sphere sphere6;
    sphere6.centre = glm::vec3(1.0f, 0.5f, -3.0f);
    sphere6.radius = 0.2f;
    sphere6.material = reflective(1.0f);

    Sphere sphere7;
    sphere7.centre = glm::vec3(-0.75f, -0.22f, -2.0f);
    sphere7.radius = 0.3f;
    sphere7.material.albedo = glm::vec3(1.0f, 1.0f, 0.6f);
    sphere7.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere7.material.emission_strength = 0.0f;
    sphere7.material.specular_colour = glm::vec3(0.9f, 0.9f, 0.9f);
    sphere7.material.roughness = 0.2f;
    sphere7.material.specular_chance = 0.1f;

    Sphere sphere8;
    sphere8.centre = glm::vec3(0.0f, -0.22f, -2.0f);
    sphere8.radius = 0.3f;
    sphere8.material.albedo = glm::vec3(1.0f, 0.6f, 0.6f);
    sphere8.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere8.material.emission_strength = 0.0f;
    sphere8.material.specular_colour = glm::vec3(0.9f, 0.9f, 0.9f);
    sphere8.material.roughness = 0.2f;
    sphere8.material.specular_chance = 0.3f;

    Sphere sphere9;
    sphere9.centre = glm::vec3(0.75f, -0.22f, -2.0f);
    sphere9.radius = 0.3f;
    sphere9.material.albedo = glm::vec3(0.0f, 0.0f, 1.0f);
    sphere9.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere9.material.emission_strength = 0.0f;
    sphere9.material.specular_colour = glm::vec3(1.0f, 0.0f, 0.0f);
    sphere9.material.roughness = 0.5f;
    sphere9.material.specular_chance = 0.5f;

    spheres[0] = sphere0;
    spheres[1] = sphere1;
    spheres[2] = sphere2;
    spheres[3] = sphere3;
    spheres[4] = sphere4;
    spheres[5] = sphere5;
    spheres[6] = sphere6;
    spheres[7] = sphere7;
    spheres[8] = sphere8;
    spheres[9] = sphere9;

    SceneData sceneData;
    sceneData.objects = spheres;
    sceneData.n_objects = NUM_SPHERES;
    sceneData.size = sceneData.n_objects * sizeof(Sphere);

	return sceneData;
}
