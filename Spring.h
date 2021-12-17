#pragma once
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Point
{
public:
	Point(glm::vec3 pos_);
	void ApplyForce(float dt);
	float k;
	glm::vec3 v;
	glm::vec3 pos;
	float mass;
	glm::vec3 F;
};

class Spring
{
	bool is_p1_fixed;
	float c1;
	float neutral_length;
public:
	Spring(std::shared_ptr<Point> p1_, std::shared_ptr<Point> p2_, bool p1_fixed_, float c1_);
	void RecalcSpring(float dt);
	std::shared_ptr<Point> p1;
	std::shared_ptr<Point> p2;
};