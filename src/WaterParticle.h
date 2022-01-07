#pragma once
#include "glm/glm.hpp"
#include <vector>

#include "Environment.h"
class WaterParticle
{
public:
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 force;
	glm::vec2 fieldForce;
	glm::vec2 acceleration;
	float mass = 1.0f;

	float restCoef = 1.0f;

	std::vector<int> neighbours;
	float density;
	float pressure;
	float colorField;
	float colorFieldGrad;
	float colorFieldLaplacian;

public:
	WaterParticle();
	WaterParticle(glm::vec2 position);
	~WaterParticle();

	void setNeighbourParticles(int otherIndex);
	void setDensity(float d);

	void checkCollision(Environment environment);
	void checkCollisionWithOtherParticles(WaterParticle other, float radius);
	void pushPoint(glm::vec2 closestPoint);
	void update(float deltaTime);

	void updateAcceleration();

	void Render();
};

