#include "util/memDbg.h"

#include <emu.h>
#include "util/log.h"
#include <emuall/util/Endian.h>
#include "gb/Gameboy.h"
#include "gb/defines.h"
#include <algorithm>
#include <assert.h>
#include "gb/cpu/GbCpu.h"
#include "gb/gpu/GbGpu.h"
#include "resources/resources.h"
#include <GL/glew.h>

void(*Log)(enum loglevel, char *, ...);

void __stdcall InitPlugin()
{
	// Initialize GLEW
	glewExperimental = true;
	GLenum result = glewInit();
	if (result != GLEW_OK) {
		Log(Error, "GLEW could not be initialized: %s", glewGetErrorString(result));
	}
}

int32_t __stdcall GetValI(EMUHANDLE handle, int32_t id)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (emulator == NULL)
		return 0;
	//switch (id) // See description XML for ids
	//{
	//default:
		return GetValU(handle, id); // default try unsigned values
	//}
}

uint32_t __stdcall GetValU(EMUHANDLE handle, int32_t id)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (emulator == NULL)
		return 0;
	if (250 <= id && id <= 325)
	{
		return emulator->GetGpu()->GetPalette((id - 250));
	}
	switch (id) // See description XML for ids
	{
	case 10: // AF
		return emulator->GetCpu()->GetRegister(REG_A) | (emulator->GetCpu()->GetRegister(REG_F) << 8);
	case 11: // BC
		return emulator->GetCpu()->GetRegisterPair(REG_BC);
	case 12: // DE
		return emulator->GetCpu()->GetRegisterPair(REG_DE);
	case 13: // HL
		return emulator->GetCpu()->GetRegisterPair(REG_HL);
	case 14: // SP
		return emulator->GetCpu()->GetRegisterPair(REG_SP);
	case 15: // PC
		return emulator->GetCpu()->GetRegisterPair(REG_PC);
	case 16: // Z
		return (emulator->GetCpu()->GetRegister(REG_F) & F_Z) != 0;
	case 17: // N
		return (emulator->GetCpu()->GetRegister(REG_F) & F_N) != 0;
	case 18: // C
		return (emulator->GetCpu()->GetRegister(REG_F) & F_C) != 0;
	case 19: // H
		return (emulator->GetCpu()->GetRegister(REG_F) & F_H) != 0;
	case 21: // halted
		return emulator->GetCpu()->IsHalted();
	case 22: // Stopped
		return emulator->GetCpu()->IsStopped();
	case 20: // IME
		return emulator->GetCpu()->InterruptsEnabled();
	case 110: // Address space
		return 0x10000;
	case 111: // ROM
		return emulator->GetMem()->GetROMSize();
	case 112: // ERAM
		return emulator->GetMem()->GetRAMSize();
	case 113: // WRAM
		return emulator->IsGBC() ? WRAM_BANK_SIZE * 8 : WRAM_BANK_SIZE * 2;
	case 114: // VRAM
		return emulator->IsGBC() ? VRAM_BANK_SIZE * 2 : VRAM_BANK_SIZE;
	case 121: // ROM bank
		return emulator->GetMem()->GetROMBank();
	case 122: // ERAM bank
		return emulator->GetMem()->GetERAMBank();
	case 123: // WRAM bank
		return emulator->GetMem()->GetWRAMBank();
	case 124: // VRAM bank
		return emulator->GetMem()->GetVRAMBank();
	case 200: // LCDC
		return emulator->GetMem()->read(LCDC);
	case 201: // STAT
		return emulator->GetMem()->read(LCDS);
	case 202: // SCX
		return emulator->GetMem()->read(SCX);
	case 203: // SCY
		return emulator->GetMem()->read(SCY);
	case 204: // LY
		return emulator->GetMem()->read(LY);
	case 205: // LYC
		return emulator->GetMem()->read(LYC);
	case 206: // WY
		return emulator->GetMem()->read(WY);
	case 207: // WX
		return emulator->GetMem()->read(WX);
	case 208: // VRAM bank
		return emulator->GetMem()->read(VBK);
	case 209: // enabled
		return emulator->GetGpu()->IsEnabled();
	case 220: // BG grid
		return emulator->GetGpu()->GetBackgroundDebugInfo()->grid;
	case 221: // BG map
		return emulator->GetGpu()->GetBackgroundDebugInfo()->mapMode;
	case 222: // BG data
		return emulator->GetGpu()->GetBackgroundDebugInfo()->dataMode;
	case 223: // BG number
		return emulator->GetGpu()->GetBackgroundDebugInfo()->number;
	case 224: // BG palette
		return emulator->GetGpu()->GetBackgroundDebugInfo()->palette;
	case 225: // BG hflip
		return emulator->GetGpu()->GetBackgroundDebugInfo()->hflip;
	case 226: // BG vflip
		return emulator->GetGpu()->GetBackgroundDebugInfo()->vflip;
	case 227: // BG priority
		return emulator->GetGpu()->GetBackgroundDebugInfo()->priority;
	case 228: // BG mousex
		return emulator->GetGpu()->GetBackgroundDebugInfo()->mouseX;
	case 229: // BG mousey
		return emulator->GetGpu()->GetBackgroundDebugInfo()->mouseY;
	case 230: // Tiles mouseX
		return emulator->GetGpu()->GetTilesDebugInfo()->mouseX;
	case 231: // Tiles mouseY
		return emulator->GetGpu()->GetTilesDebugInfo()->mouseY;
	case 232: // Tiles VRAM
		return emulator->GetGpu()->GetTilesDebugInfo()->vram;
	case 233: // Tiles VRAM
		return emulator->GetGpu()->GetTilesDebugInfo()->address;
	case 234: // Tiles VRAM
		return emulator->GetGpu()->GetTilesDebugInfo()->number;
	case 240: // OAM mousex
		return emulator->GetGpu()->GetOAMDebugInfo()->mouseX;
	case 241: // OAM mousey
		return emulator->GetGpu()->GetOAMDebugInfo()->mouseY;
	case 242: // OAM x
		return emulator->GetGpu()->GetOAMDebugInfo()->x;
	case 243: // OAM y
		return emulator->GetGpu()->GetOAMDebugInfo()->y;
	case 244: // OAM tile
		return emulator->GetGpu()->GetOAMDebugInfo()->tile;
	case 246: // OAM hflip
		return emulator->GetGpu()->GetOAMDebugInfo()->hflip;
	case 247: // OAM vflip
		return emulator->GetGpu()->GetOAMDebugInfo()->vflip;
	case 248: // OAM priority
		return emulator->GetGpu()->GetOAMDebugInfo()->priority;
	case 249: // OAM vram
		return emulator->GetGpu()->GetOAMDebugInfo()->vram;
	default:
		Log(Warn, "Received unknown ID for get value %d", id);
		return 0;
	}
}

