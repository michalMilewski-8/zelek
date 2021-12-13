#include "Line.h"

unsigned int Line::counter = 1;

Line::Line(Shader& sh) : Object(sh, 7)
{
	sprintf_s(name, 512, ("Line " + std::to_string(counter)).c_str());
	constname = "Line " + std::to_string(counter);
	counter++;
	this->color = { 0.0f,1.0f,0.0f,1.0f };
	update_object();
}

void Line::DrawObject(glm::mat4 mvp_)
{
	moved = false;
	if (points_.size() < 2) return;

	if (points_.size() > max_points) need_update = true;

	if (need_update) {
		update_object();
		need_update = false;
	}
	mvp = mvp_ * translate * rotate;
	shader.use();
	int projectionLoc = glGetUniformLocation(shader.ID, "mvp");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(mvp));
	glLineWidth(width);
	glBindVertexArray(VAO);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_LINE_STRIP, lines.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Line::AddPoint(glm::vec3 point)
{
	points_.push_back(point);
	Update();
}

void Line::Update()
{
	need_update = true;
}

void Line::ClearPoints()
{
	points_.clear();
	Update();
}

void Line::Reverse()
{
	std::reverse(points_.begin(), points_.end());
}

void Line::update_object()
{
	while (points_.size() > max_points && points_.size() > 0) {
		points_.erase(points_.begin());
	}
	lines.clear();
	points_on_curve.clear();
	int k = 0;
	for (auto& vec : points_) {
		points_on_curve.push_back(vec.x);
		points_on_curve.push_back(vec.y);
		points_on_curve.push_back(vec.z);
		points_on_curve.push_back(color.r);
		points_on_curve.push_back(color.g);
		points_on_curve.push_back(color.b);
		points_on_curve.push_back(color.a);
		lines.push_back(k);
		k++;
	}


	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points_on_curve.size(), points_on_curve.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * lines.size(), lines.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}
