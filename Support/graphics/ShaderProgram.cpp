
#include <emuall/graphics/ShaderProgram.h>
#include <emuall/graphics/texture.h>
#include <emuall/graphics/texture3D.h>
#include <emuall/graphics/BufferObject.h>
#include <emuall/graphics/bufferTexture.h>
#include <emuall/graphics/graphicsException.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <fstream>
#include <sstream>
#include <assert.h>

static GLenum GetGLShaderType(ShaderProgram::Type type) {
	switch (type) {
	case ShaderProgram::Vertex: return GL_VERTEX_SHADER;
	case ShaderProgram::Fragment: return GL_FRAGMENT_SHADER;
	case ShaderProgram::Geometry: return GL_GEOMETRY_SHADER;
	case ShaderProgram::TessellationCtrl: return GL_TESS_CONTROL_SHADER;
	case ShaderProgram::TessellationEval: return GL_TESS_EVALUATION_SHADER;
	case ShaderProgram::Compute: return GL_COMPUTE_SHADER;
	}
	throw GraphicsException(GL_INVALID_ENUM, "Unknown shader type");
}

ShaderProgram::ShaderProgram() : 
	_program(0)
{
	_name = nullptr;
	_shaders = new std::vector<unsigned int>;
}

ShaderProgram::ShaderProgram(const char *name)
{
	_name = new std::string(name);
	_shaders = new std::vector<unsigned int>;
}

ShaderProgram::~ShaderProgram()
{
	Clean();
	if (_name != nullptr) {
		delete _name;
		_name = nullptr;
	}
	delete _shaders;
}

void ShaderProgram::AddShader(Type type, ShaderSource &source)
{
	GLenum shaderType = GetGLShaderType(type);

	// Create an shader object
	GLuint shader;
	GL_CHECKED(shader = glCreateShader(shaderType));

	const char *src = source.GetSrc();
	int srcLen = source.GetSrcLen();

	Compile(type, shader, 1, &src, &srcLen);

	_shaders->push_back(shader);
}

void ShaderProgram::Link()
{
	// Create an new program
	GL_CHECKED(_program = glCreateProgram());

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
		GLint logLength;
		glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(logLength + 1);
		glGetProgramInfoLog(_program, logLength + 1, &logLength, log.data());
		stringstream << log.data();
		GL_CHECKED(glDeleteProgram(_program));
		_program = 0;
		throw ShaderCompileException(stringstream.str().c_str(), 
			_name != nullptr ? _name->c_str() : nullptr);
	}
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

void ShaderProgram::SetUniform(const char *name, int pos, Texture3D &texture)
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

void ShaderProgram::Compile(Type type, unsigned int shader, int count, const char * const * src, const int *len)
{
	assert(glIsShader(shader) == GL_TRUE);
	
	// Assign the source code to the shader
	GL_CHECKED(glShaderSource(shader, count, src, len));

	// Compile the shader
	GL_CHECKED(glCompileShader(shader));

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status != GL_TRUE) {
		GLint logLength;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		std::vector<char> log(logLength+1);
		glGetShaderInfoLog(shader, logLength+1, &logLength, log.data());
		throw ShaderCompileException(type, log.data(), _name != nullptr ? _name->c_str() : nullptr);
	}
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

ShaderCompileException::ShaderCompileException(ShaderProgram::Type type, const char *log, const char *name /*= nullptr*/)
{
	_msg = new std::string("Error while compiling ");
	switch (type) {
	case ShaderProgram::Vertex:
		_msg->append("vertex");
		break;
	case ShaderProgram::Fragment:
		_msg->append("fragment");
		break;
	case ShaderProgram::TessellationCtrl:
		_msg->append("tessellation control");
		break;
	case ShaderProgram::TessellationEval:
		_msg->append("tessellation evaluation");
		break;
	case ShaderProgram::Geometry:
		_msg->append("geometry");
		break;
	case ShaderProgram::Compute:
		_msg->append("compute");
		break;
	default:
		_msg->append("Unknown");
		break;
	}
	_msg->append(" shader");
	if (name) {
		_msg->append(" in shader program \"");
		_msg->append(name);
		_msg->append("\"");
	}
	_msg->append(":\n");
	_msg->append(log);
}

ShaderCompileException::ShaderCompileException(const char *log, const char *name /*= nullptr*/)
{
	_msg = new std::string("Error while linking shader program");
	if (name) {
		_msg->append(" \"");
		_msg->append(name);
		_msg->append("\"");
	}
	_msg->append(":\n");
	_msg->append(log);
}

ShaderCompileException::ShaderCompileException(ShaderCompileException &other)
{
	_msg = new std::string(*other._msg);
}

ShaderCompileException::~ShaderCompileException()
{
	delete _msg;
}

const char * ShaderCompileException::GetMsg()
{
	return _msg->c_str();
}

ShaderCompileException & ShaderCompileException::operator=(ShaderCompileException &other)
{
	*_msg = *other._msg;
	return *this;
}

ShaderSource::ShaderSource(const char *src, int len /*= -1*/) :
	_versionAdded(false), _source(new std::string)
{
	AddSource(src, len);
}

ShaderSource::ShaderSource(const std::string &src) :
	_versionAdded(false), _source(new std::string)
{
	AddSource(src);
}

ShaderSource::ShaderSource(const unsigned char *src, unsigned int len /*= -1*/) :
	_versionAdded(false), _source(new std::string)
{
	AddSource((const char *)src, (int)len);
}

ShaderSource::ShaderSource(const ShaderSource &other):
	_source(new std::string(*other._source)), _versionAdded(other._versionAdded)
{

}

ShaderSource::~ShaderSource()
{
	delete _source;
}

ShaderSource & ShaderSource::operator=(const ShaderSource &other)
{
	*_source = *other._source;
	_versionAdded = other._versionAdded;
	return *this;
}

void ShaderSource::AddVersionDirective(int version, bool compatibility /*= false*/)
{
	if (_versionAdded) return;
	
	std::string versionString("#version ");
	versionString.append(std::to_string(version));
	if (compatibility) {
		versionString.append(" compatibility\n");
	}
	else {
		versionString.append(" core\n");
	}
	versionString.append(*_source);

	*_source = versionString;
	_versionAdded = true;
}

void ShaderSource::AddSource(const std::string &src)
{
	_source->append(src);
}

void ShaderSource::AddSource(const char *src, int len /*= -1*/)
{
	if (len >= 0) {
		_source->append(src, len);
	}
	else {
		_source->append(src);
	}
}

void ShaderSource::AddSource(const unsigned char *src, unsigned int len /*= -1*/)
{
	AddSource((const char *)src, (int)len);
}

void ShaderSource::AddSourceFile(const std::string &path)
{
	char *src = nullptr;

	// Open the file
	std::ifstream file(path, std::ios_base::in);
	if (!file.is_open()) {
		std::ostringstream stringStream;
		stringStream << "Error opening file: " << strerror(errno) << std::endl;
		throw std::exception(stringStream.str().c_str());
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
		_source->append(src);

		// Delete the source
		if (src != nullptr) {
			delete src;
		}
	}
	catch (std::ifstream::failure &e) {
		// Delete the source
		if (src != nullptr) {
			delete src;
		}
		throw e;
	}
}

const char * ShaderSource::GetSrc() const
{
	return _source->c_str();
}

int ShaderSource::GetSrcLen() const
{
	return _source->length();
}