const uint8_t * __stdcall GetString(EMUHANDLE handle, int32_t id)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (emulator == NULL)
		return 0;
	switch (id)
	{
	case 245: // OAM palette
		return (const uint8_t *)emulator->GetGpu()->GetOAMDebugInfo()->palette.c_str();
	default:
		Log(Warn, "Received unknown ID for get string %d", id);
		return NULL;
	}
}

void __stdcall SetValI(EMUHANDLE handle, int32_t id, int32_t val)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (emulator == NULL)
		return;
	const GbCpu *cpu = emulator->GetCpu();
	switch (id)
	{
	case 221: // bg map
		emulator->GetGpu()->GetBackgroundDebugInfo()->mapMode = val;
		emulator->GetGpu()->UpdateBackgroundDebug();
		break;
	case 222: // bg data
		emulator->GetGpu()->GetBackgroundDebugInfo()->dataMode = val;
		emulator->GetGpu()->UpdateBackgroundDebug();
		break;
	case 228: // bg mousex
		emulator->GetGpu()->GetBackgroundDebugInfo()->mouseX = val;
		emulator->GetGpu()->UpdateBackgroundDebug();
		break;
	case 229: // bg mousey
		emulator->GetGpu()->GetBackgroundDebugInfo()->mouseY = val;
		emulator->GetGpu()->UpdateBackgroundDebug();
		break;
	case 230: // tile mousex
		emulator->GetGpu()->GetTilesDebugInfo()->mouseX = val;
		emulator->GetGpu()->UpdateTilesDebug();
		break;
	case 231: // tile mousey
		emulator->GetGpu()->GetTilesDebugInfo()->mouseY = val;
		emulator->GetGpu()->UpdateTilesDebug();
		break;
	case 240: // oam mousex
		emulator->GetGpu()->GetOAMDebugInfo()->mouseX = val;
		emulator->GetGpu()->UpdateOAMDebug();
		break;
	case 241: // oam mousey
		emulator->GetGpu()->GetOAMDebugInfo()->mouseY = val;
		emulator->GetGpu()->UpdateOAMDebug();
		break;
	default:
		Log(Warn, "Received unknown ID for Set value %d", id);
		return;
	}
}

void __stdcall SetValU(EMUHANDLE handle, int32_t id, uint32_t val)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (emulator == NULL)
		return;
	switch (id)
	{
	case 221: // signed numbers
	case 222:
	case 228:
	case 229:
		SetValI(handle, id, val);
		break;
	case 220: // Grid
		emulator->GetGpu()->GetBackgroundDebugInfo()->grid = val != 0;
		break;
	default:
		Log(Warn, "Received unknown ID for Set value %d", id);
		return;
	}
}

