#ifndef _LOGWX_H
#define _LOGWX_H

#include "log.h"

class LogWx : public LogDest
{
public:
	void log(const char *str);
};

#endif