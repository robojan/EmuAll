#ifndef SHADER_H_
#define SHADER_H_

#include <emuall/common.h>
#include <string>
#include <vector>

class EMUEXPORT Shader {
public:
	enum Type {
		Vertex,
		Fragment,
		Tesselation,
		Geometry,
		Compute
	};

	Shader();
	~Shader();
	
	bool AddShader(Type type, const char *src, size_t len);
	bool AddShader(Type type, const std::string &path);
	bool Link();
	void Clean();

	const char *GetLog() const;
	void Begin() const;
	void End() const;
	bool IsLinked() const;
protected:
	bool Compile(unsigned int shader, const char *source, size_t len);

private:
	std::string *_log;
	unsigned int _program;
	std::vector<unsigned int> *_shaders;
};

#endif
