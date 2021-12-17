#pragma once
#include <memory>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Parameters.h"
class Point
{
	std::shared_ptr<Parameters> params;
public:
	Point(std::shared_ptr<Parameters> params_,glm::vec3 pos_);
	void ApplyForce(float dt);
	glm::vec3 v;
	glm::vec3 pos;
	float mass;
	glm::vec3 F;
};

class Spring
{
	bool p1_fixed;
	float neutral_length;
	std::shared_ptr<Parameters> params;
public:
	Spring(std::shared_ptr<Parameters> params_,std::shared_ptr<Point> p1_, std::shared_ptr<Point> p2_, bool p1_fixed_);
	void RecalcSpring(float dt);
	std::shared_ptr<Point> p1;
	std::shared_ptr<Point> p2;
};