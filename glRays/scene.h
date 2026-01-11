#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec3 colour;             // offset 0   // align 16  // total 16
    float std140padding1;         // offset 12  // align 4   // total 16
    glm::vec3 emission_colour;    // offset 16  // align 16  // total 32
    float std140padding2;         // offset 28  // align 4   // total 32
    glm::vec3 specular_colour;    // offset 32  // align 16  // total 48
    float std140padding3;         // offset 44  // align 4   // total 48
    float specular_probability;   // offset 48  // align 4   // total 52
    float emission_strength;      // offset 52  // align 4   // total 56
    float smoothness;             // offset 56  // align 4   // total 60
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

SceneData defaultScene() {
    Sphere* spheres = new Sphere[4];

    Sphere sphere1;
    sphere1.centre = glm::vec3(0.0f, -500.5f, 0.0f);
    sphere1.radius = 500.0f;
    sphere1.material.colour = glm::vec3(0.807f, 0.2588f, 0.2588f); // Red colour
    sphere1.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere1.material.emission_strength = 0.0f;
    sphere1.material.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere1.material.smoothness = 1.0f;
    sphere1.material.specular_probability = 1.0f;

    Sphere sphere2;
    sphere2.centre = glm::vec3(-0.5f, 1.5f, -5.0f);
    sphere2.radius = 2.0f;
    sphere2.material.colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere2.material.emission_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere2.material.emission_strength = 0.0f;
    sphere1.material.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere1.material.smoothness = 1.0f;
    sphere1.material.specular_probability = 1.0f;

    Sphere sphere3;
    sphere3.centre = glm::vec3(-0.8f, 0.0f, -1.5f);
    sphere3.radius = 0.25f;
    sphere3.material.colour = glm::vec3(0.25f, 0.85f, 0.25f); 
    sphere3.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere3.material.emission_strength = 0.0f;
    sphere1.material.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere1.material.smoothness = 1.0f;
    sphere1.material.specular_probability = 1.0f;

    Sphere sphere4;
    sphere4.centre = glm::vec3(0.9f, 0.0f, -1.5f);
    sphere4.radius = 0.4f;
    sphere4.material.colour = glm::vec3(0.9f, 0.8f, 0.0f); // Yellow colour
    sphere4.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere4.material.emission_strength = 0.0f;
    sphere1.material.specular_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere1.material.smoothness = 1.0f;
    sphere1.material.specular_probability = 1.0f;

    spheres[0] = sphere1;
    spheres[1] = sphere2;
    spheres[2] = sphere3;
    spheres[3] = sphere4;

    SceneData sceneData;
    sceneData.objects = spheres;
    sceneData.n_objects = 4;
    sceneData.size = sceneData.n_objects * sizeof(Sphere);

	return sceneData;
}
