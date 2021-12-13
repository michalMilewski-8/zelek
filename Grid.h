#pragma once
#include <stdio.h>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <math.h>
#include <vector>
#include <algorithm>
#include <memory>
#include<functional>

#include "Shader.h"

class Grid
{
public:
	Grid();

	void Draw(glm::mat4 proj, glm::mat4 view, glm::mat4 proj_i, glm::mat4 view_i);

private:
	unsigned int EBO, VAO, VBO;
	Shader shader;
	std::vector<float> points;
	std::vector<unsigned int> triangles;
};

