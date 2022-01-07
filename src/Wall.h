#pragma once
#include "glm/glm.hpp"
#include <vector>

class Wall
{
public:
	glm::vec2 point1;
	glm::vec2 point2;
	float length;

public:
	Wall();
	Wall(glm::vec2 point1, glm::vec2 point2);
	~Wall();

	void Render();
};