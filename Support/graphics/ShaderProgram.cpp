
#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/texture.h>
#include <emuall/graphics/BufferObject.h>
#include <emuall/graphics/bufferTexture.h>
#include <emuall/graphics/graphicsException.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <fstream>
#include <sstream>
#include <assert.h>

ShaderProgram::ShaderProgram() : 
	_program(0)
{
	_log = new std::string;
	_shaders = new std::vector<unsigned int>;
}

ShaderProgram::~ShaderProgram()
{
	Clean();
	delete _log;
	delete _shaders;
}

bool ShaderProgram::AddShader(Type type, const char *src, int len)
{
	bool result = false;
	GLenum shaderType;
	switch (type) {
	case ShaderProgram::Vertex:
		shaderType = GL_VERTEX_SHADER;
		break;
	case ShaderProgram::Fragment:
		shaderType = GL_FRAGMENT_SHADER;
		break;
	case ShaderProgram::Geometry:
		shaderType = GL_GEOMETRY_SHADER;
		break;
	case ShaderProgram::TesselationCtrl:
		shaderType = GL_TESS_CONTROL_SHADER;
		break;
	case ShaderProgram::TesselationEval:
		shaderType = GL_TESS_EVALUATION_SHADER;
		break;
	case ShaderProgram::Compute:
		shaderType = GL_COMPUTE_SHADER;
		break;
	}

	// Create an shader object
	GLuint shader;
	GL_CHECKED(shader = glCreateShader(shaderType));
	if (shader == 0) {
		*_log = "Could not create an GL shader object\n";
		return result;
	}

	result = Compile(shader, src, len);
	if (!result) {
		GL_CHECKED(glDeleteShader(shader));
		return result;
	}

	_shaders->push_back(shader);

	return true;
}

bool ShaderProgram::AddShader(Type type, const std::string &path)
{
	char *src = nullptr;
	bool result = false;

	// Open the file
	std::ifstream file(path, std::ios_base::in);
	if (!file.is_open()) {
		std::ostringstream stringStream;
		stringStream << "Error opening file: " << strerror(errno) << std::endl;
		*_log = stringStream.str();
		return result;
	}

	file.exceptions(std::ios_base::failbit | std::ios_base::badbit);
	try {
		// Get file size
		file.seekg(0, std::ios_base::end);
		std::istream::pos_type size = file.tellg();
		file.seekg(0, std::ios_base::beg);
		assert(size <= INT32_MAX);

		// Create Source buffer
		src = new char[(unsigned int)size];

		// Read content of the file
		file.read(src, size);

		// Add the shader
		result = AddShader(type, src, (int)size);

		// Delete the source
		if (src != nullptr) {
			delete src;
		}
	}
	catch (std::ifstream::failure) {
		std::ostringstream stringStream;
		stringStream << "Error reading file: " << strerror(errno) << std::endl;
		*_log = stringStream.str();

		// Delete the source
		if (src != nullptr) {
			delete src;
		}
	}

	return result;
}

bool ShaderProgram::Link()
{
	// Create an new program
	GL_CHECKED(_program = glCreateProgram());
	if (_program == 0) {
		*_log = "Could not create an shader program\n";
		return false;
	}

	// Attach all the shaders
	for (auto &shader : *_shaders) {
		GL_CHECKED(glAttachShader(_program, shader));
	}

	// Link the program
	GL_CHECKED(glLinkProgram(_program));

	GLint status;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	if (status != GL_TRUE) {
		std::stringstream stringstream;
		stringstream << "Error linking program" << std::endl;
		GLint logLength;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(logLength + 1);
		glGetProgramInfoLog(_program, logLength + 1, &logLength, log.data());
		stringstream << log.data();
		*_log = stringstream.str();
		GL_CHECKED(glDeleteProgram(_program));
		_program = 0;
		return false;
	}
	return true;
}

void ShaderProgram::Clean()
{
	// Delete all the shaders
	for (auto &shader : *_shaders) {
		if (glIsShader(shader)) {
			GL_CHECKED(glDeleteShader(shader));
		}
	}
	_shaders->clear();
	if (glIsProgram(_program)) {
		GL_CHECKED(glDeleteProgram(_program));
		_program = 0;
	}
}

const char *ShaderProgram::GetLog() const
{
	return _log->c_str();
}

void ShaderProgram::Begin() const
{
	assert(glIsProgram(_program) == GL_TRUE);
	assert(IsLinked());
	GL_CHECKED(glUseProgram(_program));
}

