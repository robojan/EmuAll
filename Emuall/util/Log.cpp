#include "memDbg.h"

#include "log.h"
#include "logWx.h"

#include <cassert>
#include <vector>
#include <cstdarg>
#include <wx/log.h>
#include <Windows.h>

std::vector<LogDest *> _loggers;
char _LogBuffer[1024];
enum loglevel _loglevel;


void InitLog(LogDest *dst)
{
	_loglevel = Message;
	_loggers.clear();
	if (dst != 0)
		_loggers.push_back(dst);
}

void AddLogger(LogDest *dst)
{
	assert(dst != 0);
	_loggers.push_back(dst);		
}

void SetLogLevel(enum loglevel level)
{
	_loglevel = level;
}

void Log(enum loglevel level, char *fmtstr, ...)
{
	if (level <= _loglevel)
	{
		va_list args;
		va_start(args, fmtstr);
		vsnprintf(_LogBuffer, sizeof(_LogBuffer), fmtstr, args);
		std::vector<LogDest *>::iterator it;
		for (it = _loggers.begin(); it != _loggers.end(); it++)
		{
			assert(*it != 0);
			(*it)->log(_LogBuffer);
		}
		va_end(args);
	}
}

void LogWx::log(const char *str)
{
	wxString msg = str;
	msg.Replace("%", "%%");
	wxLogMessage(msg);
	OutputDebugStringA(str);
	OutputDebugStringA("\n");
}
