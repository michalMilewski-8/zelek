#include "Block.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Block::SetViewPos(glm::vec3 view_pos)
{
	this->view_pos = view_pos;
}

void Block::Update()
{
	need_update = true;
}

void Block::CalculateFrame(float T)
{
	bezier_vertices->UpdatePoints(bezier_points);
	bezier_springs->UpdatePoints(bezier_points);
	frame_springs->UpdatePoints(bezier_points, frame_points);
}

void Block::MoveFrame(glm::vec3 mvm)
{
	for (auto& pt : frame_points) {
		pt += mvm;
	}
	frame_springs->UpdatePoints(bezier_points, frame_points);
	Update();
}

void Block::DrawFrame(glm::mat4 mvp)
{
	DrawObject(mvp);
}

void Block::create_block_points()
{
	for (const auto& pt : frame_points) {
		points.push_back(pt.x);
		points.push_back(pt.y);
		points.push_back(pt.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);
	}

	quads.push_back(0);
	quads.push_back(1);
	quads.push_back(1);
	quads.push_back(2);
	quads.push_back(2);
	quads.push_back(3);
	quads.push_back(3);
	quads.push_back(0);

	quads.push_back(4);
	quads.push_back(5);
	quads.push_back(5);
	quads.push_back(6);
	quads.push_back(6);
	quads.push_back(7);
	quads.push_back(7);
	quads.push_back(4);


	quads.push_back(0);
	quads.push_back(4);

	quads.push_back(1);
	quads.push_back(5);

	quads.push_back(2);
	quads.push_back(6);

	quads.push_back(3);
	quads.push_back(7);
}

void Block::update_object()
{
	points.clear();
	quads.clear();

	create_block_points();

	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Block::create_stating_points()
{
	frame_points = {
		{-1,-0,-0.5f },
		{1,-0,-0.5f },
		{1,-0,0.5f },
		{-1,-0,0.5f },

		{-1,1,-0.5f },
		{1,1,-0.5f },
		{1,1,0.5f },
		{-1,1,0.5f }
	};

	constraint_box_points = {
	{-5,-0,-5 },
	{5,-0,-5 },
	{5,-0,5 },
	{-5,-0,5 },

	{-5,5,-5 },
	{5,5,-5 },
	{5,5,5 },
	{-5,5,5 }
	};

	glm::vec3 curr_pos;
	double mv = 2.0 / 3.0;
	double mv2 = 1.0 / 3.0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				curr_pos = { -1.0f + i * mv ,0.0f + j * mv2,-0.5f + k * mv2 };
				bezier_points.push_back(curr_pos);
			}
		}
	}
}

Block::Block(Shader sh) :
	Object(sh, 7)
{
	shader = sh;

	create_stating_points();

	constraint_box = std::make_unique<ConstraintBox>(sh, constraint_box_points);
	bezier_vertices = std::make_unique<BezierPoints>(sh, bezier_points);
	bezier_springs = std::make_unique<BezierSprings>(sh, bezier_points);
	frame_springs = std::make_unique<FrameSprings>(sh, bezier_points, frame_points);
	bezier_box = std::make_unique<BezierBox>(sh, bezier_points);

	update_object();
}

