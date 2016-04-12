#pragma once

enum loglevel
{
	Fatal,
	Error,
	Warn,
	Message,
	Debug
};

extern void(*Log)(enum loglevel, char *fmtstr, ...);