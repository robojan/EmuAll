
#include <emuall/graphics/shader.h>

#include <GL/glew.h>
#include <GL/GL.h>

Shader::Shader()
{
	_log = new std::string;
	_shaders = new std::vector<unsigned int>;
}

Shader::~Shader()
{
	delete _log;
	delete _shaders;
}

bool Shader::AddShader(Type type, const char *src, size_t len)
{
	GLenum shaderType;
	switch (type) {
	case Shader::Vertex:
		shaderType = GL_VERTEX_SHADER;
		break;
	case Shader::Fragment:
		shaderType = GL_FRAGMENT_SHADER;
		break;

	}
	GLuint shader = glCreateShader(shaderType);
	return false;
}

bool Shader::AddShader(Type type, const std::string &path)
{
	return false;
}

bool Shader::Link()
{
	return false;
}

void Shader::Clean()
{

}

const char *Shader::GetLog() const
{
	return "Not implemented";
}

void Shader::Begin() const
{

}

void Shader::End() const
{

}

bool Shader::IsLinked() const
{
	return false;
}

bool Shader::Compile(unsigned int shader, const char *source, size_t len)
{
	return false;
}
