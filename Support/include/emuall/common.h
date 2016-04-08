#ifndef EMUALL_COMMON_H_
#define EMUALL_COMMON_H_

#ifdef EMULIBRARYDLL
#	define DLLEXPORT __declspec(dllexport) 
#else
#	define DLLEXPORT __declspec(dllimport)
#endif

#endif