#define _CRT_SECURE_NO_WARNINGS
#include "../util/memDbg.h"

#include "Gameboy.h"
#include "mem/GbMem.h"
#include "cpu/GbCpu.h"
#include "gpu/GbGpu.h"
#include "Sound/GbSound.h"
#include "input/GbInput.h"
#include "debug/gbDis.h"
#include "../util/exceptions.h"

#include <iostream>
#include <fstream>

Gameboy::Gameboy() :
	_mem(NULL), _cpu(NULL), _gpu(NULL), _input(NULL),
	_sound(NULL), _disassembler(NULL), _running(false)
{
	_rom.data = NULL;
	_continuePoint.ptr = NULL;
}

Gameboy::~Gameboy()
{
	Exit();
}

void Gameboy::Step(void)
{
	uint32_t unbankedAddress = _mem->GetUnbankedAddress(_cpu->regPC);
	if (IsBreakpoint(unbankedAddress))
	{
		// Add skip
		*(_breakpoints[unbankedAddress].ptr) = 0xFC;
	}
	_cpu->SetRunningMode(GbCpuRunningMode::Step);
	_running = true;
	Log(Message, "Step once");
}

void Gameboy::Run(bool run)
{
	_running = run;
	if (run)
	{
		_cpu->SetRunningMode(GbCpuRunningMode::Normal);
		_sound->EnableAudio(true);
		Log(Message, "Start running the cpu");
	}
	else {
		_sound->EnableAudio(false);
		Log(Message, "Stop running the cpu");
	}
}

bool Gameboy::Init()
{
	this->Exit();
	_mem = new GbMem(this);
	_cpu = new GbCpu(this);
	_gpu = new GbGpu(this);
	_input = new GbInput(this);
	_sound = new GbSound(this);
	_disassembler = new GbDis(this);
	return true;
}

bool Gameboy::Load(const std::string &fileName)
{
	std::string file = fileName;
	std::ifstream romFile(fileName, std::ios::in | std::ios::binary);
	if(!romFile.is_open())
	{
		Log(Error, "Could not open ROM file: %s", fileName);
		return false;
	}
	if(_rom.data != NULL)
	{
		delete _rom.data;
		_rom.data = NULL;
		_rom.length = 0;
	}
	// Get size of ROM file
	romFile.seekg(0, std::ios::end);
	_rom.length = (int)romFile.tellg();
	romFile.seekg(0, std::ios::beg);
	// Make room for the data and read it
	_rom.data = new uint8_t[_rom.length];
	romFile.read((char *)_rom.data, _rom.length);

	if(romFile.gcount() != _rom.length)
	{
		Log(Error, "Error while reading the file: %s", strerror(errno));
		romFile.close();
		return false;
	}
	romFile.close();
	
	rom_t ram;
	ram.data = NULL;
	ram.length = 0;
	file.append(".sav");
	std::ifstream ramFile(file, std::ios::in | std::ios::binary);
	if(ramFile.is_open())
	{
		// Read the ram file size
		ramFile.seekg(0, std::ios::end);
		ram.length = (int) ramFile.tellg();
		ramFile.seekg(0, std::ios::beg);
		if (ram.length > 0)
		{
			// Allocate the data and read it
			ram.data = new uint8_t[ram.length];
			ramFile.read((char *) ram.data, ram.length);
			if (ramFile.gcount() != ram.length)
			{
				Log(Error, "Error while reading the file: %s", strerror(errno));
				delete[] ram.data;
				ram.data = NULL;
				ram.length = 0;
			}
		}
		ramFile.close();
	}

	if(_mem->load(&_rom, &ram) != true)
	{
		Log(Error, "Could not load the memory");
		delete[] ram.data;
		ram.data = NULL;
		ram.length = 0;
		return false;
	}
	// RAM is not needed anymore
	delete[] ram.data;
	ram.data = NULL;
	ram.length = 0;
	_gpu->registerEvents();
	_cpu->registerEvents();
	_input->registerEvents();
	_sound->registerEvents();

	// Detect modes
	_gbc = (_mem->read(CGB) & 0x80) != 0; // gameboy color mode
	_sgb = _mem->read(SGB) == 0x03 && _mem->read(OLC) == USE_NLC; // Supports super gameboy functions

	_cpu->reset(RESET_ADDRESS);
	_cpu->bootup();

	return true;
}

