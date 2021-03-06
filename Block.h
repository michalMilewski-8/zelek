#pragma once
#include "Object.h"
#include "Line.h"
#include "Spring.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#define _USE_MATH_DEFINES
#include <cmath>

class ConstraintBox : public Object {
public:
	ConstraintBox(Shader sh, std::shared_ptr<Parameters> params_, std::vector<glm::vec3>& pt);

	void DrawObject(glm::mat4 mvp) override;

private:

	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class BezierPoints : public Object {
public:
	BezierPoints(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& pt);

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
	BezierSprings(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& pt);

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
	FrameSprings(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& bezier_points, std::vector<std::shared_ptr<Point>>& frame_points);

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
	BezierBox(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& bezier_pts);

	void DrawObject(glm::mat4 mvp) override;

	void UpdatePoints();
	glm::mat4 inv_view_mat;
private:

	void update_object() override;

	std::vector<std::shared_ptr<Point>> bezier_points;

	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class BezierKaczor : public Object {
public:
	BezierKaczor(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& bezier_pts);

	void DrawObject(glm::mat4 mvp) override;

	void UpdatePoints();
	glm::mat4 inv_view_mat;
private:

	void update_object() override;

	std::vector<std::shared_ptr<Point>> bezier_points;

	std::vector<glm::vec3> bezier_points_vec;

	unsigned int texture;

	std::vector<float> points;
	std::vector<unsigned int> quads;
};

class Block :
	public Object
{
public:
	Block(Shader sh, std::shared_ptr<Parameters> params_);

	void DrawObject(glm::mat4 mvp) override;

	void SetViewPos(glm::vec3 view_pos);
	void SetViewMat(glm::mat4 view_mat);
	void Update() override;

	void DrawFrame(glm::mat4 mvp);
	void CalculateFrame(float T);

	void MoveFrame(glm::vec3);
	void RotateFrame(glm::quat);

private:
	void create_block_points();
	void update_object() override;
	void create_stating_points();

	glm::vec3 view_pos;
	glm::mat4 inv_view_mat;

	unsigned int texture;

	std::vector<std::shared_ptr<Point>> frame_points;
	std::vector<std::shared_ptr<Point>> bezier_points;
	std::vector<glm::vec3> constraint_box_points;

	glm::vec4 color = { 1,0.3,0,1 };
	glm::vec3 center = { 0,0,0 };

	Shader shader_bez;

	std::vector<float> points;
	std::vector<unsigned int> quads;

	std::unique_ptr<ConstraintBox> constraint_box;
	std::unique_ptr<BezierPoints> bezier_vertices;
	std::unique_ptr<BezierSprings> bezier_springs;
	std::unique_ptr<FrameSprings> frame_springs;
	std::unique_ptr<BezierBox> bezier_box;
	std::unique_ptr<BezierKaczor> bezier_kaczor;
};
