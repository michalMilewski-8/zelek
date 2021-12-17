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
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>

#include "Block.h"
#include "Cursor.h"
#include "Grid.h"
#include "Parameters.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Dependencies/include/rapidxml-1.13/rapidxml.hpp"
#include "Dependencies/include/rapidxml-1.13/rapidxml_print.hpp"
#include "Dependencies/include/rapidxml-1.13/rapidxml_utils.hpp"
#include "./Dependencies/include/ImGuiFileDialog-Lib_Only/ImGuiFileDialog.h"

#define DEFAULT_WIDTH 1280
#define DEFAULT_HEIGHT 720
#define EPS 0.1
#define PRECISION 1.0f
using namespace rapidxml;
using namespace std;

glm::vec3 cameraPos, cameraFront, cameraUp, lookAt, moving_up;
unsigned int width_, height_;

int e = 0;
glm::mat4 projection, view, model, mvp;
glm::mat4 projection_i, view_i, model_i, mvp_i;
glm::vec2 mousePosOld, angle;
float start_angle = 45.0f;
float angular_speed_ = 1.0f;
float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame
float ipd = 0.01f;
float d = 0.1f;
float near = 0.01f;
float far = 200.0f;
bool animate = true;
float T = 0.0f;
float animation_time = 1.0f;
int iteration_per_frame = 10;

float distance_d = 1.0f;
float speed = 0.2f; // speed of milling tool in milimeters per second;
float size_x = 1;
float size_y = 1;
float size_z = 1;
float denisity = 1;
int divisions_x = 400;
int divisions_y = 400;
glm::vec3 translation_s;
glm::vec3 translation_e;
glm::vec3 rot_euler_s;
glm::vec3 rot_euler_e;
glm::quat quaternion_s = { 0,0,0,1 };
glm::quat quaternion_e = { 0,0,0,1 };
bool draw_trajectory = false;

Camera cam;
Shader ourShader;
std::unique_ptr<Grid> gridShader;
GLFWwindow* window;
GLFWwindow* window2;
//std::unique_ptr<Cursor> cursor, center;

std::vector<std::shared_ptr<Object>> objects_list = {};
std::unique_ptr<Block> block;
std::unique_ptr<Cursor> cursor;
std::unique_ptr<Line> trajectory;

std::shared_ptr<Parameters> params;

void draw_scene();
void draw_scene2();
void framebuffer_size_callback(GLFWwindow* window_1, int width, int height);
void framebuffer_size_callback2(GLFWwindow* window_1, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void create_gui();

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	window = glfwCreateWindow(DEFAULT_WIDTH, DEFAULT_HEIGHT, "Zelek", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	model = glm::mat4(1.0f);
	view = glm::mat4(1.0f);

	cameraPos = { 0,0,5 };
	cameraFront = { 0,0,-1 };
	lookAt = { 0,0,0 };
	cameraUp = { 0,1,0 };

	angle = { -90.0f, 0.0f };
	width_ = DEFAULT_WIDTH;
	height_ = DEFAULT_HEIGHT;

	cam = Camera(cameraPos, cameraFront, cameraUp);
	cam.SetPerspective(glm::radians(45.0f), DEFAULT_WIDTH / (float)DEFAULT_HEIGHT, near, far);


	glfwMakeContextCurrent(window);

	params = std::make_shared<Parameters>();

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();
	const char* glsl_version = "#version 330";
	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// build and compile our shader program
	// ------------------------------------
	ourShader = Shader("shader.vert", "shader.frag"); // you can name your shader files however you like
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	gridShader = std::make_unique<Grid>();

	block = std::make_unique<Block>(ourShader, params);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		glfwMakeContextCurrent(window);
		// cleaning frame
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		projection = cam.GetProjectionMatrix();
		projection_i = glm::inverse(projection);
		view = cam.GetViewMatrix();
		view_i = glm::inverse(view);

		mvp = projection * view;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);
		create_gui();

		draw_scene();

		// Render the grid
		gridShader->Draw(projection, view, projection_i, view_i);

		// Render dear imgui into screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// check and call events and swap the buffers
		glfwPollEvents();
		glfwSwapBuffers(window);

		if (animate) {
			T += deltaTime / animation_time;
		}
	}

	// cleanup stuff
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	objects_list.clear();
	ourShader.deleteShader();
	return 0;
}

