#pragma once
#include "Object.h"
#include "Line.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>

class Point
{
public:
	Point(glm::vec3 pos_);
	void ApplyForce(float dt);
	glm::vec3 v;
	glm::vec3 pos;
	float mass;
	glm::vec3 F;
};

class Spring
{
	bool is_p1_fixed;
	float c1;
	float k;
	float neutral_length;
public:
	Spring(std::shared_ptr<Point> p1_, std::shared_ptr<Point> p2_, bool p1_fixed_);
	void RecalcSpring(float dt);
	std::shared_ptr<Point> p1;
	std::shared_ptr<Point> p2;
};


class ConstraintBox : public Object {
public:
	ConstraintBox(Shader sh, std::vector<glm::vec3>& pt);

	void DrawObject(glm::mat4 mvp) override;

private:

	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class BezierPoints : public Object {
public:
	BezierPoints(Shader sh, std::vector<std::shared_ptr<Point>>& pt);

	void DrawObject(glm::mat4 mvp) override;

	void UpdatePoints();
private:

	void update_object() override;

	std::vector<std::shared_ptr<Point>> bezier_points;

	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class BezierSprings : public Object {
public:
	BezierSprings(Shader sh, std::vector<std::shared_ptr<Point>>& pt);

	void DrawObject(glm::mat4 mvp) override;

	void UpdatePoints();
	void RecalcSprings(float dt);
private:

	void update_object() override;

	std::vector<std::shared_ptr<Spring>> springs;
	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class FrameSprings : public Object {
public:
	FrameSprings(Shader sh, std::vector<std::shared_ptr<Point>>& bezier_points, std::vector<std::shared_ptr<Point>>& frame_points);

	void DrawObject(glm::mat4 mvp) override;

	void UpdatePoints();
	void RecalcSprings(float dt);
private:

	void update_object() override;

	std::vector<std::shared_ptr<Spring>> springs;
	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class BezierBox : public Object {
public:
	BezierBox(Shader sh, std::vector<std::shared_ptr<Point>>& bezier_pts);

	void DrawObject(glm::mat4 mvp) override;

	void UpdatePoints();
private:

	void update_object() override;

	std::vector<std::shared_ptr<Point>> bezier_points;

	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class Block :
	public Object
{
public:
	Block(Shader sh);

	void DrawObject(glm::mat4 mvp) override;

	void SetViewPos(glm::vec3 view_pos);
	void Update() override;

	void DrawFrame(glm::mat4 mvp);
	void CalculateFrame(float T);

	void MoveFrame(glm::vec3);

private:
	void create_block_points();
	void update_object() override;
	void create_stating_points();

	glm::vec3 view_pos;

	unsigned int texture;

	std::vector<std::shared_ptr<Point>> frame_points;
	std::vector<std::shared_ptr<Point>> bezier_points;
	std::vector<glm::vec3> constraint_box_points;

	glm::vec4 color = { 1,0.3,0,1 };

	bool draw_frame = true;
	bool draw_bezier_points = true;
	bool draw_bezier_springs = true;
	bool draw_frame_springs = true;
	bool draw_bezier_box = true;

	Shader shader_bez;

	std::vector<float> points;
	std::vector<unsigned int> quads;

	std::unique_ptr<ConstraintBox> constraint_box;
	std::unique_ptr<BezierPoints> bezier_vertices;
	std::unique_ptr<BezierSprings> bezier_springs;
	std::unique_ptr<FrameSprings> frame_springs;
	std::unique_ptr<BezierBox> bezier_box;
};
