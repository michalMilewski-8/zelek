#include "Spring.h"
Point::Point(std::shared_ptr<Parameters> params_, glm::vec3 pos_) : params(params_), pos(pos_)
{
	v = { 0.0f, 0.0f, 0.0f };
	mass = 1.0f;
	F = { 0.0f,0.0f,0.0f };
}

void Point::ApplyForce(float dt)
{
	F -= dt * params->k * v;
	auto v_new = (F / mass) + v;
	auto pos_new = dt * v_new + pos;
	pos = pos_new;
	v = v_new;
	F = { 0.0f,0.0f,0.0f };
}

Spring::Spring(std::shared_ptr<Parameters> params_, std::shared_ptr<Point> p1_, std::shared_ptr<Point> p2_, bool p1_fixed_)
	:params(params_), p1(p1_), p2(p2_), p1_fixed(p1_fixed_)
{
	neutral_length = glm::distance(p1->pos, p2->pos);
}

void Spring::RecalcSpring(float dt)
{
	auto l = glm::distance(p1->pos, p2->pos) - neutral_length;
	auto dir = glm::normalize(p2->pos - p1->pos);
	if (l == 0)
		dir = { 0.0f,0.0f,0.0f };

	float c;
	if (!p1_fixed)
		c = params->c1;
	else
		c = params->c2;

	auto f_val = dt * c * l;
	if (!p1_fixed) {
		p1->F += f_val * dir;
	}
	p2->F += f_val * (-dir);
}
