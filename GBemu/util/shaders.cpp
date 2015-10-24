#include "memDbg.h"


#include "shaders.h"

#include <GL/glew.h>
#include "log.h"


unsigned int Shaders::LoadShaderMem(const char *vertex, int vertexSize, const char *fragment, int fragmentSize)
{
	GLuint vertexShader = LoadVertexShaderMem(vertex, vertexSize);
	GLuint fragmentShader = LoadFragmentShaderMem(fragment, fragmentSize);
	GLint result;
	int infoLogLength;

	// Link the program
	Log(Message, "Linking shader program");
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);

	// Check the result
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	char *errorLog = new char[infoLogLength];
	glGetProgramInfoLog(programID, infoLogLength, NULL, errorLog);
	if (result == GL_FALSE)
	{
		Log(Error, "Failed to link the program:\n%s", errorLog);
	}
	else {
		Log(Debug, "linking successfull!");
	}

	delete[] errorLog;

	// delete shaders
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return programID;
}

unsigned int Shaders::LoadVertexShaderMem(const char *vertex, int vertexSize)
{
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLint result = GL_FALSE;
	int infoLogLength;

	Log(Message, "Compiling vertex shader ...");
	glShaderSource(vertexShaderID, 1, &vertex, &vertexSize);
	glCompileShader(vertexShaderID);

	// Check the result
	glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	char *errorLog = new char[infoLogLength];
	glGetShaderInfoLog(vertexShaderID, infoLogLength, NULL, errorLog);
	if (result == GL_FALSE)
	{
		Log(Error, "Failed to compile the vertex shader:\n%s", errorLog);
	}
	else {
		Log(Debug, "Compilation successfull!");
	}

	delete[] errorLog;
	return vertexShaderID;
}

unsigned int Shaders::LoadFragmentShaderMem(const char *fragment, int fragmentSize)
{
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
	GLint result = GL_FALSE;
	int infoLogLength;

	Log(Message, "Compiling fragment shader ...");
	glShaderSource(fragmentShaderID, 1, &fragment, &fragmentSize);
	glCompileShader(fragmentShaderID);

	// Check the result
	glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragmentShaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
	char *errorLog = new char[infoLogLength];
	glGetShaderInfoLog(fragmentShaderID, infoLogLength, NULL, errorLog);
	if (result == GL_FALSE)
	{
		Log(Error, "Failed to compile the fragment shader:\n%s", errorLog);
	}
	else {
		Log(Debug, "Compilation successfull!");
	}

	delete[] errorLog;
	return fragmentShaderID;
}
