#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
		 
#include "PerlinNoise.hpp"

#include "shaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "IBO.h"
#include "Camera.h"

const unsigned int width = 800;
const unsigned int height = 800;

const int grid_width = 1024;
const int indices_range = (grid_width - 1) * (grid_width - 1) * 2 * 3;

GLfloat vertices[grid_width * grid_width * 3 * 2] ={};
GLuint indices[indices_range] = {};

void setVertices()
{
	int counter = 0;
	siv::Generate(grid_width);

	int width, height, bpp;
	int dataI = 0;
	unsigned char* data = stbi_load("Perlin_noise.bmp", &width, &height, &bpp, 3);


	for (int i = 0; i < grid_width * grid_width * 3 * 2; i += 6)
	{
		vertices[i] = float(int(counter % grid_width));
		vertices[i + 1] = data[dataI];
		dataI += 3;
		vertices[i + 2] = float(int(counter / grid_width));

		vertices[i + 3] = 0;
		vertices[i + 4] = 0;
		vertices[i + 5] = 0;

		counter++;
	}
}

void setIndices()
{
	int counter = 0;

	for (int i = 0; i < indices_range;)
	{
		if ((counter % grid_width) != (grid_width-1)) {
			indices[i] = counter;
			indices[i + 1] = counter + 1;
			indices[i + 2] = counter + grid_width;

			indices[i + 3] = counter + 1;
			indices[i + 4] = counter + grid_width;
			indices[i + 5] = counter + grid_width + 1;
			i += 6;
		}

		counter++;
	}
}

glm::vec3 getVertex(int vertNumber)
{
	int x = vertices[vertNumber * 6];
	int y = vertices[vertNumber * 6 + 1];
	int z = vertices[vertNumber * 6 + 2];
	return glm::vec3(x, y, z);
}

void setNormals()
{
	for (int i = 0; i < indices_range; i+=3)
	{
		glm::vec3 p1 = getVertex(indices[i]);
		glm::vec3 p2 = getVertex(indices[i+1]);
		glm::vec3 p3 = getVertex(indices[i+2]);

		glm::vec3 vecU = p2 - p1;
		glm::vec3 vecV = p3 - p1;

		int normalX = (vecU.y * vecV.z) - (vecU.z * vecV.y);
		int normalY = (vecU.z * vecV.x) - (vecU.x * vecV.z);
		int normalZ = (vecU.x * vecV.y) - (vecU.y * vecV.x);

		vertices[indices[i] * 6 + 3] = normalX;
		vertices[indices[i] * 6 + 4] = normalY;
		vertices[indices[i] * 6 + 5] = normalZ;

		vertices[indices[i + 1] * 6 + 3] = normalX;
		vertices[indices[i + 1] * 6 + 4] = normalY;
		vertices[indices[i + 1] * 6 + 5] = normalZ;

		vertices[indices[i + 2] * 6 + 3] = normalX;
		vertices[indices[i + 2] * 6 + 4] = normalY;
		vertices[indices[i + 2] * 6 + 5] = normalZ;
	}
}

int main()
{
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "3D Terrain Generator", NULL, NULL);

	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	gladLoadGL();

	setVertices();
	setIndices();
	setNormals();



	Shader shaderProgram("default.vert", "default.frag");

	VAO VAO1;
	VAO1.Bind();

	VBO VBO1(vertices, sizeof(vertices));
	IBO IBO1(indices, sizeof(indices));

	VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));

	VAO1.Unbind();
	VBO1.Unbind();
	IBO1.Unbind();

	glm::vec3 terrainPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 terrainModel = glm::mat4(1.0f);
	terrainModel = glm::translate(terrainModel, terrainPos);

	Camera camera(width, height, glm::vec3(grid_width, 128.0f, grid_width));
	
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetOrientation(), camera.GetUp());


	shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(terrainModel));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0.53f, 0.81f, 0.92f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shaderProgram.Activate();

		camera.Inputs(window);
		camera.Matrix(45.0f, 0.1f, 700.0f, shaderProgram, "camMatrix");
		view = glm::lookAt(camera.GetPosition(), camera.GetPosition() + camera.GetOrientation(), camera.GetUp());
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));

		VAO1.Bind();
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	VAO1.Delete();
	VBO1.Delete();
	IBO1.Delete();
	shaderProgram.Delete();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}