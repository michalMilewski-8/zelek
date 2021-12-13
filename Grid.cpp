#include "Grid.h"

Grid::Grid()
{
	shader = Shader("shader_grid.vert", "shader_grid.frag");
	glGenBuffers(1, &EBO);
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	points = {};
	triangles = {};

	points.push_back(-100);
	points.push_back(0);
	points.push_back(-100);


	points.push_back(100);
	points.push_back(0);
	points.push_back(-100);


	points.push_back(100);
	points.push_back(0);
	points.push_back(100);


	points.push_back(-100);
	points.push_back(0);
	points.push_back(100);


	triangles.push_back(0);
	triangles.push_back(1);
	triangles.push_back(3);

	triangles.push_back(1);
	triangles.push_back(2);
	triangles.push_back(3);

	shader.use();
	// 1. bind Vertex Array Object
	glBindVertexArray(VAO);
	// 2. copy our vertices array in a vertex buffer for OpenGL to use
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * points.size(), points.data(), GL_DYNAMIC_DRAW);
	// 3. copy our index array in a element buffer for OpenGL to use
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * triangles.size(), triangles.data(), GL_DYNAMIC_DRAW);
	// 4. then set the vertex attributes pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3* sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

}

void Grid::Draw(glm::mat4 proj, glm::mat4 view, glm::mat4 proj_i, glm::mat4 view_i)
{
	shader.use();

	glBindVertexArray(VAO);

	int projectionLoc = glGetUniformLocation(shader.ID, "proj");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj));

	int trmodelLoc = glGetUniformLocation(shader.ID, "vieww");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(view));

	projectionLoc = glGetUniformLocation(shader.ID, "proj_i");
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(proj_i));

	trmodelLoc = glGetUniformLocation(shader.ID, "vieww_i");
	glUniformMatrix4fv(trmodelLoc, 1, GL_FALSE, glm::value_ptr(view_i));

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}
