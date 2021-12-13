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
	BezierPoints(Shader sh, std::vector<glm::vec3>& pt);

	void DrawObject(glm::mat4 mvp) override;

	void UpdatePoints(std::vector<glm::vec3>& pt);
private:

	void update_object() override;

	std::vector<glm::vec3> bezier_points;

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

	std::vector<glm::vec3> frame_points;
	std::vector<glm::vec3> bezier_points;
	std::vector<glm::vec3> constraint_box_points;

	glm::vec4 color = { 1,0.3,0,1 };

	bool draw_frame = true;
	bool draw_bezier_points = true;

	Shader shader_bez;

	std::vector<float> points;
	std::vector<unsigned int> quads;

	std::unique_ptr<ConstraintBox> constraint_box;
	std::unique_ptr<BezierPoints> bezier_ppoints;
};