void Block::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}
	if (draw_frame) {
		Object::DrawObject(mvp);
		glm::mat4 model = translate * rotate * resize;
		glm::mat4 vp = mvp;
		shader.use();

		glBindVertexArray(VAO);

		glm::mat4 trmodel = glm::transpose(glm::inverse(model));
		int projectionLoc = glGetUniformLocation(shader.ID, "model");
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

		int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
		glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

		int mvLoc = glGetUniformLocation(shader.ID, "vp");
		glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

		int viewPos = glGetUniformLocation(shader.ID, "viewPos");
		glUniform3fv(viewPos, 1, &view_pos[0]);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		//glDrawElements(GL_TRIANGLES, quads.size(), GL_UNSIGNED_INT, 0);

		glLineWidth(1.0f);
		glDrawElements(GL_LINES, quads.size(), GL_UNSIGNED_INT, 0);

		glPointSize(4.0f);
		glDrawElements(GL_POINTS, quads.size(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	constraint_box->DrawObject(mvp);
	if (draw_bezier_points)
		bezier_vertices->DrawObject(mvp);
	if (draw_bezier_springs)
		bezier_springs->DrawObject(mvp);
	if (draw_frame_springs)
		frame_springs->DrawObject(mvp);
	if (draw_bezier_box)
		bezier_box->DrawObject(mvp);
}

ConstraintBox::ConstraintBox(Shader sh, std::vector<glm::vec3>& frame_points) :
	Object(sh, 7)
{
	color = { 0.2,0.5,0.9,1.0 };

	for (const auto& pt : frame_points) {
		points.push_back(pt.x);
		points.push_back(pt.y);
		points.push_back(pt.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);
	}

	quads.push_back(0);
	quads.push_back(1);
	quads.push_back(1);
	quads.push_back(2);
	quads.push_back(2);
	quads.push_back(3);
	quads.push_back(3);
	quads.push_back(0);

	quads.push_back(4);
	quads.push_back(5);
	quads.push_back(5);
	quads.push_back(6);
	quads.push_back(6);
	quads.push_back(7);
	quads.push_back(7);
	quads.push_back(4);


	quads.push_back(0);
	quads.push_back(4);

	quads.push_back(1);
	quads.push_back(5);

	quads.push_back(2);
	quads.push_back(6);

	quads.push_back(3);
	quads.push_back(7);
	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void ConstraintBox::DrawObject(glm::mat4 mvp)
{
	Object::DrawObject(mvp);
	glm::mat4 model = translate * rotate * resize;
	glm::mat4 vp = mvp;
	shader.use();

	glBindVertexArray(VAO);

	glm::mat4 trmodel = glm::transpose(glm::inverse(model));
	int projectionLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	int mvLoc = glGetUniformLocation(shader.ID, "vp");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawElements(GL_TRIANGLES, quads.size(), GL_UNSIGNED_INT, 0);

	glLineWidth(1.0f);
	glDrawElements(GL_LINES, quads.size(), GL_UNSIGNED_INT, 0);

	glPointSize(4.0f);
	glDrawElements(GL_POINTS, quads.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

BezierPoints::BezierPoints(Shader sh, std::vector<glm::vec3>& frame_points) :
	Object(sh, 7)
{
	//shader = Shader("shader.vert", "shader.frag");
	color = { 0.9,0.9,0,1.0 };
	bezier_points = {};
	points = {};
	quads = {};
	for (const auto& pt : frame_points) {
		bezier_points.push_back(pt);
	}
	update_object();
}

void BezierPoints::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);
	glm::mat4 model = translate * rotate * resize;
	glm::mat4 vp = mvp;
	shader.use();

	glBindVertexArray(VAO);

	glm::mat4 trmodel = glm::transpose(glm::inverse(model));
	int projectionLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	int mvLoc = glGetUniformLocation(shader.ID, "vp");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawElements(GL_TRIANGLES, quads.size(), GL_UNSIGNED_INT, 0);
	glPointSize(5.0f);
	glDrawElements(GL_POINTS, quads.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BezierPoints::UpdatePoints(std::vector<glm::vec3>& pt_)
{
	bezier_points.clear();
	for (const auto& pt : pt_) {
		bezier_points.push_back(pt);
	}
	need_update = true;
}

void BezierPoints::update_object()
{
	quads.clear();
	points.clear();
	int i = 0;
	for (const auto& pt : bezier_points) {
		points.push_back(pt.x);
		points.push_back(pt.y);
		points.push_back(pt.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);
		quads.push_back(i);
		i++;
	}

	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

BezierSprings::BezierSprings(Shader sh, std::vector<glm::vec3>& frame_points) :
	Object(sh, 7)
{
	//shader = Shader("shader.vert", "shader.frag");
	color = { 0.0,0.8,0.8,1.0 };
	bezier_points = {};
	points = {};
	quads = {};
	for (const auto& pt : frame_points) {
		bezier_points.push_back(pt);
	}
	update_object();
}

void BezierSprings::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);
	glm::mat4 model = translate * rotate * resize;
	glm::mat4 vp = mvp;
	shader.use();

	glBindVertexArray(VAO);

	glm::mat4 trmodel = glm::transpose(glm::inverse(model));
	int projectionLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	int mvLoc = glGetUniformLocation(shader.ID, "vp");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawElements(GL_TRIANGLES, quads.size(), GL_UNSIGNED_INT, 0);
	glLineWidth(2.0f);
	glDrawElements(GL_LINES, quads.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BezierSprings::UpdatePoints(std::vector<glm::vec3>& pt_)
{
	bezier_points.clear();
	for (const auto& pt : pt_) {
		bezier_points.push_back(pt);
	}
	need_update = true;
}

void BezierSprings::update_object()
{
	quads.clear();
	points.clear();
	for (const auto& pt : bezier_points) {
		points.push_back(pt.x);
		points.push_back(pt.y);
		points.push_back(pt.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				if (k < 3) {
					quads.push_back(i * 16 + j * 4 + k);
					quads.push_back(i * 16 + j * 4 + k + 1);
				}

				if (j < 3) {
					quads.push_back(i * 16 + j * 4 + k);
					quads.push_back(i * 16 + (j + 1) * 4 + k);
				}

				if (i < 3) {
					quads.push_back(i * 16 + j * 4 + k);
					quads.push_back((i + 1) * 16 + j * 4 + k);
				}
			}
		}
	}

	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

FrameSprings::FrameSprings(Shader sh, std::vector<glm::vec3>& bezier_pts, std::vector<glm::vec3>& frame_pts) :
	Object(sh, 7)
{
	//shader = Shader("shader.vert", "shader.frag");
	color = { 0.5,0,0.2,1.0 };
	bezier_points = {};
	frame_points = {};
	points = {};
	quads = {};
	for (const auto& pt : frame_pts) {
		frame_points.push_back(pt);
	}

	for (const auto& pt : bezier_pts) {
		bezier_points.push_back(pt);
	}
	update_object();
}

void FrameSprings::UpdatePoints(std::vector<glm::vec3>& bezier_pts, std::vector<glm::vec3>& frame_pts)
{
	bezier_points.clear();
	frame_points.clear();

	for (const auto& pt : frame_pts) {
		frame_points.push_back(pt);
	}

	for (const auto& pt : bezier_pts) {
		bezier_points.push_back(pt);
	}

	need_update = true;
}

void FrameSprings::update_object()
{
	quads.clear();
	points.clear();

	std::vector<glm::vec3> pts = {};

	pts.push_back(frame_points[0]);
	pts.push_back(bezier_points[0]);

	pts.push_back(frame_points[1]);
	pts.push_back(bezier_points[48]);

	pts.push_back(frame_points[2]);
	pts.push_back(bezier_points[51]);

	pts.push_back(frame_points[3]);
	pts.push_back(bezier_points[3]);


	pts.push_back(frame_points[4]);
	pts.push_back(bezier_points[12]);

	pts.push_back(frame_points[5]);
	pts.push_back(bezier_points[60]);

	pts.push_back(frame_points[6]);
	pts.push_back(bezier_points[63]);

	pts.push_back(frame_points[7]);
	pts.push_back(bezier_points[15]);

	for (const auto& pt : pts) {
		points.push_back(pt.x);
		points.push_back(pt.y);
		points.push_back(pt.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);
	}


	for (int i = 0; i < 16; i++) {
		quads.push_back(i);
	}

	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void FrameSprings::DrawObject(glm::mat4 mvp)
{
	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);
	glm::mat4 model = translate * rotate * resize;
	glm::mat4 vp = mvp;
	shader.use();

	glBindVertexArray(VAO);

	glm::mat4 trmodel = glm::transpose(glm::inverse(model));
	int projectionLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	int mvLoc = glGetUniformLocation(shader.ID, "vp");
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

	//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawElements(GL_TRIANGLES, quads.size(), GL_UNSIGNED_INT, 0);
	glLineWidth(3.0f);
	glDrawElements(GL_LINES, quads.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

BezierBox::BezierBox(Shader sh, std::vector<glm::vec3>& bezier_pts) :
	Object(sh, 3)
{
	//shader = Shader("shader.vert", "shader.frag");
	shader = Shader("tes_shader.vert", "tes_shader.frag", "tes_shader.tesc", "tes_shader.tese");
	color = { 0.5,0,0.2,1.0 };
	bezier_points = {};
	points = {};
	quads = {};

	for (const auto& pt : bezier_pts) {
		bezier_points.push_back(pt);
	}
	update_object();
}

void BezierBox::UpdatePoints(std::vector<glm::vec3>& bezier_pts)
{
	bezier_points.clear();

	for (const auto& pt : bezier_pts) {
		bezier_points.push_back(pt);
	}

	need_update = true;
}

void BezierBox::update_object()
{
	quads.clear();
	points.clear();

	for (const auto& pt : bezier_points) {
		points.push_back(pt.x);
		points.push_back(pt.y);
		points.push_back(pt.z);

		//points.push_back(color.r);
		//points.push_back(color.g);
		//points.push_back(color.b);
		//points.push_back(color.a);
	}


	for (int i = 0; i < 16; i++) {
		quads.push_back(i);
	}

	for (int i = 48; i < 64; i++) {
		quads.push_back(i);
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			quads.push_back(i * 16 + j);
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			quads.push_back(i * 16 + j + 12);
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			quads.push_back(i * 16 + j * 4);
		}
	}

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			quads.push_back(i * 16 + j * 4 + 3);
		}
	}

	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * quads.size(), quads.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	//glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, description_number * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
}

void BezierBox::DrawObject(glm::mat4 mvp) {

	if (need_update) {
		update_object();
		need_update = false;
	}

	Object::DrawObject(mvp);
	glm::mat4 model = translate * rotate * resize;
	glm::mat4 vp = mvp;
	shader.use();

	glPatchParameteri(GL_PATCH_VERTICES, 16);

	glBindVertexArray(VAO);

	//glm::mat4 trmodel = glm::transpose(glm::inverse(model));
	//int projectionLoc = glGetUniformLocation(shader.ID, "model");
	//glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(model));

	//int trmodelLoc = glGetUniformLocation(shader.ID, "trmodel");
	//glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(trmodel));

	//int mvLoc = glGetUniformLocation(shader.ID, "vp");
	//glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(vp));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	//glDrawElements(GL_TRIANGLES, quads.size(), GL_UNSIGNED_INT, 0);
	//glLineWidth(3.0f);
	glDrawElements(GL_PATCHES, quads.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}