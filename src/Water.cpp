#include "Water.h"
#include <iostream>

#define PI 3.1415926f

Water::Water()
{
	particleNumber = 1;
    screenWidth = 0;
    screenHeight = 0;
}

Water::Water(int particleNumber, int screenWidth, int screenHeight) : particleNumber(particleNumber),
                                                                      screenWidth(screenWidth),
                                                                      screenHeight(screenHeight)
{

};

Water::~Water()
{

};

float Water::getPoly6Kernal(float radius, glm::vec2 positionDifference)
{
    float r = glm::length(positionDifference);
    if (r >= 0 && r <= radius)
    {
        float diff = (radius * radius) - (r * r);
        
        float returnVal = (315 / (64 * PI * std::pow(radius, 9))) * (diff * diff * diff);

        return returnVal;
    }

    return 0;
};

// A derivative of the poly 6 kernal
float Water::getPoly6GradKernal(float radius, glm::vec2 positionDifference)
{
    float r = glm::length(positionDifference);
    if (r >= 0 && r <= radius)
    {
        float diff = (radius * radius) - (r * r);

        float returnVal = (945 / (64 * PI * std::pow(radius, 9))) * (diff * diff);

        return returnVal;
    }

    return 0;
};

// A laplacian of the poly 6 kernal
float Water::getPoly6LaplacianKernal(float radius, glm::vec2 positionDifference)
{
    float r = glm::length(positionDifference);
    if (r >= 0 && r <= radius)
    {
        float diff = (radius * radius) - (r * r);

        float returnVal = (1890 / (64 * PI * std::pow(radius, 9))) * (diff);

        return returnVal;
    }

    return 0;
};

float Water::getSpikyGradKernal(float radius, glm::vec2 positionDifference)
{
    float r = glm::length(positionDifference);
    if (r >= 0 && r <= radius)
    {
        float diff = (radius - r);

        float returnVal = (45 / (PI * std::pow(radius, 6))) * (diff * diff);

        return returnVal;
    }

    return 0;
};

float Water::getViscosityKernal(float radius, glm::vec2 positionDifference)
{
    float r = glm::length(positionDifference);
    if (r >= 0 && r <= radius)
    {
        float diffa = -(std::pow(r, 3) / (2 * std::pow(radius, 3)));
        float diffb = (r * r) / (radius * radius);
        float diffc = radius / (2 * r);

        //float returnVal = 
        //    (15 / (2 * PI * positionDifference * positionDifference * positionDifference)) * (diffa + diffb + diffc - 1);

        float diff = radius - r;
        float returnVal = (45 / (PI * std::pow(radius, 6))) * diff;

        return returnVal;
    }

    return 0;
};

// Find neighbour particles within a certain radius
void Water::findNeighbourParticles()
{
    for (int i = 0; i < waterParticles.size(); i++)
    {
        // Resetting the neightbour list
        waterParticles[i].neighbours.clear();
        int counter = 0;
        for (int j = 0; j < waterParticles.size(); j++)
        {
            // Prevent detecting itself
            if (i != j)
            {
                // Assuming every particle is a ball
                // And its position is its the cetner
                // So we multiple radius by 2
                if (glm::distance(waterParticles[i].position, waterParticles[j].position) < (radius))
                {
                    waterParticles[i].setNeighbourParticles(j);
                }
            }
        }
    }
};

void Water::findDensityAndPressure()
{
    for (int i = 0; i < waterParticles.size(); i++)
    {
        float density = 0.0f;
        for (int j = 0; j < waterParticles[i].neighbours.size(); j++)
        {
            // According to Müller's paper
            // density is roughly the sum of all neighbour's mass 
            // multiply with the smoothing kernal
            int neighbourIndex = waterParticles[i].neighbours[j];
            float otherMass = waterParticles[neighbourIndex].mass;
            glm::vec2 diff = waterParticles[i].position - waterParticles[neighbourIndex].position;
            density += otherMass * getPoly6Kernal(radius, diff);
        }
        waterParticles[i].density = std::max(density, restDensity);
        // Ideal gas state, equation 12 from the paper
        waterParticles[i].pressure = gasConstant * (waterParticles[i].density - restDensity);
    }
};

