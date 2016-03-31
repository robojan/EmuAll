#ifndef _GAMEBOY_H
#define _GAMEBOY_H

// Forward declaration
class GbMem;
class GbCpu;
class GbGpu;
class GbInput;
class GbSound;
class GbDis;

#include <string>
#include <map>
#include <inttypes.h>
#include <emu.h>

typedef uint_fast8_t	gbByte;
typedef gbByte*			bank_t;
typedef uint_fast16_t	address_t;

typedef struct {
	uint8_t *data;
	uint32_t length;
} rom_t;

typedef struct {
	uint8_t	*ptr;
	gbByte	org;
} breakpoint_t;


class Gameboy
{
	friend class GbDis;
	friend class GbMem;
	friend class GbCpu;
	friend class GbGpu;
	friend class GbInput;
	friend class GbSound;
	friend class GbChannel12;
	friend class GbChannel3;
	friend class GbChannel4;
public:
	Gameboy();
	~Gameboy();
	bool Init();
	void Step(void);
	void Run(bool run = true);
	inline bool IsRunning(void) const { return _running; }
	bool Tick(uint32_t time);
	void Reshape(int id, int width, int height, bool keepAspect);
	bool Input(int key, int pressed);
	bool Load(const SaveData_t *data);
	bool Save(SaveData_t *fileName);
	bool LoadState(const SaveData_t *data);
	bool SaveState(SaveData_t *data);
	bool Exit(void);
	GbCpu *GetCpu() const;
	GbMem *GetMem() const;
	GbGpu *GetGpu() const;
	GbSound *GetSound() const;
	char GetInstruction(address_t pos, const char **raw, const char **instr) const;
	void AddBreakpoint(address_t address);
	void RemoveBreakpoint(address_t address);
	bool IsBreakpoint(uint32_t address) const;
	bool IsGBC() const;

private:
	GbMem	*_mem;
	GbCpu	*_cpu;
	GbGpu	*_gpu;
	GbInput	*_input;
	GbSound *_sound;
	GbDis	*_disassembler;
	rom_t	_rom;
	std::map<address_t, breakpoint_t> _breakpoints;
	breakpoint_t _continuePoint;
	bool	_gbc;
	bool	_sgb;
	bool	_running;

};

#endif