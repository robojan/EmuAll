//Boby Thomas pazheparampil - march 2006
#ifndef os_call_h
#define os_call_h

#include <stdlib.h>
#if defined(_MSC_VER) // Microsoft compiler
#include <windows.h>
#elif defined(__GNUC__) // GNU compiler
#include <dlfcn.h>
#else
#error define your copiler
#endif


#define RTLD_LAZY   1
#define RTLD_NOW    2
#define RTLD_GLOBAL 4

void* LoadSharedLibrary(const char *pcDllname, int iMode = 2)
{
	char *name = (char *) malloc(strlen(pcDllname) + 1 + 4);
	void *handle = NULL;
	name = strcpy(name, pcDllname);
#if defined(_MSC_VER) // Microsoft compiler
	name = strcat(name, ".dll");
	handle = (void*) LoadLibraryA(name);
#elif defined(__GNUC__) // GNU compiler
	name = strcat(name, ".so");
	hanlde = dlopen(name, iMode);
#endif
	free(name);
	return handle;
}

void *GetFunction(void *Lib, const char *Fnname)
{
#if defined(_MSC_VER) // Microsoft compiler
	return (void*) GetProcAddress((HINSTANCE) Lib, Fnname);
#elif defined(__GNUC__) // GNU compiler
	return dlsym(Lib, Fnname);
#endif
}

void *_GetStdcallFunc(void *lib, const char *name, int argSize)
{
#ifdef _WIN32
#ifndef _WIN64
	static char buffer[255];
	sprintf(buffer, "_%s@%d", name, argSize);
	printf("Function %s\n", buffer);
	return GetFunction(lib, buffer);
#endif
	(void)argSize;
	return GetFunction(lib, name);
#else
	(void)argSize;
	return GetFunction(lib, name);
#endif
}

void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, 0);
}

template<typename T1>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1));
}

template<typename T1, typename T2>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2));
}

template<typename T1, typename T2, typename T3>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3));
}

template<typename T1, typename T2, typename T3, typename T4>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3) + sizeof(T4));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3) + sizeof(T4) + sizeof(T5));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3) + sizeof(T4) + sizeof(T5) + sizeof(T6));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3) + sizeof(T4) + sizeof(T5) + sizeof(T6) + sizeof(T7));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3) + sizeof(T4) + sizeof(T5) + sizeof(T6) + sizeof(T7) + sizeof(T8));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3) + sizeof(T4) + sizeof(T5) + sizeof(T6) + sizeof(T7) + sizeof(T8) + sizeof(T9));
}

template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6, typename T7, typename T8, typename T9, typename T10>
void *GetStdcallFunc(void *lib, const char *name)
{
	return _GetStdcallFunc(lib, name, sizeof(T1) + sizeof(T2) + sizeof(T3) + sizeof(T4) + sizeof(T5) + sizeof(T6) + sizeof(T7) + sizeof(T8) + sizeof(T9) + sizeof(T10));
}


bool FreeSharedLibrary(void *hDLL)
{
#if defined(_MSC_VER) // Microsoft compiler
	return FreeLibrary((HINSTANCE) hDLL) != 0;
#elif defined(__GNUC__) // GNU compiler
	return dlclose(hDLL);
#endif
}


#endif //os_call_h

