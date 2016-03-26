#ifndef _GBCPU_H
#define _GBCPU_H
#ifdef __WXMSW__
    #include <wx/msw/msvcrt.h>      // redefines the new() operator 
#endif

#include "../mem/GbMem.h"
#include <stdint.h>
#include "../Gameboy.h"
#include <vector>

enum class GbCpuRunningMode
{
	Normal,
	Step,
	StepOver,
	StepOut
};

class GbCpu : public GbMemEvent
{
	friend class Gameboy;
public:	
	GbCpu(Gameboy *master);
	~GbCpu();
	
	bool Tick();
	void reset(address_t address);
	void bootup(void);
	inline void interrupt(void);
	inline void handleInstruction(void);
	void registerEvents(void);
	void MemEvent(address_t address, gbByte val);
	inline bool InterruptsEnabled() const { return _ime; }
	inline bool IsHalted() const { return _halted; }
	inline bool IsStopped() const { return _stopped; }
	uint8_t GetRegister(int id) const;
	uint16_t GetRegisterPair(int id) const;
	GbCpuRunningMode GetRunningMode() const { return _runningMode; }
	void SetRunningMode(GbCpuRunningMode mode) { _runningMode = mode; }

	bool LoadState(const SaveData_t *data);
	bool SaveState(std::vector<uint8_t> &data);

private:

	inline uint8_t GetOP() { 
		uint8_t ret = _gb->_mem->read(_regPC); 
		_regPC+=1; 
		return ret;
	}

	inline uint16_t GetNN() {
		uint16_t ret = _gb->_mem->read(_regPC) | _gb->_mem->read(_regPC + 1) << 8;
		_regPC += 2;
		return ret;
	}

	
	union{
		struct {
			uint8_t _regF;
			uint8_t _regA;
		};
		uint16_t _regAF;
	};
	union{
		struct {
			uint8_t _regC;
			uint8_t _regB;
		};
		uint16_t _regBC;
	};
	union{
		struct {
			uint8_t _regE;
			uint8_t _regD;
		};
		uint16_t _regDE;
	};
	union{
		struct {
			uint8_t _regL;
			uint8_t _regH;
		};
		uint16_t _regHL;
	};
	uint16_t _regSP;
	uint16_t _regPC;

	GbCpuRunningMode _runningMode;
	Gameboy		*_gb;
	int			_cycles;
	bool		_ime;
	bool		_stopped;
	bool		_halted;
	bool		_doubleSpeed;
	bool		_tim_enabled;
	int			_div_counter;
	int			_tim_counter;
	
	int			_timerClocks;
	int			_dividerClocks;
};

//#include "GbOpcodes.h"
//#include "GbInterrupt.h"

#endif