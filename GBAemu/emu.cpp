#include <string>
#include <algorithm>

#include <emu.h>
#include <GBAemu/gba.h>
#include "resources/resources.h"

void(*Log)(enum loglevel, char *, ...);

// General functions
const uint8_t * __stdcall GetDescription(uint32_t *size) {
	if (size != nullptr) {
		*size = resource_gbaemu_xml_len;
	}
	return resource_gbaemu_xml;
}

int32_t __stdcall IsCompatible(const uint8_t *filename) {
	std::string name = (const char *)filename;
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	std::string extension = name.substr(name.rfind('.') + 1);
	return extension == "gba";
}

int32_t __stdcall GetInterfaceVersion() {
	return INTERFACEVERSION;
}

EMUHANDLE __stdcall CreateEmulator() {
	return new Gba();
}

void __stdcall ReleaseEmulator(EMUHANDLE handle) {
	if (handle == nullptr)
		return;
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	delete emulator;
}

int32_t __stdcall Init(EMUHANDLE handle, callBackfunctions_t funcs) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	Log = funcs.Log;
	if (emulator == nullptr)
		return 0;
	return emulator->Init();
}

int32_t __stdcall Load(EMUHANDLE handle, const SaveData_t *data) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return 0;
	return emulator->Load(data);
}

int32_t __stdcall Tick(EMUHANDLE handle, uint32_t time) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return 0;
	return emulator->Tick(time);
}

void __stdcall Run(EMUHANDLE handle, int32_t run) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return;
	emulator->Run(run != 0);
}

int32_t __stdcall IsRunning(EMUHANDLE handle) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return 0;
	return emulator->IsRunning() ? 1 : 0;
}