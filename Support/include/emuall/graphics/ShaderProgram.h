#ifndef SHADER_H_
#define SHADER_H_

#include <emuall/common.h>
#include <string>
#include <vector>

class Texture;
class Texture3D;
class BufferObject;
class BufferTexture;

class DLLEXPORT ShaderSource {
public:
	explicit ShaderSource(const unsigned char *src, unsigned int len = -1);
	explicit ShaderSource(const char *src, int len = -1);
	explicit ShaderSource(const std::string &src);
	explicit ShaderSource(const ShaderSource &other);
	virtual ~ShaderSource();

	ShaderSource &operator=(const ShaderSource &other);

	void AddVersionDirective(int version, bool compatibility = false);
	void AddSource(const std::string &src);
	void AddSource(const char *src, int len = -1);
	void AddSource(const unsigned char *src, unsigned int len = -1);
	void AddSourceFile(const std::string &file);

	const char * GetSrc() const;
	int GetSrcLen() const;

private:
	bool _versionAdded;
	std::string *_source;
};

class DLLEXPORT ShaderProgram {
public:
	enum Type {
		Vertex,
		Fragment,
		TessellationCtrl,
		TessellationEval,
		Geometry,
		Compute
	};

	ShaderProgram();
	ShaderProgram(const char *name);
	~ShaderProgram();

	void AddShader(Type type, ShaderSource &source);
	void Link();
	void Clean();

	void Begin() const;
	void End() const;
	bool IsLinked() const;

	void SetUniform(const char *name, int val);
	void SetUniform(const char *name, int val0, int val1);
	void SetUniform(const char *name, int val0, int val1, int val2);
	void SetUniform(const char *name, int val0, int val1, int val2, int val3);
	void SetUniform(const char *name, unsigned int val);
	void SetUniform(const char *name, unsigned int val0, unsigned int val1);
	void SetUniform(const char *name, unsigned int val0, unsigned int val1, unsigned int val2);
	void SetUniform(const char *name, unsigned int val0, unsigned int val1, unsigned int val2, unsigned int val3);
	void SetUniform(const char *name, float val);
	void SetUniform(const char *name, float val0, float val1);
	void SetUniform(const char *name, float val0, float val1, float val2);
	void SetUniform(const char *name, float val0, float val1, float val2, float val3);
	void SetUniform(const char *name, double val);
	void SetUniform(const char *name, double val0, double val1);
	void SetUniform(const char *name, double val0, double val1, double val2);
	void SetUniform(const char *name, double val0, double val1, double val2, double val3);
	void SetUniform(const char *name, int pos, Texture &texture);
	void SetUniform(const char *name, int pos, Texture3D &texture);
	void SetUniform(const char *name, int pos, BufferTexture &texture);
	void SetUniform(const char *name, int pos, BufferObject &object);

protected:
	void Compile(Type type, unsigned int shader, int count, const char * const * src, const int *len);
	int GetUniformLocation(const char *name);
	int GetUniformBlockLocation(const char *name);
private:
	std::string *_name;
	unsigned int _program;
	std::vector<unsigned int> *_shaders;
};

class DLLEXPORT ShaderCompileException {
public:
	explicit ShaderCompileException(ShaderProgram::Type type, const char *log, const char *name = nullptr);
	explicit ShaderCompileException(const char *log, const char *name = nullptr);

	ShaderCompileException(ShaderCompileException &other);
	ShaderCompileException &operator=(ShaderCompileException &other);

	virtual ~ShaderCompileException();

	virtual const char *GetMsg();

protected:

	std::string *_msg;
};

#endif
