#include "GLFW/glfw3.h"
#include <iostream>

#include "WaterParticle.h"
WaterParticle::WaterParticle()
{
	position.x = 0.0f;
	position.y = 0.0f;

	force.x = 0;
	force.y = 0;

	velocity.x = 0;
	velocity.y = 0;
};

WaterParticle::WaterParticle(glm::vec2 position) : position(position) 
{
	force.x = 0;
	force.y = 0;

	velocity.x = 0;
	velocity.y = 0;
};

WaterParticle::~WaterParticle()
{
	
};

void WaterParticle::setNeighbourParticles(int otherIndex)
{
	neighbours.push_back(otherIndex);
};

void WaterParticle::setDensity(float d)
{

};

void WaterParticle::checkCollision(Environment environment)
{
	// Ignore the top collision because top is open
	bool collisionOnLeftWall = position.x < environment.minX && position.y < environment.maxY;
	bool collisionOnRightWall = position.x > environment.maxX && position.y < environment.maxY;
	bool collisionOnGround = position.y < environment.minY;

	bool collision = collisionOnLeftWall || collisionOnRightWall || collisionOnGround;

	if (collision)
	{
		//std::cout << "Collision!" << "\n";

		if (collisionOnGround)
		{
			float yDistance = std::abs(position.y - environment.minY);

			glm::vec2 closestPoint = glm::vec2(position.x, position.y + yDistance);

			pushPoint(closestPoint);
		}

		if (collisionOnLeftWall || collisionOnRightWall)
		{
			// Doing this is to prevent reductant code
			// As it would be either left wall or right wall
			float xDistance = position.x - (collisionOnLeftWall ? environment.minX : environment.maxX);

			glm::vec2 closestPoint = glm::vec2(position.x - xDistance, position.y);

			pushPoint(closestPoint);
		}
	}
};

void WaterParticle::checkCollisionWithOtherParticles(WaterParticle other, float radius)
{
	bool collision = glm::distance(position, other.position) < radius * 2;

	if (collision)
	{
		glm::vec2 delta = position - other.position;
		float d = glm::length(delta);

		glm::vec2 mtd = delta * (((radius + radius) - d) / d);

		float im1 = 1 / mass;
		float im2 = 1 / other.mass;

		position = position + (mtd * (im1 / (im1 + im2)));
		other.position = other.position - (mtd * (im2 / (im1 + im2)));

		glm::vec2 v = velocity - other.velocity;
		float vn = glm::dot(v, glm::normalize(mtd));

		if (vn > 0.0f) return;

		float i = (-(1.0f + restCoef) * vn) / (im1 + im2);
		glm::vec2 impulse = glm::normalize(mtd) * i;

		velocity = (velocity + (impulse * im1)) * restCoef;
		other.velocity = (other.velocity - (impulse * im2)) * restCoef;
	}
};

void WaterParticle::pushPoint(glm::vec2 closestPoint)
{
	glm::vec2 pushVector = closestPoint - position;

	position = closestPoint + (pushVector * restCoef);
	velocity = pushVector * restCoef;
};

void WaterParticle::update(float deltaTime)
{
	// Reset force to prevent it from accumulating
	force.x = 0;
	force.y = 0;

	force += acceleration;

	velocity += (force * deltaTime) / mass;
	position += velocity * deltaTime;
};

void WaterParticle::updateAcceleration()
{
	acceleration = fieldForce / density;
};

void WaterParticle::Render()
{
	glPointSize(5);
	glBegin(GL_POINTS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex2f(position.x, position.y);
	glEnd();
};