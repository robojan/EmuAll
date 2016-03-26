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
	inline bool InterruptsEnabled() const { return ime; }
	inline bool IsHalted() const { return halted; }
	inline bool IsStopped() const { return stopped; }
	uint8_t GetRegister(int id) const;
	uint16_t GetRegisterPair(int id) const;
	GbCpuRunningMode GetRunningMode() const { return runningMode; }
	void SetRunningMode(GbCpuRunningMode mode) { runningMode = mode; }

	bool LoadState(const SaveData_t *data);
	bool SaveState(std::vector<uint8_t> &data);

private:

	inline uint8_t GetOP() { 
		uint8_t ret = gb->_mem->read(regPC); 
		regPC+=1; 
		return ret;
	}

	inline uint16_t GetNN() {
		uint16_t ret = gb->_mem->read(regPC) | gb->_mem->read(regPC + 1) << 8;
		regPC += 2;
		return ret;
	}

	
	union{
		struct {
			uint8_t regF;
			uint8_t regA;
		};
		uint16_t regAF;
	};
	union{
		struct {
			uint8_t regC;
			uint8_t regB;
		};
		uint16_t regBC;
	};
	union{
		struct {
			uint8_t regE;
			uint8_t regD;
		};
		uint16_t regDE;
	};
	union{
		struct {
			uint8_t regL;
			uint8_t regH;
		};
		uint16_t regHL;
	};
	uint16_t regSP;
	uint16_t regPC;

	GbCpuRunningMode runningMode;
	Gameboy		*gb;
	int			cycles;
	bool		ime;
	bool		stopped;
	bool		halted;
	bool		doubleSpeed;
	bool		m_tim_enabled;
	int			m_div_counter;
	int			m_tim_counter;
	
	int			timerClocks;
	int			dividerClocks;
};

//#include "GbOpcodes.h"
//#include "GbInterrupt.h"

#endif