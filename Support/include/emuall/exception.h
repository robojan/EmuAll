#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <emuall/common.h>
#include <string>

class EMUEXPORT BaseException {
public:
	explicit BaseException(const char *msg);
	explicit BaseException(const std::string &msg);
	BaseException(BaseException &other);

	BaseException &operator=(BaseException &other);

	virtual ~BaseException();

	virtual const char *GetMsg() const;
	virtual const char *GetStacktrace() const;

protected:
	void CreateStacktrace();

	std::string *_msg;
	std::string *_stacktrace;
};

#endif