#pragma once

#include <glm/glm.hpp>

struct Material
{
	glm::vec4 colour;
    glm::vec3 emission_colour;
    float emission_strength;
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

SceneData defaultScene() {
    Sphere* spheres = new Sphere[4];

    Sphere sphere1;
    sphere1.centre = glm::vec3(0.0f, -10.0f, 5.0f);
    sphere1.radius = 9.0f;
    sphere1.material.colour = glm::vec4(0.807f, 0.2588f, 0.2588f, 1.0f); // Red colour
    sphere1.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere1.material.emission_strength = 0.0f;

    Sphere sphere2;
    sphere2.centre = glm::vec3(-476.0f, 513.0f, 0.0f);
    sphere2.radius = 50.0f;
    sphere2.material.colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    sphere2.material.emission_colour = glm::vec3(1.0f, 0.9f, 1.0f);
    sphere2.material.emission_strength = 3.0f;

    Sphere sphere3;
    sphere3.centre = glm::vec3(0.3f, 2.0f, 10.0f);
    sphere3.radius = 0.7f;
    sphere3.material.colour = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f); // skin colour
    sphere3.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    sphere3.material.emission_strength = 0.0f;

    Sphere sphere4;
    sphere4.centre = glm::vec3(-3.0f, -0.9f, 5.0f);
    sphere4.radius = 0.8f;
    sphere4.material.colour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Yellow colour
    sphere4.material.emission_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    sphere4.material.emission_strength = 4.0f;

    //Sphere sphere1;
    //sphere1.centre = glm::vec3(-1.0f, 0.0f, -5.0f);
    //sphere1.radius = 1.0f;
    //sphere1.material.colour = glm::vec4(0.807f, 0.2588f, 0.2588f, 1.0f); // Red colour
    //sphere1.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    //sphere1.material.emission_strength = 0.0f;

    //Sphere sphere2;
    //sphere2.centre = glm::vec3(-0.5f, -0.25f, -5.0f);
    //sphere2.radius = 1.0f;
    //sphere2.material.colour = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    //sphere2.material.emission_colour = glm::vec3(1.0f, 0.9f, 1.0f);
    //sphere2.material.emission_strength = 3.0f;

    //Sphere sphere3;
    //sphere3.centre = glm::vec3(0.3f, 0.0f, -10.0f);
    //sphere3.radius = 0.7f;
    //sphere3.material.colour = glm::vec4(1.0f, 1.0f, 0.9f, 1.0f); // skin colour
    //sphere3.material.emission_colour = glm::vec3(0.0f, 0.0f, 0.0f);
    //sphere3.material.emission_strength = 0.0f;

    //Sphere sphere4;
    //sphere4.centre = glm::vec3(1.0f, -0.9f, -5.0f);
    //sphere4.radius = 3.0f;
    //sphere4.material.colour = glm::vec4(0.7f, 0.6f, 0.0f, 1.0f); // Yellow colour
    //sphere4.material.emission_colour = glm::vec3(1.0f, 1.0f, 1.0f);
    //sphere4.material.emission_strength = 4.0f;


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
