#include <string>
#include <algorithm>

#include <emu.h>
#include <GBAemu/gba.h>
#include <GBAemu/util/preprocessor.h>
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

uint8_t __stdcall Disassemble(EMUHANDLE handle, uint32_t pos, const uint8_t **raw, const uint8_t **instr) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return -1;
	*(char **)raw = "00000000";
	*(char **)instr = "Not implemented";
	return 4;
}

uint8_t __stdcall GetMemoryData(EMUHANDLE handle, int32_t memory, uint32_t address) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return 0;
	switch (memory) {
	case 2000: return emulator->GetMemory().Read8(address);
	case 2001: return emulator->GetMemory().ReadBios8(address);
	case 2002: return emulator->GetMemory().ReadWRAM8(address);
	case 2003: return emulator->GetMemory().ReadChipWRAM8(address);
	case 2004: return emulator->GetMemory().ReadRegister8(address);
	case 2005: return emulator->GetMemory().ReadPRAM8(address);
	case 2006: return emulator->GetMemory().ReadVRAM8(address);
	case 2007: return emulator->GetMemory().ReadORAM8(address);
	case 2008: return emulator->GetMemory().ReadSRAM8(address);
	case 2010: return emulator->GetMemory().ReadROM8(address);
	default: return 0;
	}
}

int32_t __stdcall GetValI(EMUHANDLE handle, int32_t id) {
	return GetValU(handle, id);
}

uint32_t __stdcall GetValU(EMUHANDLE handle, int32_t id) {
	Gba *emulator = (Gba *)handle;
	if (emulator == NULL)
		return 0;
	switch (id) {
	CASE_RANGE16(1000)
	case 1016:
	case 1017:
		return emulator->GetCpu().GetRegisterValue(id - 1000);
	case 2009: // SRAM size
		return emulator->GetMemory().GetSRAMSize();
	case 2011: // ROM size
		return emulator->GetMemory().GetRomSize();
	default:
		return 0;
	}
}