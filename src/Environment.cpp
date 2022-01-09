#include "Environment.h"
#include <iostream>
Environment::Environment()
{

};

Environment::~Environment()
{

};

void Environment::addWalls(Wall wall)
{
	walls.push_back(wall);
};

void Environment::updateMinMax()
{
	float offset = 7.0f;
	for (int i = 0; i < walls.size(); i++)
	{
		// Point 1
		if (walls[i].point1.x + offset < minX)
			minX = walls[i].point1.x + offset;

		if (walls[i].point1.x - offset > maxX)
			maxX = walls[i].point1.x - offset;

		if (walls[i].point1.y + offset < minY)
			minY = walls[i].point1.y + offset;

		// Ignore maxY because we don't need it in our setup
		if (walls[i].point1.y > maxY)
			maxY = walls[i].point1.y;

		// Point 2
		if (walls[i].point2.x + offset < minX)
			minX = walls[i].point2.x + offset;

		if (walls[i].point2.x - offset > maxX)
			maxX = walls[i].point2.x - offset;

		if (walls[i].point2.y + offset < minY)
			minY = walls[i].point2.y + offset;

		// Ignore maxY because we don't need it in our setup
		if (walls[i].point2.y > maxY)
			maxY = walls[i].point2.y;
	}
};

void Environment::Render()
{
	for (int i = 0; i < walls.size(); i++)
	{
		walls[i].Render();
	}
};