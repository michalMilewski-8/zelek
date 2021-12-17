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
	frame_springs->RecalcSprings(T);
	bezier_springs->RecalcSprings(T);
	for (auto p : frame_points)
		p->ApplyForce(T);
	for (auto p : bezier_points)
		p->ApplyForce(T);
	bezier_vertices->UpdatePoints();
	bezier_springs->UpdatePoints();
	bezier_box->UpdatePoints();
	frame_springs->UpdatePoints();
}

void Block::MoveFrame(glm::vec3 mvm)
{
	for (auto& pt : frame_points) {
		pt->pos += mvm;
	}
	frame_springs->UpdatePoints();
	Update();
}

void Block::DrawFrame(glm::mat4 mvp)
{
	DrawObject(mvp);
}

void Block::create_block_points()
{
	for (const auto& pt : frame_points) {
		points.push_back(pt->pos.x);
		points.push_back(pt->pos.y);
		points.push_back(pt->pos.z);

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
	std::vector<glm::vec3> frame_points_tmp = {
		{-1,-1,-1 },
		{1,-1,-1 },
		{1,-1,1 },
		{-1,-1,1 },

		{-1,1,-1 },
		{1,1,-1 },
		{1,1,1 },
		{-1,1,1 }
	};
	for (auto pt : frame_points_tmp)
	{
		frame_points.push_back(std::make_shared<Point>(params, pt));
	}

	constraint_box_points = {
	{-4,-4,-4 },
	{4,-4,-4 },
	{4,-4,4 },
	{-4,-4,4 },

	{-4,4,-4 },
	{4,4,-4 },
	{4,4,4 },
	{-4,4,4 }
	};


	glm::vec3 curr_pos;
	double mv = 2.0 / 3.0;

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				curr_pos = { -1.0f + i * mv ,-1.0f + j * mv,-1.0f + k * mv };
				bezier_points.push_back(std::make_shared<Point>(params, curr_pos));
			}
		}
	}
}

Block::Block(Shader sh, std::shared_ptr<Parameters> params_) :
	Object(sh, 7, params_)
{
	shader = sh;

	create_stating_points();

	constraint_box = std::make_unique<ConstraintBox>(sh, params_, constraint_box_points);
	bezier_vertices = std::make_unique<BezierPoints>(sh, params_, bezier_points);
	bezier_springs = std::make_unique<BezierSprings>(sh, params_, bezier_points);
	frame_springs = std::make_unique<FrameSprings>(sh, params_, bezier_points, frame_points);
	bezier_box = std::make_unique<BezierBox>(sh, params_, bezier_points);

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

ConstraintBox::ConstraintBox(Shader sh, std::shared_ptr<Parameters> params_, std::vector<glm::vec3>& frame_points) :
	Object(sh, 7, params_)
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

BezierPoints::BezierPoints(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& frame_points) :
	Object(sh, 7, params_)
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

void BezierPoints::UpdatePoints()
{
	need_update = true;
}

void BezierPoints::update_object()
{
	quads.clear();
	points.clear();
	int i = 0;
	for (const auto& pt : bezier_points) {
		points.push_back(pt->pos.x);
		points.push_back(pt->pos.y);
		points.push_back(pt->pos.z);

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

BezierSprings::BezierSprings(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& frame_points) :
	Object(sh, 7, params_)
{
	//shader = Shader("shader.vert", "shader.frag");
	color = { 0.0,0.8,0.8,1.0 };
	springs = {};
	points = {};
	quads = {};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			for (int k = 0; k < 4; k++) {
				if (k < 3) {
					springs.push_back(std::make_shared<Spring>(params,
						frame_points[i * 16 + j * 4 + k],
						frame_points[i * 16 + j * 4 + k + 1], false));
				}

				if (j < 3) {
					springs.push_back(std::make_shared<Spring>(params,
						frame_points[i * 16 + j * 4 + k],
						frame_points[i * 16 + (j + 1) * 4 + k], false));
				}

				if (i < 3) {
					springs.push_back(std::make_shared<Spring>(params,
						frame_points[i * 16 + j * 4 + k],
						frame_points[(i + 1) * 16 + j * 4 + k], false));
				}
			}
		}
	}

	for (int i = 0; i < springs.size() * 2; ++i)
		quads.push_back(i);

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

void BezierSprings::UpdatePoints()
{
	need_update = true;
}

void BezierSprings::update_object()
{
	points.clear();
	for (const auto& sp : springs) {
		points.push_back(sp->p1->pos.x);
		points.push_back(sp->p1->pos.y);
		points.push_back(sp->p1->pos.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);

		points.push_back(sp->p2->pos.x);
		points.push_back(sp->p2->pos.y);
		points.push_back(sp->p2->pos.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);
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

void BezierSprings::RecalcSprings(float dt)
{
	for (auto spring : springs)
		spring->RecalcSpring(dt);
}

FrameSprings::FrameSprings(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& bezier_points, std::vector<std::shared_ptr<Point>>& frame_points) :
	Object(sh, 7, params_)
{
	//shader = Shader("shader.vert", "shader.frag");
	color = { 0.5,0,0.2,1.0 };
	springs = {};
	points = {};
	quads = {};

	springs.push_back(std::make_shared<Spring>(params, frame_points[0], bezier_points[0], true));
	springs.push_back(std::make_shared<Spring>(params, frame_points[1], bezier_points[48], true));
	springs.push_back(std::make_shared<Spring>(params, frame_points[2], bezier_points[51], true));
	springs.push_back(std::make_shared<Spring>(params, frame_points[3], bezier_points[3], true));
	springs.push_back(std::make_shared<Spring>(params, frame_points[4], bezier_points[12], true));
	springs.push_back(std::make_shared<Spring>(params, frame_points[5], bezier_points[60], true));
	springs.push_back(std::make_shared<Spring>(params, frame_points[6], bezier_points[63], true));
	springs.push_back(std::make_shared<Spring>(params, frame_points[7], bezier_points[15], true));

	for (int i = 0; i < 16; i++) {
		quads.push_back(i);
	}

	update_object();
}

void FrameSprings::UpdatePoints()
{
	need_update = true;
}

void FrameSprings::RecalcSprings(float dt)
{
	for (auto spring : springs)
		spring->RecalcSpring(dt);
}

void FrameSprings::update_object()
{
	points.clear();

	for (const auto& spr : springs) {
		points.push_back(spr->p1->pos.x);
		points.push_back(spr->p1->pos.y);
		points.push_back(spr->p1->pos.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);

		points.push_back(spr->p2->pos.x);
		points.push_back(spr->p2->pos.y);
		points.push_back(spr->p2->pos.z);

		points.push_back(color.r);
		points.push_back(color.g);
		points.push_back(color.b);
		points.push_back(color.a);
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

BezierBox::BezierBox(Shader sh, std::shared_ptr<Parameters> params_, std::vector<std::shared_ptr<Point>>& bezier_pts) :
	Object(sh, 3, params_)
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

void BezierBox::UpdatePoints()
{
	need_update = true;
}

void BezierBox::update_object()
{
	quads.clear();
	points.clear();

	for (const auto& pt : bezier_points) {
		points.push_back(pt->pos.x);
		points.push_back(pt->pos.y);
		points.push_back(pt->pos.z);

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