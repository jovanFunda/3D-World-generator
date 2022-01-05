#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 color;

out vec3 normal;

out vec3 FragPos;

smooth out vec4 EyeSpacePosition;

uniform mat4 camMatrix;
uniform mat4 model;
uniform mat4 view;


void main()
{
	mat4 mvMatrix = view * model;

	gl_Position = camMatrix * vec4(aPos.x, aPos.y / 3.0, aPos.z, 1.0);

	FragPos = vec3(model * vec4(aPos, 1.0));

	color = vec3(1.0, 1.0, 1.0);

	//color = vec3(aPos.y / 256, aPos.y / 256, aPos.y / 256);

	normal = aNormal;
	EyeSpacePosition = mvMatrix * vec4(aPos.x, aPos.y / 3.0, aPos.z, 1.0);
}