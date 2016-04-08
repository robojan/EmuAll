#ifndef SHADER_H_
#define SHADER_H_

#include <emuall/common.h>
#include <string>
#include <vector>

class Texture;

class DLLEXPORT ShaderProgram {
public:
	enum Type {
		Vertex,
		Fragment,
		TesselationCtrl,
		TesselationEval,
		Geometry,
		Compute
	};

	ShaderProgram();
	~ShaderProgram();
	
	bool AddShader(Type type, const char *src, int len);
	bool AddShader(Type type, const std::string &path);
	bool Link();
	void Clean();

	const char *GetLog() const;
	void Begin() const;
	void End() const;
	bool IsLinked() const;

	void SetUniform(const char *name, int val);
	void SetUniform(const char *name, int val0, int val1);
	void SetUniform(const char *name, int val0, int val1, int val2);
	void SetUniform(const char *name, int val0, int val1, int val2, int val3);
	void SetUniform(const char *name, float val);
	void SetUniform(const char *name, float val0, float val1);
	void SetUniform(const char *name, float val0, float val1, float val2);
	void SetUniform(const char *name, float val0, float val1, float val2, float val3);
	void SetUniform(const char *name, double val);
	void SetUniform(const char *name, double val0, double val1);
	void SetUniform(const char *name, double val0, double val1, double val2);
	void SetUniform(const char *name, double val0, double val1, double val2, double val3);
	void SetUniform(const char *name, int pos, Texture &texture);

protected:
	bool Compile(unsigned int shader, const char *source, int len);
	int GetUniformLocation(const char *name);
private:
	std::string *_log;
	unsigned int _program;
	std::vector<unsigned int> *_shaders;
};

#endif
