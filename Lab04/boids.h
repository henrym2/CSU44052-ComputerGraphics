#pragma once
#include "Mesh.h"
#include <vector> // STL dynamic memory.
#include "Model.h"
#include "vec3.hpp"


struct Boid {
	vec3 location;
	vec3 Velocity;
	Model model;
	Mesh mesh;
	int id;
};

class Boids {
public:
	Boids();
	Boids(std::vector<Mesh> mesh, int num, vec3 areaMax, vec3 areaMin);
	void generateObjectBufferMesh(Shader shaderProgram);
	void drawAll(Shader shader);
	void updateAll();

private:
	void avoidOthers(Boid &boid);
	float distance(Boid boid1, Boid boid2);
	void keepInBounds(Boid &boid);
	void flyTowardsCenter(Boid &boid);
	void matchVelocity(Boid &boid);
	void limitSpeed(Boid &boid);
	std::vector<Mesh> boidMesh;
	Model boidModel;
	int num_boid;
	vec3 boidAreaMax;
	vec3 boidAreaMin;
	vec3* boidLocations;
	vec3* boidVelocity;
	Boid* boids;
};