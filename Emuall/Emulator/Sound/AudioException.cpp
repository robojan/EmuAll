#include "AudioException.h"
#include <AL/alc.h>
#include <AL/al.h>

AudioException::AudioException(unsigned int error, bool al) :
	BaseException(al ? GetALErrorMsg(error) : GetALCErrorMsg(error)), _error(error),
	_alError(al)
{

}

AudioException::AudioException(unsigned int error, const char *msg, bool al) :
	BaseException(msg), _error(error), _alError(al)
{
	_msg->append("(");
	_msg->append(al ? GetALErrorMsg(error) : GetALCErrorMsg(error));
	_msg->append(")");
}

AudioException::AudioException(unsigned int error, const std::string &msg, bool al) :
	BaseException(msg), _error(error), _alError(al)
{
	_msg->append("(");
	_msg->append(al ? GetALErrorMsg(error) : GetALCErrorMsg(error));
	_msg->append(")");
}

AudioException::AudioException(AudioException &other) :
	BaseException(other), _error(other._error), _alError(other._alError)
{

}

AudioException::~AudioException()
{

}

AudioException & AudioException::operator=(AudioException &other)
{
	BaseException::operator=(other);
	_error = other._error;
	_alError = other._alError;
	return *this;
}

unsigned int AudioException::GetErrorCode() const
{
	return _error;
}

bool AudioException::IsALError() const
{
	return _alError;
}

const char * AudioException::GetALCErrorMsg(unsigned int error)
{
	switch (error) {
	default:
	case ALC_NO_ERROR:
		return "ALC_NO_ERROR";
	case ALC_INVALID_DEVICE:
		return "ALC_INVALID_DEVICE";
	case ALC_INVALID_CONTEXT:
		return "ALC_INVALID_CONTEXT";
	case ALC_INVALID_ENUM:
		return "ALC_INVALID_ENUM";
	case ALC_INVALID_VALUE:
		return "ALC_INVALID_VALUE";
	case ALC_OUT_OF_MEMORY:
		return "ALC_OUT_OF_MEMORY";
	}
}

const char * AudioException::GetALErrorMsg(unsigned int error)
{
	switch (error) {
	default:
	case AL_NO_ERROR:
		return "AL_NO_ERROR";
	case AL_INVALID_NAME:
		return "AL_INVALID_NAME";
	case AL_INVALID_ENUM:
		return "AL_INVALID_ENUM";
	case AL_INVALID_VALUE:
		return "AL_INVALID_VALUE";
	case AL_INVALID_OPERATION:
		return "AL_INVALID_OPERATION";
	case AL_OUT_OF_MEMORY:
		return "AL_OUT_OF_MEMORY";
	}
}
