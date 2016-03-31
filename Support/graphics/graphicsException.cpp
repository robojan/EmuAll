#include <emuall/graphics/graphicsException.h>
#include <windows.h>
#include <GL/glu.h>

GraphicsException::GraphicsException(unsigned int error, const std::string &msg) :
	BaseException(msg), _error(error)
{

}

GraphicsException::GraphicsException(unsigned int error, const char *msg) : 
	BaseException(msg), _error(error)
{

}

GraphicsException::GraphicsException(unsigned int error) :
	BaseException(""), _error(error)
{
	*_msg = (const char *)gluErrorString(_error);
}

GraphicsException::~GraphicsException()
{

}
