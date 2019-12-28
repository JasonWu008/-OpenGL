#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Camera.h"
#include "Shader.h"
#include "Model.h"

#include <iostream>
#include <direct.h>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
std::string getFullPath(std::string path);

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float vertices[] = {
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
	0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
	-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
	-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

glm::vec3 pointLightPositions[] = {
	glm::vec3(1.2f, 1.0f, 0.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
};


int rotateFlag = 1;
float speed = 0.001;

glm::vec3 spotLightIntensity1(1.0f, 1.0f, 1.0f);
glm::vec3 spotLightIntensity2(1.0f, 1.0f, 1.0f);
int spotFlag = 1;

int main() {

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
#endif

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader ourShader("Shader_2.vs", "Shader_2.fs");
	Model ourModel(getFullPath("sands\\sands location.obj").c_str(), getFullPath("sands\\sands location.mtl").c_str());
	Model ourModel2(getFullPath("weapon\\mxng.obj").c_str(), getFullPath("weapon\\mxng.mtl").c_str());
	Model ourModel3(getFullPath("nanosuit\\nanosuit.obj").c_str(), getFullPath("nanosuit\\nanosuit.mtl").c_str());

	Shader lampShader("Shader_1.vs", "Shader_1.fs");

	// VBO，物体VAO
	unsigned int VBO, lightVAO;
	//(1, &cubeVAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 光源VAO (VBO相同，因为顶点数据是同一组)
	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// 位置属性（只需要更新跨度就可以了）
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//lightPos = glm::vec3(0.5f, 0.5f, 2.0f);
	
	float angle = 0;

	while (!glfwWindowShouldClose(window)) {
		if (rotateFlag == 1) angle += speed;

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ourShader.use();

		ourShader.setVec3("viewPos", camera.Position);
		ourShader.setFloat("material.shininess", 32.0f);

		ourShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		ourShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

		ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
		ourShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		ourShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		ourShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		ourShader.setFloat("pointLights[0].constant", 1.0f);
		ourShader.setFloat("pointLights[0].linear", 0.09);
		ourShader.setFloat("pointLights[0].quadratic", 0.032);
		
		ourShader.setVec3("spotLight[0].position", 0.0f, 0.4f, 0.7f);
		ourShader.setVec3("spotLight[0].direction", 0.0f,-1.0f,0.0f);
		ourShader.setVec3("spotLight[0].ambient", spotLightIntensity1);
		ourShader.setVec3("spotLight[0].diffuse", spotLightIntensity1);
		ourShader.setVec3("spotLight[0].specular", spotLightIntensity1);
		ourShader.setFloat("spotLight[0].constant", 1.0f);
		ourShader.setFloat("spotLight[0].linear", 0.09);
		ourShader.setFloat("spotLight[0].quadratic", 0.032);
		ourShader.setFloat("spotLight[0].cutOff", glm::cos(glm::radians(15.5f)));
		ourShader.setFloat("spotLight[0].outerCutOff", glm::cos(glm::radians(18.0f)));

		//摄像机聚光灯
		ourShader.setVec3("spotLight[1].position", camera.Position);
		ourShader.setVec3("spotLight[1].direction", camera.Front);

		ourShader.setVec3("spotLight[1].ambient", spotLightIntensity2);
		ourShader.setVec3("spotLight[1].diffuse", spotLightIntensity2);
		ourShader.setVec3("spotLight[1].specular", spotLightIntensity2);

		ourShader.setFloat("spotLight[1].constant", 1.0f);
		ourShader.setFloat("spotLight[1].linear", 0.09);
		ourShader.setFloat("spotLight[1].quadratic", 0.032);
		ourShader.setFloat("spotLight[1].cutOff", glm::cos(glm::radians(1.5f)));
		ourShader.setFloat("spotLight[1].outerCutOff", glm::cos(glm::radians(15.0f)));
		

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		ourShader.setMat4("projection", glm::value_ptr(projection));
		ourShader.setMat4("view", glm::value_ptr(view));

		glm::mat4 model;
		model = glm::translate(model, glm::vec3(0.0f, -1.75f, 0.0f)); 
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));	
		ourShader.setMat4("model", glm::value_ptr(model));
		ourModel.Draw(ourShader);

		model = glm::translate(model, glm::vec3(0.0f, 10.75f, 8.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.15f, 0.15f));
		model = glm::rotate(model, 1*(float)angle,glm::vec3(0.0f, 0.1f, 0.0f));
		ourShader.setMat4("model", glm::value_ptr(model));	
		ourModel2.Draw(ourShader);
		
		lampShader.use();
		lampShader.setMat4("projection", glm::value_ptr(projection));
		lampShader.setMat4("view", glm::value_ptr(view));

		glm::mat4 model3;
		model3 = glm::translate(model3,pointLightPositions[0]);
		model3 = glm::scale(model3, glm::vec3(0.2f));
		lampShader.setMat4("model", glm::value_ptr(model3));
		glBindVertexArray(lightVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	//处理旋转
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
	{
		spotFlag == 0;
		spotLightIntensity1 = glm::vec3(0.0f, 0.0f, 0.0f);
		rotateFlag = 0;
	}
		
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
	{
		spotFlag == 1;
		spotLightIntensity1 = glm::vec3(1.0f, 1.0f, 1.0f);
		rotateFlag = 1;
	}

	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		speed += 0.0001;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		speed -= 0.0001;

	//光的颜色
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(1.0f, 1.0f, 0.0f);
	}

	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(0.0f, 1.0f, 1.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(1.0f, 0.0f, 1.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(0.5f, 0.5f, 1.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(0.9f, 0.0f, 0.3f);
	}
	if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(0.2f, 0.4f, 0.7f);
	}
	if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(0.9f, 0.7f, 0.1f);
	}
	if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS)
	{
		if (spotFlag == 1)
			spotLightIntensity1 = glm::vec3(0.5f, 0.4f, 0.5f);
	}

	//手电筒
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		//spotFlag == 0;
		spotLightIntensity2 = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
	{
		//spotFlag == 1;
		spotLightIntensity2 = glm::vec3(1.0f, 1.0f, 1.0f);
	}
				
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos) {
	if (firstMouse) {
		lastX = xPos;
		lastY = yPos;
		firstMouse = false;
	}

	float xoffset = lastX - xPos;	
	float yoffset = lastY - yPos;	
	lastX = xPos;
	lastY = yPos;

	camera.ProcessMouseMovement(xoffset, yoffset);

}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	camera.ProcessMouseScroll(yoffset);
}

std::string getFullPath(std::string path) {
	char* buffer;
	buffer = _getcwd(NULL, 0);
	std::string fullPath(buffer);
	
	fullPath = fullPath + "\\" + path;

	return fullPath;
}
