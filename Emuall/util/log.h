#ifndef _LOG_H
#define _LOG_H

enum loglevel
{
	Fatal,
	Error,
	Warn,
	Message,
	Debug
};

class LogDest
{
public:
	virtual void log(const char *) = 0;
};

void InitLog(LogDest *dst = 0);
void AddLogger(LogDest *dst);
void SetLogLevel(enum loglevel);
void Log(enum loglevel, char *fmtstr, ...);

#endif