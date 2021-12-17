#include "Spring.h"
Point::Point(glm::vec3 pos_) : pos(pos_)
{
	k = 1.0f;
	v = { 0.0f, 0.0f, 0.0f };
	mass = 1.0f;
	F = { 0.0f,0.0f,0.0f };
}

void Point::ApplyForce(float dt)
{
	F -= dt * k * v;
	auto v_new = (F / mass) + v;
	auto pos_new = dt * v_new + pos;
	pos = pos_new;
	v = v_new;
	F = { 0.0f,0.0f,0.0f };
}

Spring::Spring(std::shared_ptr<Point> p1_, std::shared_ptr<Point> p2_, bool p1_fixed_, float c1_) 
	: p1(p1_), p2(p2_), is_p1_fixed(p1_fixed_), c1(c1_)
{
	neutral_length = glm::distance(p1->pos, p2->pos);
}

void Spring::RecalcSpring(float dt)
{
	auto l = glm::distance(p1->pos, p2->pos) - neutral_length;
	auto dir = glm::normalize(p2->pos - p1->pos);
	if (l == 0)
		dir = { 0.0f,0.0f,0.0f };
	auto f_val = dt * c1 * l;
	if (!is_p1_fixed) {
		p1->F += f_val * dir;
	}
	p2->F += f_val * (-dir);
}
