#ifndef AUDIOEXCEPTION_H_
#define AUDIOEXCEPTION_H_

#include <emuall/exception.h>

class AudioException : public BaseException
{
public:
	AudioException(unsigned int error, bool al = true);
	AudioException(unsigned int error, const char *msg, bool al = true);
	AudioException(unsigned int error, const std::string &msg, bool al = true);
	AudioException(AudioException &other);

	virtual ~AudioException();

	AudioException &operator=(AudioException &other);

	unsigned int GetErrorCode() const;
	bool IsALError() const;

	static const char *GetALCErrorMsg(unsigned int error);
	static const char *GetALErrorMsg(unsigned int error);
private:
	unsigned int _error;
	bool _alError;
};

#endif
