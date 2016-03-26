#ifndef _EMU_H
#define _EMU_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef EMUDLL
#	define EMUEXPORT __declspec(dllexport) 
#else
#	define EMUEXPORT __declspec(dllimport)
#endif

#define INTERFACEVERSION 100

typedef void *EMUHANDLE;
typedef struct
{
	void (*Log)(enum loglevel, char *, ...);
} callBackfunctions_t;

typedef struct
{
	void *romData;
	size_t romDataLen;
	void *ramData;
	size_t ramDataLen;
	void *miscData;
	size_t miscDataLen;
} SaveData_t;

#ifdef EMUDLL

// General functions
EMUEXPORT EMUHANDLE __stdcall CreateEmulator();
EMUEXPORT void __stdcall ReleaseEmulator(EMUHANDLE handle);
EMUEXPORT int32_t __stdcall Init(EMUHANDLE handle, callBackfunctions_t funcs);
EMUEXPORT int32_t __stdcall Load(EMUHANDLE handle, const SaveData_t *data);
EMUEXPORT int32_t  __stdcall Save(EMUHANDLE handle, SaveData_t *data);
EMUEXPORT int32_t __stdcall SaveState(EMUHANDLE handle, SaveData_t *data);
EMUEXPORT int32_t __stdcall LoadState(EMUHANDLE handle, const SaveData_t *data);
EMUEXPORT uint32_t __stdcall InitGL(EMUHANDLE handle, int32_t id);
EMUEXPORT void __stdcall DestroyGL(EMUHANDLE handle, int32_t id);

// Misc functions
EMUEXPORT int32_t __stdcall GetValI(EMUHANDLE handle, int32_t id);
EMUEXPORT uint32_t __stdcall GetValU(EMUHANDLE handle, int32_t id);
EMUEXPORT const uint8_t * __stdcall GetString(EMUHANDLE handle, int32_t id);
EMUEXPORT void __stdcall SetValI(EMUHANDLE handle, int32_t id, int32_t val);
EMUEXPORT void __stdcall SetValU(EMUHANDLE handle, int32_t id, uint32_t val);
EMUEXPORT const uint8_t * __stdcall GetDescription(uint32_t *size);
EMUEXPORT int32_t __stdcall IsCompatible(const uint8_t *filename);
EMUEXPORT int32_t __stdcall GetInterfaceVersion();
EMUEXPORT void __stdcall Run(EMUHANDLE handle, int32_t run);
EMUEXPORT int32_t __stdcall IsRunning(EMUHANDLE handle);

// Running functions
EMUEXPORT void __stdcall Step(EMUHANDLE handle);
EMUEXPORT int32_t __stdcall Tick(EMUHANDLE handle, uint32_t time);
EMUEXPORT void __stdcall Input(EMUHANDLE handle, int32_t key, int32_t pressed);
EMUEXPORT void __stdcall Draw(EMUHANDLE handle, int32_t id);
EMUEXPORT void __stdcall Reshape(EMUHANDLE handle, int32_t width, int32_t height, int32_t keepAspect);
EMUEXPORT void __stdcall GetAudio(EMUHANDLE handle, int32_t source, int16_t *buffer, int32_t samples, int32_t channels, uint32_t samplerate);

// Debugging functions
EMUEXPORT uint8_t __stdcall Disassemble(EMUHANDLE handle, uint32_t pos, const uint8_t **raw, const uint8_t **instr);
EMUEXPORT void __stdcall AddBreakpoint(EMUHANDLE handle, uint32_t pos);
EMUEXPORT void __stdcall RemoveBreakpoint(EMUHANDLE handle, uint32_t pos);
EMUEXPORT int32_t __stdcall IsBreakpoint(EMUHANDLE handle, uint32_t pos);
EMUEXPORT uint8_t __stdcall GetMemoryData(EMUHANDLE handle, int32_t memory, uint32_t address);
#endif

#ifdef __cplusplus
}
#endif

#endif