void ShaderProgram::End() const
{
	GL_CHECKED(glUseProgram(0));
}

bool ShaderProgram::IsLinked() const
{
	GLint status;
	if (glIsProgram(_program) == GL_FALSE) return false;
	glGetProgramiv(_program, GL_LINK_STATUS, &status);
	return status == GL_TRUE;
}

void ShaderProgram::SetUniform(const char *name, int val)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform1i(loc, val));
	}
}

void ShaderProgram::SetUniform(const char *name, int val0, int val1)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform2i(loc, val0, val1));
	}
}

void ShaderProgram::SetUniform(const char *name, int val0, int val1, int val2)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform3i(loc, val0, val1, val2));
	}
}

void ShaderProgram::SetUniform(const char *name, int val0, int val1, int val2, int val3)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform4i(loc, val0, val1, val2, val3));
	}
}

void ShaderProgram::SetUniform(const char *name, unsigned int val)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform1ui(loc, val));
	}
}

void ShaderProgram::SetUniform(const char *name, unsigned int val0, unsigned int val1)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform2ui(loc, val0, val1));
	}
}

void ShaderProgram::SetUniform(const char *name, unsigned int val0, unsigned int val1, unsigned int val2)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform3ui(loc, val0, val1, val2));
	}
}

void ShaderProgram::SetUniform(const char *name, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform4ui(loc, val0, val1, val2, val3));
	}
}

void ShaderProgram::SetUniform(const char *name, float val)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform1f(loc, val));
	}
}

void ShaderProgram::SetUniform(const char *name, double val)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform1d(loc, val));
	}
}


void ShaderProgram::SetUniform(const char *name, float val0, float val1)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform2f(loc, val0, val1));
	}
}

void ShaderProgram::SetUniform(const char *name, float val0, float val1, float val2)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform3f(loc, val0, val1, val2));
	}
}

void ShaderProgram::SetUniform(const char *name, float val0, float val1, float val2, float val3)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform4f(loc, val0, val1, val2, val3));
	}
}

void ShaderProgram::SetUniform(const char *name, double val0, double val1)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform2d(loc, val0, val1));
	}
}

void ShaderProgram::SetUniform(const char *name, double val0, double val1, double val2)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform3d(loc, val0, val1, val2));
	}
}

void ShaderProgram::SetUniform(const char *name, double val0, double val1, double val2, double val3)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniform4d(loc, val0, val1, val2, val3));
	}
}

void ShaderProgram::SetUniform(const char *name, int pos, BufferObject &object)
{
	int loc = GetUniformBlockLocation(name);
	if (loc != -1) {
		GL_CHECKED(glUniformBlockBinding(_program, loc, pos));
		GL_CHECKED(glBindBufferBase(GL_UNIFORM_BUFFER, pos, object._bo));
	}
}

void ShaderProgram::SetUniform(const char *name, int pos, Texture &texture)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glActiveTexture(GL_TEXTURE0 + pos));
		texture.Bind();
		GL_CHECKED(glUniform1i(loc, pos));
	}
}

void ShaderProgram::SetUniform(const char *name, int pos, BufferTexture &texture)
{
	int loc = GetUniformLocation(name);
	if (loc != -1) {
		GL_CHECKED(glActiveTexture(GL_TEXTURE0 + pos));
		texture.Bind();
		GL_CHECKED(glUniform1i(loc, pos));
	}
}

bool ShaderProgram::Compile(unsigned int shader, const char *source, int len)
{
	assert(glIsShader(shader) == GL_TRUE);


	// Assign the source code to the shader
	GL_CHECKED(glShaderSource(shader, 1, &source, &len));

	// Compile the shader
	GL_CHECKED(glCompileShader(shader));

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		std::stringstream stringstream;
		stringstream << "Error compiling shader\n";
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(logLength+1);
		glGetShaderInfoLog(shader, logLength+1, &logLength, log.data());
		stringstream << log.data();
		*_log = stringstream.str();
		return false;
	}

	return true;
}

int ShaderProgram::GetUniformLocation(const char *name)
{
	int loc;
	GL_CHECKED(loc = glGetUniformLocation(_program, name));
	return loc;
}

int ShaderProgram::GetUniformBlockLocation(const char *name)
{
	int loc;
	GL_CHECKED(loc = glGetUniformBlockIndex(_program, name));
	return loc;
}
