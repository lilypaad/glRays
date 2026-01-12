#pragma once

#include <glm/glm.hpp>

const int NUM_SPHERES = 7;

struct Material
{
	glm::vec3 colour;             // offset 0   // align 16  // total 16
    float std140padding1;         // offset 12  // align 4   // total 16
    glm::vec3 emission_colour;    // offset 16  // align 16  // total 32
    float std140padding2;         // offset 28  // align 4   // total 32
    glm::vec3 specular_colour;    // offset 32  // align 16  // total 48
    float std140padding3;         // offset 44  // align 4   // total 48
    float emission_strength;      // offset 48  // align 4   // total 52
    float specular_probability;   // offset 52  // align 4   // total 56
    float roughness;             // offset 56  // align 4   // total 60
    float std140padding4;         // offset 60  // align 4   // total 64
};

struct Sphere
{
	Material material;            // offset 0   // alignment 16  // size 32  // total 64 bytes
	glm::vec3 centre;             // offset 64  // alignment 16  // size 12  // total 80 bytes
	float radius;                 // offset 80  // alignment 4   // size 4   // total 84 bytes
};

struct SceneData
{
	const void* objects;
	size_t size;
	int n_objects;
};

Material reflective(float smooth)
{
    Material m;
    m.colour = glm::vec3(1.0f, 1.0f, 1.0f);
    m.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    m.emission_strength = 0.0f;
    m.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    m.roughness = smooth;
    m.specular_probability = 1.0f;
    return m;
}

SceneData defaultScene() {
    Sphere* spheres = new Sphere[NUM_SPHERES];

    Sphere sphere0;
    sphere0.centre = glm::vec3(0.0f, -100.5f, 0.0f);
    sphere0.radius = 100.0f;
    sphere0.material.colour = glm::vec3(0.807f, 0.2588f, 0.2588f);
    sphere0.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere0.material.emission_strength = 0.0f;
    sphere0.material.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere0.material.roughness = 1.0f;
    sphere0.material.specular_probability = 0.0f;

    Sphere sphere1;
    sphere1.centre = glm::vec3(0.0f, 13.0f, -2.0f);
    sphere1.radius = 10.0f;
    sphere1.material.colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere1.material.emission_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere1.material.emission_strength = 1.0f;
    sphere1.material.specular_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere1.material.roughness = 1.0f;
    sphere1.material.specular_probability = 1.0f;

    Sphere sphere2;
    sphere2.centre = glm::vec3(-1.0f, 0.0f, -2.0f);
    sphere2.radius = 0.2f;
    sphere2.material = reflective(0.0f);

    Sphere sphere3;
    sphere3.centre = glm::vec3(-0.5f, 0.0f, -2.0f);
    sphere3.radius = 0.2f;
    sphere3.material = reflective(0.25f);

    Sphere sphere4;
    sphere4.centre = glm::vec3(0.0f, 0.0f, -2.0f);
    sphere4.radius = 0.2f;
    sphere4.material = reflective(0.5f);

    Sphere sphere5;
    sphere5.centre = glm::vec3(0.5f, 0.0f, -2.0f);
    sphere5.radius = 0.2f;
    sphere5.material = reflective(0.75f);

    Sphere sphere6;
    sphere6.centre = glm::vec3(1.0f, 0.0f, -2.0f);
    sphere6.radius = 0.2f;
    sphere6.material = reflective(1.0f);

    spheres[0] = sphere0;
    spheres[1] = sphere1;
    spheres[2] = sphere2;
    spheres[3] = sphere3;
    spheres[4] = sphere4;
    spheres[5] = sphere5;
    spheres[6] = sphere6;

    SceneData sceneData;
    sceneData.objects = spheres;
    sceneData.n_objects = NUM_SPHERES;
    sceneData.size = sceneData.n_objects * sizeof(Sphere);

	return sceneData;
}
