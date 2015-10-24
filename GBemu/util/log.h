#ifndef _LOG_H
#define _LOG_H

#include "../emu.h"

enum loglevel
{
	Fatal,
	Error,
	Warn,
	Message,
	Debug
};

extern void (*Log)(enum loglevel, char *fmtstr, ...);

#endif