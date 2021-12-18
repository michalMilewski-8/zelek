#include "Spring.h"
Point::Point(std::shared_ptr<Parameters> params_, glm::vec3 pos_) : params(params_), pos(pos_)
{
	v = { 0.0f, 0.0f, 0.0f };
	mass = 1.0f;
	F = { 0.0f,0.0f,0.0f };
}

void Point::ApplyForce(float dt)
{
	float box_size = 4.0f;
	F -= dt * params->k * v;
	auto v_new = (F / mass) + v;
	glm::vec3 pos_new = pos;
	while (dt > 0.0f)
	{
		auto pos_t = dt * v_new + pos_new;

		bool zp = false, zm = false, xp = false, xm = false, yp = false, ym = false;
		if (pos_t.x > box_size) xp = true;
		if (pos_t.y > box_size) yp = true;
		if (pos_t.z > box_size) zp = true;

		if (pos_t.x < -box_size) xm = true;
		if (pos_t.y < -box_size) ym = true;
		if (pos_t.z < -box_size) zm = true;

		if (!zp && !zm && !yp && !ym && !xp && !xm)
		{
			pos_new = pos_t;
			break;
		}


		float zp_dt = 9999999.0f, zm_dt = 9999999.0f, xp_dt = 9999999.0f, xm_dt = 9999999.0f, yp_dt = 9999999.0f, ym_dt = 9999999.0f;
		if (xp) xp_dt = (box_size - pos_new.x) / v_new.x;
		if (yp) yp_dt = (box_size - pos_new.y) / v_new.y;
		if (zp) zp_dt = (box_size - pos_new.z) / v_new.z;

		if (xm) xm_dt = (-box_size - pos_new.x) / v_new.x;
		if (ym) ym_dt = (-box_size - pos_new.y) / v_new.y;
		if (zm) zm_dt = (-box_size - pos_new.z) / v_new.z;

		int switch_int = -1;
		float min = 9999999.0f;
		if (xp_dt < min) { min = xp_dt; switch_int = 1; }
		if (yp_dt < min) { min = yp_dt; switch_int = 2; }
		if (zp_dt < min) { min = zp_dt; switch_int = 3; }
		if (xm_dt < min) { min = xm_dt; switch_int = 4; }
		if (ym_dt < min) { min = ym_dt; switch_int = 5; }
		if (zm_dt < min) { min = zm_dt; switch_int = 6; }

		if (min < 0)
			throw;

		switch (switch_int)
		{
		case 1:
			dt -= xp_dt;
			pos_new = xp_dt * v_new + pos_new;
			if (params->collision_version)
				v_new = params->mi * glm::vec3(-v_new.x, v_new.y, v_new.z);
			else
				v_new = glm::vec3(params->mi * -v_new.x, v_new.y, v_new.z);
			break;
		case 2:
			dt -= yp_dt;
			pos_new = yp_dt * v_new + pos_new;
			if (params->collision_version)
				v_new = params->mi * glm::vec3(v_new.x, -v_new.y, v_new.z);
			else
				v_new = glm::vec3(v_new.x, params->mi * -v_new.y, v_new.z);
			break;
		case 3:
			dt -= zp_dt;
			pos_new = zp_dt * v_new + pos_new;
			if (params->collision_version)
				v_new = params->mi * glm::vec3(v_new.x, v_new.y, -v_new.z);
			else
				v_new = glm::vec3(v_new.x, v_new.y, params->mi * -v_new.z);
			break;
		case 4:
			dt -= xm_dt;
			pos_new = xm_dt * v_new + pos_new;
			if (params->collision_version)
				v_new = params->mi * glm::vec3(-v_new.x, v_new.y, v_new.z);
			else
				v_new = glm::vec3(params->mi * -v_new.x, v_new.y, v_new.z);
			break;
		case 5:
			dt -= ym_dt;
			pos_new = ym_dt * v_new + pos_new;
			if (params->collision_version)
				v_new = params->mi * glm::vec3(v_new.x, -v_new.y, v_new.z);
			else
				v_new = glm::vec3(v_new.x, params->mi * -v_new.y, v_new.z);
			break;
		case 6:
			dt -= zm_dt;
			pos_new = zm_dt * v_new + pos_new;
			if (params->collision_version)
				v_new = params->mi * glm::vec3(v_new.x, v_new.y, -v_new.z);
			else
				v_new = glm::vec3(v_new.x, v_new.y, params->mi * -v_new.z);
			break;
		default:
			throw;
		}

	}


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
