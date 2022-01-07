#pragma once
#include "Wall.h"
#include <vector>
#include <limits>

// Just a big Environment class containing all walls
// or things that can possibly have collision
class Environment
{
public:
	std::vector<Wall> walls;

	float minX = std::numeric_limits<float>::max();
	float minY = std::numeric_limits<float>::max();

	float maxX = std::numeric_limits<float>::lowest();
	float maxY = std::numeric_limits<float>::lowest();

public:
	Environment();
	~Environment();

	void addWalls(Wall wall);
	void updateMinMax();
	void Render();
};