void Water::findColorField()
{
    // Color Field
    // Gradient
    // Surface Normal
    for (int i = 0; i < waterParticles.size(); i++)
    {
        float colorField = 0.0f;
        float colorFieldGradient = 0.0f;
        float colorFieldLaplacian = 0.0f;
        for (int j = 0; j < waterParticles[i].neighbours.size(); j++)
        {
            int neighbourIndex = waterParticles[i].neighbours[j];
            float otherMass = waterParticles[neighbourIndex].mass;
            float otherDensity = waterParticles[neighbourIndex].density;
            glm::vec2 diff = waterParticles[i].position - waterParticles[neighbourIndex].position;
            colorField += (otherMass * (1 / otherDensity)) * getPoly6Kernal(radius, diff);
            colorFieldGradient += (otherMass * (1 / otherDensity)) * getPoly6GradKernal(radius, diff);
            colorFieldLaplacian += (otherMass * (1 / otherDensity)) * getPoly6LaplacianKernal(radius, diff);
        }
        waterParticles[i].colorField = colorField;
        waterParticles[i].colorFieldGrad = colorFieldGradient;
        waterParticles[i].colorFieldLaplacian = colorFieldLaplacian;
    }
};

void Water::updateParticleNumber(int particleNumber)
{
    this->particleNumber = particleNumber;
};

void Water::updateVariables()
{
    waterParticles.clear();
    float area = 2 * 3;
    // Water particles per 1m
    float particlesPerMeter = std::round((float)particleNumber / area);
    float widthDistance = 0.3125 - 0.1875;
    float heightDistance = 0.8889 - 0.5555;

    for (float height = 0.5555; height <= 0.8889; height += particlesHeightIncrement)
    {
        for (float width = 0.1875; width <= 0.3125; width += particlesWidthIncrement)
        {
            // Safe Guard to prevent creating more particles than we need 
            if (waterParticles.size() >= particleNumber)
            {
                break;
            }
            WaterParticle waterParticle(glm::vec2(width * screenWidth, height * screenHeight));
            waterParticles.push_back(waterParticle);
        }
    }
};

void Water::checkCollision(Environment environment)
{
    // Box collision
    for (int i = 0; i < waterParticles.size(); i++)
    {
        waterParticles[i].checkCollision(environment);
    }

    // Water Particle collision
    for (int i = 0; i < waterParticles.size(); i++)
    {
        for (int j = 0; j < waterParticles.size(); j++)
        {
            if(i != j)
                waterParticles[i].checkCollisionWithOtherParticles(waterParticles[j], 2);
        }
    }
};

void Water::update(float deltaTime)
{
    for (int i = 0; i < waterParticles.size(); i++)
    {
        waterParticles[i].update(deltaTime);
    }
};

void Water::updateAcceleration()
{
    for (int i = 0; i < waterParticles.size(); i++)
    {
        waterParticles[i].updateAcceleration();
    }
};

void Water::updateParticleForce()
{
    for (int i = 0; i < waterParticles.size(); i++)
    {
        glm::vec2 fieldForce(0.0, 0.0);
        for (int j = 0; j < waterParticles[i].neighbours.size(); j++)
        {
            // Force due to pressure
            // Using equation 10 from the paper
            int neighbourIndex = waterParticles[i].neighbours[j];

            float otherMass = waterParticles[neighbourIndex].mass;
            float otherDensity = waterParticles[neighbourIndex].density;
            float pressureSum = waterParticles[i].pressure + waterParticles[neighbourIndex].pressure;
            glm::vec2 positionDifference = waterParticles[i].position - waterParticles[neighbourIndex].position;
            
            // Use spiky kernal for pressure
            fieldForce -= (otherMass * (pressureSum / (2 * otherDensity)) * getSpikyGradKernal(radius, positionDifference));


            // Force due to Viscosity
            // Using equation 14 from the paper
            // And viscosity kernal
            glm::vec2 velocityDifference = waterParticles[neighbourIndex].velocity - waterParticles[i].velocity;
            fieldForce += miu * (otherMass * (velocityDifference / otherDensity) * getViscosityKernal(radius, positionDifference));


            // Surface tension
            // Find surface normal first
            float threshold = 0.00000000001f;
            // Make sure we're not dividing something by 0
            float n = std::max(waterParticles[i].colorFieldGrad, threshold);
            float surfaceNormal = -waterParticles[i].colorFieldLaplacian / std::abs(n);

            fieldForce += tensionCoef * surfaceNormal * n;
        }

        // Acceleration due to gravity
        fieldForce += glm::vec2(0, -9.81);

        waterParticles[i].fieldForce = fieldForce;
    }
};

void Water::updateParticlePosition(float deltaTime)
{
    for (int i = 0; i < waterParticles.size(); i++)
    {
        // Update using Leapfrog scheme

        // Velocity half step
        glm::vec2 velocityHalfStep = waterParticles[i].velocity 
            + (waterParticles[i].acceleration * (deltaTime / 2));

        waterParticles[i].position = waterParticles[i].position + (velocityHalfStep * deltaTime);
        waterParticles[i].velocity = velocityHalfStep + (waterParticles[i].acceleration * (deltaTime / 2));
    }
};

void Water::Render()
{
    for (int i = 0; i < waterParticles.size(); i++)
    {
        waterParticles[i].Render();
    }
};