void draw_scene() {
	if (animate) {
		float delta = deltaTime / iteration_per_frame;
		for (int i = 0; i < iteration_per_frame; i++) {
			block->CalculateFrame(delta);
		}
		block->DrawObject(mvp);
	}
	else {
		block->DrawObject(mvp);
	}
}

#pragma region  boilerCodeOpenGL

void framebuffer_size_callback(GLFWwindow* window_1, int width, int height)
{
	glfwMakeContextCurrent(window_1);
	glViewport(0, 0, width, height);

	cam.SetPerspective(glm::radians(45.0f), width / (float)height, near, far);
	width_ = width;
	height_ = height;
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;
	glm::vec2 mousePos = { xpos,ypos };
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = 40 * deltaTime;
		float radius;
		diff *= cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * -diff.x;
		glm::vec3 up_movement = cam.GetUpVector() * diff.y;
		glm::vec3 angle2 = lookAt - (cameraPos + right_movement + up_movement);

		auto rotation = Object::RotationBetweenVectors(lookAt - cameraPos, angle2);
		auto roation = glm::toMat4(rotation);
		angle += diff;
		if (angle.y > 90.0f) angle.y = 90.0f - EPS;
		if (angle.y < -90.0f) angle.y = -90.0f + EPS;
		if (angle.x > 180.0f) angle.x = -180.0f + EPS;
		if (angle.x < -180.0f) angle.x = 180.0f - EPS;
		radius = glm::length(cameraPos - lookAt);

		cameraPos.x = lookAt.x + radius * glm::cos(glm::radians(angle.y)) * glm::cos(glm::radians(angle.x));
		cameraPos.z = lookAt.z + radius * -glm::cos(glm::radians(angle.y)) * glm::sin(glm::radians(angle.x));
		cameraPos.y = lookAt.y + radius * glm::sin(glm::radians(-angle.y));

		cameraFront = glm::normalize(lookAt - cameraPos);
		cam.LookAt(cameraPos, cameraFront, cameraUp);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = speed * deltaTime;

		glm::vec2 movement = diff * cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * movement.x;
		glm::vec3 up_movement = cam.GetUpVector() * -movement.y;
		cameraPos += right_movement + up_movement;
		lookAt += right_movement + up_movement;

		cam.LookAt(cameraPos, cameraFront, cameraUp);
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		glm::vec2 diff = (mousePosOld - mousePos) * PRECISION;
		float cameraSpeed = 2.6f * deltaTime;

		glm::vec2 movement = diff * cameraSpeed;

		glm::vec3 right_movement = cam.GetRightVector() * -movement.x;
		glm::vec3 up_movement = cam.GetUpVector() * movement.y;
		block->MoveFrame(right_movement + up_movement);

	}
	mousePosOld = mousePos;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	float precision = 0.05f;

	float movement = 1.0f - yoffset * precision;
	if (movement <= 0.0f)
		movement = 0.1f;
	cameraFront = glm::normalize(lookAt - cameraPos);
	float dist = glm::length(lookAt - cameraPos);
	cameraPos = lookAt - (cameraFront * dist * movement);
	cam.LookAt(cameraPos, cameraFront, cameraUp);

}

#pragma endregion

void create_gui() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	//ImGui::ShowDemoWindow();
	bool open;
	ImGuiWindowFlags flags = 0;
	//flags |= ImGuiWindowFlags_MenuBar;
	ImGui::Begin("Main Menu##uu", &open, flags);

	ImGui::SliderFloat("k", &params->k, 0.0f, 100.0f);
	ImGui::SliderFloat("c1", &params->c1, 0.01f, 100.0f);
	ImGui::SliderFloat("c2", &params->c2, 0.01f, 100.0f);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
}
