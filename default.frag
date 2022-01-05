#version 330 core

out vec4 FragColor;

in vec3 color;

in vec3 FragPos;

in vec3 normal;

in vec4 EyeSpacePosition;

void main()
{
	float fogMax = 700;
	float fogMin = 700;

	vec3 norm = normalize(normal);
	// lightPos?
	vec3 lightDir = normalize(vec3(0.0, 150.0, 0.0) - FragPos);

	float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * vec3(1.0, 1.0, 1.0);

    vec3 result = ambient * color;

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * vec3(1.0, 1.0, 1.0);

	result = diffuse * color;

	float fogCoordinate = abs(EyeSpacePosition.z / EyeSpacePosition.w);

	float fogFactor = 0.0;
	float fogLength = fogMax - fogMin;
	fogFactor = (fogMax - fogCoordinate) / fogLength;
	fogFactor = 1.0 - clamp(fogFactor, 0.0, 1.0);

	result = mix(result, vec3(1.0, 1.0, 1.0), fogFactor);

	FragColor = vec4(result, 1.0);
}