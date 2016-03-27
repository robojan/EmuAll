#ifndef EMUALL_COMMON_H_
#define EMUALL_COMMON_H_

#ifdef EMUDLL
#	define EMUEXPORT __declspec(dllexport) 
#else
#	define EMUEXPORT __declspec(dllimport)
#endif

#endif