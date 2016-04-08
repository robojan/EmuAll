#ifndef GRAPHICSEXCEPTION_H_
#define GRAPHICSEXCEPTION_H_

#include <emuall/exception.h>

#ifdef _DEBUG
#define GL_CHECKED(x) \
	do { \
		x;\
		GLenum _glresult = glGetError(); \
		if(_glresult != GL_NO_ERROR) throw GraphicsException(_glresult); \
	} while(0)
#else 
#define GL_CHECKED(x) x
#endif

class DLLEXPORT GraphicsException : public BaseException
{
public:
	GraphicsException(unsigned int error, const std::string &msg);
	GraphicsException(unsigned int error, const char *msg);
	GraphicsException(unsigned int error);
	virtual ~GraphicsException();

private:
	unsigned int _error;
};

#endif
