
#include <GL/glew.h>
#include <GL/wglew.h>

#include <string>
#include <algorithm>
#include <cassert>

#include <emu.h>
#include <GBAemu/gba.h>
#include <GBAemu/defines.h>
#include <GBAemu/util/preprocessor.h>
#include <GBAemu/cpu/armException.h>
#include "resources/resources.h"

void(*Log)(enum loglevel, char *, ...);

void __stdcall InitPlugin()
{
	// Initialize GLEW
	glewExperimental = true;
	GLenum result = glewInit();
	assert(result == GLEW_OK);
}

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

void __stdcall Step(EMUHANDLE handle) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return;
	return emulator->Step();
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

uint32_t __stdcall InitGL(EMUHANDLE handle, int32_t id) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return 0;
	return emulator->GetGpu().InitGL(id) ? 1 : 0;
}
void __stdcall DestroyGL(EMUHANDLE handle, int32_t id) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return;
	emulator->GetGpu().DestroyGL(id);
}

void __stdcall Reshape(EMUHANDLE handle, int32_t id, int32_t width, int32_t height, int32_t keepAspect)
{
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return;
	emulator->GetGpu().Reshape(id, width, height, keepAspect != 0);
}

void __stdcall Draw(EMUHANDLE handle, int32_t id) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return;
	emulator->GetGpu().Draw(id);
}

uint8_t __stdcall Disassemble(EMUHANDLE handle, uint32_t pos, const uint8_t **raw, const uint8_t **instr) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return -1;

	try {
		return emulator->GetDisassembler().Disassemble(pos, (const char **)raw, (const char **)instr);
	}
	catch (DataAbortARMException &) {
		*((const char **)raw) = "xxxx";
		*((const char **)instr) = "DataAbort exception";
		emulator->Run(false);
		return 2;
	}
}

uint8_t __stdcall GetMemoryData(EMUHANDLE handle, int32_t memory, uint32_t address) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return 0;
	try {
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
	catch (DataAbortARMException &) {
		return 0x00;
	}
}

void __stdcall AddBreakpoint(EMUHANDLE handle, uint32_t pos) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return;
	emulator->GetCpu().AddBreakpoint(pos);
}

void __stdcall RemoveBreakpoint(EMUHANDLE handle, uint32_t pos) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return;
	emulator->GetCpu().RemoveBreakpoint(pos);
}

