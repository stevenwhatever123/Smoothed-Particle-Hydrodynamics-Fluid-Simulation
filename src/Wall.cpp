#include "Wall.h"
#include <GLFW/glfw3.h>
Wall::Wall() 
{
	point1.x = 0.0f;
	point1.y = 0.0f;

	point2.x = 0.0f;
	point2.y = 0.0f;

	length = glm::distance(point1, point2);
};

Wall::Wall(glm::vec2 point1, glm::vec2 point2) : point1(point1), point2(point2)
{
	length = glm::distance(point1, point2);
};

Wall::~Wall()
{

};

void Wall::Render()
{
	glLineWidth(5.0);
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2f(point1.x, point1.y);
	glVertex2f(point2.x, point2.y);
	glEnd();
};