EMUHANDLE __stdcall CreateEmulator()
{
	Gameboy *emulator = new Gameboy();
	return emulator;
}

void __stdcall ReleaseEmulator(EMUHANDLE handle)
{
	Gameboy *emulator = (Gameboy *)handle;
	if (emulator == NULL)
		return;
	emulator->Exit();
	delete emulator;
}

int32_t __stdcall Init(EMUHANDLE handle, callBackfunctions_t funcs)
{
	Gameboy *emulator = (Gameboy *) handle;
	Log = funcs.Log;
	if (handle == NULL)
		return false;
	return emulator->Init();
}

int32_t __stdcall Load(EMUHANDLE handle, const SaveData_t *data)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return false;
	return emulator->Load(data);
}

uint32_t __stdcall InitGL(EMUHANDLE handle, int32_t id)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return false;
	return emulator->GetGpu()->InitGL(id) ? 1:0;
}

void __stdcall DestroyGL(EMUHANDLE handle, int32_t id)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->GetGpu()->DestroyGL(id);
}

// Misc functions
const uint8_t *__stdcall GetDescription(uint32_t *size)
{
	if (size != NULL)
	{
		*size = sizeof(resource_gbemu_xml);
	}
	return resource_gbemu_xml;
}

int32_t __stdcall IsCompatible(const uint8_t *filename)
{
	std::string name = (const char*) filename;
	std::transform(name.begin(), name.end(), name.begin(), ::tolower);
	std::string extension = name.substr(name.rfind('.') + 1);
	return extension == "gb" || extension == "gbc" || extension == "cgb";
}

int32_t __stdcall GetInterfaceVersion()
{
	return INTERFACEVERSION;
}

int32_t __stdcall IsRunning(EMUHANDLE handle)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return -1;
	return emulator->IsRunning() ? 1 : 0;
}

void __stdcall Run(EMUHANDLE handle, int32_t run)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->Run(run != 0);
}

void __stdcall Step(EMUHANDLE handle)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->Step();
}

// Running functions

void __stdcall Input(EMUHANDLE handle, int32_t key, int32_t pressed)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->Input(key, pressed);
}

int32_t __stdcall Tick(EMUHANDLE handle, uint32_t time)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return false;
	return emulator->Tick(time);
}

void __stdcall Draw(EMUHANDLE handle, int32_t id)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->GetGpu()->drawGL(id);
}

void __stdcall Reshape(EMUHANDLE handle, int32_t id, int32_t width, int32_t height, int32_t keepAspect)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->Reshape(id, width, height, keepAspect!=0);
}

// stopping functions
int32_t __stdcall Save(EMUHANDLE handle, SaveData_t *data)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return false;
	return emulator->Save(data);
}

int32_t __stdcall SaveState(EMUHANDLE handle, SaveData_t *data)
{
	Gameboy *emulator = (Gameboy *)handle;
	if (handle == NULL)
		return false;
	return emulator->SaveState(data);
}

int32_t __stdcall LoadState(EMUHANDLE handle, const SaveData_t *data)
{
	Gameboy *emulator = (Gameboy *)handle;
	if (handle == NULL)
		return false;
	return emulator->LoadState(data);
}

uint8_t __stdcall Disassemble(EMUHANDLE handle, uint32_t pos, const uint8_t **raw, const uint8_t **instr)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return -1;
	return emulator->GetInstruction(pos, (const char**) raw, (const char**) instr);
}

void __stdcall AddBreakpoint(EMUHANDLE handle, uint32_t pos)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->AddBreakpoint(pos);
}

void __stdcall RemoveBreakpoint(EMUHANDLE handle, uint32_t pos)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return;
	emulator->RemoveBreakpoint(pos);
}

int32_t __stdcall IsBreakpoint(EMUHANDLE handle, uint32_t pos)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL || emulator->GetMem() == NULL)
		return 1 == 0;
	uint32_t unbankedAddress = emulator->GetMem()->GetUnbankedAddress(pos);
	return emulator->IsBreakpoint(unbankedAddress);
}

uint8_t __stdcall GetMemoryData(EMUHANDLE handle, int32_t memory, uint32_t address)
{
	Gameboy *emulator = (Gameboy *) handle;
	if (handle == NULL)
		return 0;
	switch (memory)
	{
	case 0: // Address space
		return emulator->GetMem()->read(address);
	case 1: // ROM
		return emulator->GetMem()->GetROMData(address);
	case 2: // ERAM
		return emulator->GetMem()->GetERAMData(address);
	case 3: // WRAM
		return emulator->GetMem()->GetWRAMData(address);
	case 4: // VRAM
		return emulator->GetMem()->GetVRAMData(address);
	}
	return 0;
}