int32_t __stdcall IsBreakpoint(EMUHANDLE handle, uint32_t pos) {
	Gba *emulator = reinterpret_cast<Gba *>(handle);
	if (emulator == nullptr)
		return 0;
	return emulator->GetCpu().IsBreakpoint(pos) ? 1 : 0;
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
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return emulator->GetCpu().GetRegisterValue(id - 1000);
	case 1018:
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return (emulator->GetCpu().GetRegisterValue(16) & CPSR_N_MASK) != 0 ? 1 : 0;
	case 1019:
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return (emulator->GetCpu().GetRegisterValue(16) & CPSR_Z_MASK) != 0 ? 1 : 0;
	case 1020:
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return (emulator->GetCpu().GetRegisterValue(16) & CPSR_C_MASK) != 0 ? 1 : 0;
	case 1021:
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return (emulator->GetCpu().GetRegisterValue(16) & CPSR_V_MASK) != 0 ? 1 : 0;
	case 1022:
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return (emulator->GetCpu().GetRegisterValue(16) & CPSR_T_MASK) != 0 ? 1 : 0;
	case 1023:
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return (emulator->GetCpu().GetRegisterValue(16) & CPSR_I_MASK) != 0 ? 1 : 0;
	case 1024:
		emulator->GetCpu().SaveHostFlagsToCPSR();
		return (emulator->GetCpu().GetRegisterValue(16) & CPSR_F_MASK) != 0 ? 1 : 0;
	case 1025:
		if (emulator->GetCpu().IsInThumbMode()) {
			uint32_t pc = emulator->GetCpu().GetRegisterValue(REGPC);
			if (emulator->GetCpu().IsStalled()) {
				return pc;
			}
			else {
				return pc - 2;
			}
		}
		else {
			uint32_t pc = emulator->GetCpu().GetRegisterValue(REGPC);
			if (emulator->GetCpu().IsStalled()) {
				return pc;
			}
			else {
				return pc - 4;
			}
		}
	case 2009: // SRAM size
		return emulator->GetMemory().GetSRAMSize();
	case 2011: // ROM size
		return emulator->GetMemory().GetRomSize();
	case 3000: // BG mode
		return emulator->GetMemory().Read32(DISPCNT) & 0x7;
	case 3001: // Active frame
		return (emulator->GetMemory().Read32(DISPCNT) >> 4) & 0x1;
	case 3002: // CGB mode
		return (emulator->GetMemory().Read32(DISPCNT) & 0x8) ? 1 : 0;
	case 3003: // H-Blank interval free
		return (emulator->GetMemory().Read32(DISPCNT) & 0x20) ? 1 : 0;
	case 3004: // OBJ Character VRAM Mapping
		return (emulator->GetMemory().Read32(DISPCNT) & 0x40) ? 1 : 0;
	case 3005: // Forced blank
		return (emulator->GetMemory().Read32(DISPCNT) & 0x80) ? 1 : 0;
	case 3006: // BG0 Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x100) ? 1 : 0;
	case 3007: // BG1 Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x200) ? 1 : 0;
	case 3008: // BG2 Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x400) ? 1 : 0;
	case 3009: // BG3 Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x800) ? 1 : 0;
	case 3010: // OBJ Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x1000) ? 1 : 0;
	case 3011: // Window 0 Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x2000) ? 1 : 0;
	case 3012: // Window 1 Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x4000) ? 1 : 0;
	case 3013: // OBJ window Enabled
		return (emulator->GetMemory().Read32(DISPCNT) & 0x8000) ? 1 : 0;
	case 3014: // V-Blank IRQ
		return (emulator->GetMemory().Read32(DISPSTAT) & 0x1) ? 1 : 0;
	case 3015: // H-Blank IRQ
		return (emulator->GetMemory().Read32(DISPSTAT) & 0x2) ? 1 : 0;
	case 3016: // V-Match IRQ
		return (emulator->GetMemory().Read32(DISPSTAT) & 0x4) ? 1 : 0;
	case 3017: // V-Blank IE
		return (emulator->GetMemory().Read32(DISPSTAT) & 0x8) ? 1 : 0;
	case 3018: // H-Blank IE
		return (emulator->GetMemory().Read32(DISPSTAT) & 0x10) ? 1 : 0;
	case 3019: // V-Match IE
		return (emulator->GetMemory().Read32(DISPSTAT) & 0x20) ? 1 : 0;
	case 3020: // V-Match
		return (emulator->GetMemory().Read32(DISPSTAT) >> 8) & 0xFF;
	case 3021: // BG0CNT
		return (emulator->GetMemory().Read16(BG0CNT));
	case 3022: // BG0HOFS
		return (emulator->GetMemory().Read16(BG0HOFS));
	case 3023: // BG0VOFS
		return (emulator->GetMemory().Read16(BG0VOFS));
	case 3024: // BG1CNT
		return (emulator->GetMemory().Read16(BG1CNT));
	case 3025: // BG1HOFS
		return (emulator->GetMemory().Read16(BG1HOFS));
	case 3026: // BG1VOFS
		return (emulator->GetMemory().Read16(BG1VOFS));
	case 3027: // BG2CNT
		return (emulator->GetMemory().Read16(BG2CNT));
	case 3028: // BG2HOFS
		return (emulator->GetMemory().Read16(BG2HOFS));
	case 3029: // BG2VOFS
		return (emulator->GetMemory().Read16(BG2VOFS));
	case 3030: // BG3CNT
		return (emulator->GetMemory().Read16(BG3CNT));
	case 3031: // BG3HOFS
		return (emulator->GetMemory().Read16(BG3HOFS));
	case 3032: // BG3VOFS
		return (emulator->GetMemory().Read16(BG3VOFS));
	case 3033: // BG2X
		return (emulator->GetMemory().Read32(BG2X_L));
	case 3034: // BG2Y
		return (emulator->GetMemory().Read32(BG2Y_L));
	case 3035: // BG2PA
		return (emulator->GetMemory().Read16(BG2PA));
	case 3036: // BG2PB
		return (emulator->GetMemory().Read16(BG2PB));
	case 3037: // BG2PC
		return (emulator->GetMemory().Read16(BG2PC));
	case 3038: // BG2PD
		return (emulator->GetMemory().Read16(BG2PD));
	case 3039: // BG3X
		return (emulator->GetMemory().Read32(BG3X_L));
	case 3040: // BG3Y
		return (emulator->GetMemory().Read32(BG3Y_L));
	case 3041: // BG3PA
		return (emulator->GetMemory().Read16(BG3PA));
	case 3042: // BG3PB
		return (emulator->GetMemory().Read16(BG3PB));
	case 3043: // BG3PC
		return (emulator->GetMemory().Read16(BG3PC));
	case 3044: // BG3PD
		return (emulator->GetMemory().Read16(BG3PD));
	case 3050: // bit depth tiles 1
		return (emulator->GetGpu()._debugTiles8BitDepth[0] ? 0 : 1);
	case 3051: // bit depth tiles 2
		return (emulator->GetGpu()._debugTiles8BitDepth[1] ? 0 : 1);
	case 3052: // bit depth obj tiles
		return (emulator->GetGpu()._debugTiles8BitDepth[2] ? 0 : 1);
	case 3053: // grid enabled tiles 1
		return (emulator->GetGpu()._debugTilesGridEnabled[0] ? 1 :0);
	case 3054: // grid enabled tiles 2
		return (emulator->GetGpu()._debugTilesGridEnabled[1] ? 1 : 0);
	case 3055: // grid enabled obj tiles
		return (emulator->GetGpu()._debugTilesGridEnabled[2] ? 1 : 0);
	default:
		return 0;
	}
}

void __stdcall SetValI(EMUHANDLE handle, int32_t id, int32_t val) {
	SetValU(handle, id, val);
}

void __stdcall SetValU(EMUHANDLE handle, int32_t id, uint32_t val) {
	Gba *emulator = (Gba *)handle;
	if (emulator == NULL)
		return;
	switch (id) {
	case 3050: // bit depth tiles 1
		emulator->GetGpu()._debugTiles8BitDepth[0] = (val == 0);
		break;
	case 3051: // bit depth tiles 2
		emulator->GetGpu()._debugTiles8BitDepth[1] = (val == 0);
		break;
	case 3052: // bit depth obj tiles
		emulator->GetGpu()._debugTiles8BitDepth[2] = (val == 0);
		break;
	case 3053: // grid enabled tiles 1
		emulator->GetGpu()._debugTilesGridEnabled[0] = (val != 0);
		break;
	case 3054: // grid enabled tiles 2
		emulator->GetGpu()._debugTilesGridEnabled[1] = (val != 0);
		break;
	case 3055: // grid enabled obj tiles
		emulator->GetGpu()._debugTilesGridEnabled[2] = (val != 0);
		break;
	}
}