bool Gameboy::Tick(uint32_t time)
{
	if (!(_running))
		return false;
	try
	{
		_sound->BeginTick();
		int execute = ((uint64_t)time * FCPU + 500000)/1000000;
		if (_cpu->doubleSpeed) // CPU in double speed mode?
		{
			execute = execute * 2;
		}
		int slowTickCounter = execute/4;
		for (int i = execute; i != 0; --i)
		{
			if (_cpu->stopped)
			{
				gbByte key1 = _mem->read(KEY1);
				if (key1 & KEY1_PREP) // if the cpu is stopped and the switch to double frequency is set
				{
					if (key1 & KEY1_CURSPEED) // if the cpu is in double speed
					{
						i = i / 2; // half the execution speed;
						_mem->write(KEY1, (gbByte) 0x00);
						_cpu->doubleSpeed = false;
					}
					else {
						i = i * 2; // double the execution speed;
						_mem->write(KEY1, (gbByte) KEY1_CURSPEED);
						_cpu->doubleSpeed = true;
					}
					// And continue the cpu
					_cpu->stopped = false;
				}
			}
			_cpu->Tick();
			// TODO: add serial port
			if ((_cpu->doubleSpeed && (i & 1)) || !_cpu->doubleSpeed)
			{
				// In double speed this is stil normal
				_gpu->tick();
				// keep sound in sync
				_sound->Tick();
			}
			--slowTickCounter;
			if (slowTickCounter == 0)
			{
				// Update sound
				_sound->SlowTick();
				slowTickCounter = execute / 4;
			}
		}
	}
	catch (const BreakPointException &e)
	{
		if (_cpu->GetRunningMode() == GbCpuRunningMode::Normal)
		{
			Log(Message, "Breakpoint hit at 0x%04X", e.GetAddress());

			breakpoint_t bp = _breakpoints[_mem->GetUnbankedAddress(e.GetAddress())];
			*bp.ptr = 0xFC; // Add continue point;
		}
		_running = false;
		_sound->EnableAudio(false);
		return true;
	}
	
	return false;
}

void Gameboy::Reshape(int width, int height, bool keepAspect)
{
	_gpu->Reshape(width, height, keepAspect);
}

bool Gameboy::Input(int key, int pressed)
{
	if(_input == NULL)
	{
		return true;
	}
	_input->Input(key, pressed != 0);
	return false;
}

bool Gameboy::Save(const std::string &fileName)
{
	bool success = false;
	// save ram
	Run(false);

	std::string file = fileName;

	if (!file.empty() && _mem != NULL)
	{
		file.append(".sav");
		std::ofstream ramFile(file, std::ios::binary | std::ios::out);
		if(ramFile.is_open() == false)
		{
			Log(Error, "Could not save ram file");
			return false;
		}
		success = _mem->Save(ramFile);
		ramFile.close();
	}
	return success;
}

bool Gameboy::Exit(void)
{
	// TODO: add clean exit
	if(_mem != NULL)
	{
		delete _mem;
		_mem = NULL;
	}
	if(_cpu != NULL)
	{
		delete _cpu;
		_cpu = NULL;
	}
	if(_gpu != NULL)
	{
		delete _gpu;
		_gpu = NULL;
	}
	if(_input != NULL)
	{
		delete _input;
		_input = NULL;
	}
	if(_rom.data != NULL)
	{
		delete _rom.data;
		_rom.data = NULL;
	}
	if(_sound != NULL)
	{
		delete _sound;
		_sound = NULL;
	}
	if (_disassembler != NULL)
	{
		delete _disassembler;
		_disassembler = NULL;
	}
	return true;
}


char Gameboy::GetInstruction(address_t pos, const char **raw, const char **instr) const
{
	if (_disassembler == NULL)
		return -1;
	return _disassembler->Disassemble(raw, instr, pos);
}

void Gameboy::AddBreakpoint(address_t address)
{
	if (_mem == NULL)
		return;
	uint32_t unbankedAddress = _mem->GetUnbankedAddress(address);
	if (IsBreakpoint(unbankedAddress))
		return;
	// Get memory location
	breakpoint_t bp;
	address_t realAddress = address;
	if (realAddress >= 0xE000 && realAddress < 0xFEA0) // copy of memory
		realAddress -= 0x2000;
	bp.ptr = &_mem->mem[realAddress / 0x1000][realAddress % 0x1000];
	// Insert breakpoint
	bp.org = *bp.ptr;
	*bp.ptr = 0xFD; // Insert breakpoint
	_breakpoints[unbankedAddress] = bp;
}

void Gameboy::RemoveBreakpoint(address_t address)
{
	if (_mem == NULL) 
		return;
	uint32_t unbankedAddress = _mem->GetUnbankedAddress(address);
	if (!IsBreakpoint(unbankedAddress))
		return;
	// Get breakpoint
	breakpoint_t bp = _breakpoints[unbankedAddress];
	// Restore breakpoint
	*bp.ptr = bp.org;
	_breakpoints.erase(unbankedAddress);
}

bool Gameboy::IsBreakpoint(uint32_t address) const
{
	return _breakpoints.find(address) != _breakpoints.end();
}

GbCpu *Gameboy::GetCpu() const
{
	return _cpu;
}

GbMem *Gameboy::GetMem() const 
{
	return _mem;
}

GbGpu *Gameboy::GetGpu() const
{
	return _gpu;
}

GbSound *Gameboy::GetSound() const
{
	return _sound;
}

bool Gameboy::IsGBC() const
{
	return _gbc;
}