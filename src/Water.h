#pragma once
#include "WaterParticle.h"
#include "Environment.h"
#include <vector>
class Water
{
public:
	int particleNumber;
	int screenWidth;
	int screenHeight;
	std::vector<WaterParticle> waterParticles;

	float particlesWidthIncrement = 0.006;
	float particlesHeightIncrement = 0.008;

	float restDensity = 0.001f;
	float gasConstant = 1.0f;
	float miu = 0.1f;
	float tensionCoef = 5.0f;

	float radius = 10.0f;

public:
	Water();
	Water(int particleNumber, int screenWidth, int screenHeight);
	~Water();

	float getPoly6Kernal(float radius, glm::vec2 positionDifference);
	float getPoly6GradKernal(float radius, glm::vec2 positionDifference);
	float getPoly6LaplacianKernal(float radius, glm::vec2 positionDifference);


	float getSpikyGradKernal(float radius, glm::vec2 positionDifference);
	float getViscosityKernal(float radius, glm::vec2 positionDifference);

	void findNeighbourParticles();
	void findDensityAndPressure();
	void findColorField();

	void updateParticleNumber(int particleNumber);
	void updateVariables();
	void checkCollision(Environment environment);
	void update(float deltaTime);

	void updateAcceleration();
	void updateParticleForce();
	void updateParticlePosition(float deltaTime);


	void Render();
};

