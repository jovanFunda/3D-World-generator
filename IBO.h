#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>

class IBO
{
public:
	GLuint ID;
	IBO(GLuint* indices, GLsizeiptr size);

	void Bind();
	void Unbind();
	void Delete();
};

#endif