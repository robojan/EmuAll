#include "../../util/memDbg.h"



#include "GbCpu.h"
#include "../defines.h"
#include "../../util/log.h"
#include "../../util/exceptions.h"
#include "../../util/Endian.h"
#include "Gbopcodes.h"
#include "GbInterrupt.h"

GbCpu::GbCpu(Gameboy *gb) : 
	_gb(gb), _stopped(false), _halted(false), _ime(false), 
	_regAF(0), _regBC(0), _regDE(0), _regHL(0), _regSP(0), _regPC(0),
	_tim_enabled(false), _dividerClocks(FCPU / FDIV), 
	_timerClocks(TIMER_CLK0_CLKS * 2), _runningMode(GbCpuRunningMode::Normal)
{
	
}

GbCpu::~GbCpu()
{

}

bool GbCpu::Tick()
{
	bool executed = false;
	_div_counter++;
	if (_doubleSpeed)
	{
		_tim_counter += 1; // double speed
	}
	else {
		_tim_counter += 2; // single speed
	}
	if (_cycles == 0 || _halted || _stopped)
	{
		if (!(_halted || _stopped))
		{
			handleInstruction();
			executed = true;
		}
		this->interrupt();
	}
	if (!(_halted || _stopped))
	{
		_cycles--;
	}
	if ((_div_counter >= _dividerClocks))
	{
		this->_gb->_mem->write(DIV, this->_gb->_mem->read(DIV) + 1, false);
		_div_counter = 0;
	}
	if ((_tim_counter >= _timerClocks) && (_tim_enabled))
	{
		_tim_counter = 0;
		gbByte tima = this->_gb->_mem->read(TIMA);
		tima++;
		this->_gb->_mem->write(TIMA, tima, false);
		if (tima == 0x00) // overflow
		{
			this->_gb->_mem->write(IF, this->_gb->_mem->read(IF) | INT_TIMER);
			this->_gb->_mem->write(TIMA, this->_gb->_mem->read(TMA));
		}
	}
	if (_runningMode == GbCpuRunningMode::Step && _cycles == 1)
		throw  BreakPointException(_regPC);
	return executed;
}

void GbCpu::registerEvents(void)
{
	this->_gb->_mem->registerEvent(DIV, this);
	this->_gb->_mem->registerEvent(TAC, this);
}

void GbCpu::MemEvent(address_t address, gbByte val)
{
	switch(address)
	{
	case DIV:
		this->_gb->_mem->write(address, 0, false);
		break;
	case TAC: // Timer frequency clks times 2 for double speed
		switch(val & TAC_CLK)
		{
		case 0:
			_timerClocks = TIMER_CLK0_CLKS*2; 
			break;
		case 1:
			_timerClocks = TIMER_CLK1_CLKS*2;
			break;
		case 2:
			_timerClocks = TIMER_CLK2_CLKS*2;
			break;
		case 3:
			_timerClocks = TIMER_CLK3_CLKS*2;
			break;
		}
		_tim_enabled = (val & TAC_EN) != 0;
		break;
	default:
		return;
	}
}

void GbCpu::reset(address_t address)
{
	// reset all the registers
	_regAF = 0;
	_regBC = 0;
	_regDE = 0;
	_regHL = 0;

	_halted = false;
	_ime = false;
	_doubleSpeed = false;
	_regSP=0;
	_div_counter = 0;
	_tim_counter = 0;
	_cycles = 0;
	// set the Program counter to the reset address
	_regPC=address;
}

void GbCpu::bootup(void)
{
	// Setup the stack
	if(this->_gb->_mem->read(CGB) & 0x80)
	{
		_regSP = 0xFFFE;
		_regA = 0x11;
		_regF = 0x80;
		_regB = 0x00;
		_regC = 0x00;
		_regD = 0xFF;
		_regE = 0x56;
		_regH = 0x00;
		_regL = 0x0D;
	} else {
		_regA = 0x01;
		_regF = 0xB0;
		_regB = 0x00;
		_regC = 0x13;
		_regD = 0x00;
		_regE = 0xd8;
		_regH = 0x01;
		_regL = 0x4d;
	}
	this->_gb->_mem->write(0xFF05, 0x00);
	this->_gb->_mem->write(0xFF06, 0x00);
	this->_gb->_mem->write(0xFF07, 0x00);
	this->_gb->_mem->write(0xFF10, 0x80);
	this->_gb->_mem->write(0xFF11, 0xBF);
	this->_gb->_mem->write(0xFF12, 0xF3);
	this->_gb->_mem->write(0xFF14, 0xBF);
	this->_gb->_mem->write(0xFF16, 0x3F);
	this->_gb->_mem->write(0xFF17, 0x00);
	this->_gb->_mem->write(0xFF19, 0xBF);
	this->_gb->_mem->write(0xFF1A, 0x7F);
	this->_gb->_mem->write(0xFF1B, 0xFF);
	this->_gb->_mem->write(0xFF1C, 0x9F);
	this->_gb->_mem->write(0xFF1E, 0xBF);
	this->_gb->_mem->write(0xFF20, 0xFF);
	this->_gb->_mem->write(0xFF21, 0x00);
	this->_gb->_mem->write(0xFF22, 0x00);
	this->_gb->_mem->write(0xFF23, 0xBF);
	this->_gb->_mem->write(0xFF24, 0x77);
	this->_gb->_mem->write(0xFF25, 0xF3);
	this->_gb->_mem->write(0xFF26, 0xF0);
	this->_gb->_mem->write(0xFF40, 0x91);
	this->_gb->_mem->write(0xFF42, 0x00);
	this->_gb->_mem->write(0xFF43, 0x00);
	this->_gb->_mem->write(0xFF45, 0x00);
	this->_gb->_mem->write(0xFF47, 0xFC);
	this->_gb->_mem->write(0xFF48, 0xFF);
	this->_gb->_mem->write(0xFF49, 0xFF);
	this->_gb->_mem->write(0xFF4A, 0x00);
	this->_gb->_mem->write(0xFF4B, 0x00);
	this->_gb->_mem->write(0xFF51, 0xFF);
	this->_gb->_mem->write(0xFF52, 0xFF);
	this->_gb->_mem->write(0xFF53, 0xFF);
	this->_gb->_mem->write(0xFF54, 0xFF);
	this->_gb->_mem->write(0xFF55, 0xFF);
	this->_gb->_mem->write(0xFFFF, 0x00);
	// Clear the this->gb->memory 0x8000 to 0x9FFF
	// Setup audio
	// Setup BG palette
	// Convert and load logo into video RAM
	// Load additional bytes in video ram
	// setup background tilemap
	// scroll logo and play logo sound
	// initialize scroll count
	// set loop count 0x64 
	// set vertical scroll register
	// turn on LCD, showing background
	// set B=1

	// wait for screen frame
	// increment scroll count
	// 0x62 counts in, play sound #1
	// 0x64 counts in, play sound #2
	// play sound
	// scroll logo up if B=1
	// set b = 0 first time
	// use scrolling loop to pause
}

uint8_t GbCpu::GetRegister(int id) const
{
	switch (id)
	{
	case REG_B: return _regB;
	case REG_C: return _regC;
	case REG_D: return _regD;
	case REG_E: return _regE;
	case REG_H: return _regH;
	case REG_L: return _regL;
	case REG_F: return _regF;
	case REG_A: return _regA;
	}
	return 0;
}

uint16_t GbCpu::GetRegisterPair(int id) const
{
	switch (id)
	{
	case REG_BC: return _regBC;
	case REG_DE: return _regDE;
	case REG_HL: return _regHL;
	case REG_SP: return _regSP;
	case REG_PC: return _regPC;
	}
	return 0;
}

static const uint32_t StateCPUid = 0x43505520;

// 0 - id
// 4 - size
// 8 - regAF
// 10 - regBC
// 12 - regDE
// 14 - regHL
// 16 - regSP
// 18 - regPC
// 20 - runningMode
// 21 - ime << 4 | stopped << 3 | halted << 2 | doubleSpeed << 1 | m_tim_enabled << 0
// 22 - cycles
// 26 - m_div_counter
// 30 - m_tim_counter
// 34 - timerClocks
// 38 - dividerClocks
// 42 -

bool GbCpu::LoadState(const SaveData_t *data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	uint8_t *ptr = (uint8_t *)data->miscData;
	int miscLen = data->miscDataLen;
	// Find cpu segment
	while (miscLen >= 8) {
		uint32_t id = conv->convu32(*(uint32_t *)(ptr + 0));
		uint32_t len = conv->convu32(*(uint32_t *)(ptr + 4));
		if (id == StateCPUid && len >= 42) {
			_regAF = conv->convu16(*(uint16_t *)(ptr + 8));
			_regBC = conv->convu16(*(uint16_t *)(ptr + 10));
			_regDE = conv->convu16(*(uint16_t *)(ptr + 12));
			_regHL = conv->convu16(*(uint16_t *)(ptr + 14));
			_regSP = conv->convu16(*(uint16_t *)(ptr + 16));
			_regPC = conv->convu16(*(uint16_t *)(ptr + 18));
			_runningMode = (GbCpuRunningMode)ptr[20];
			_tim_enabled = (ptr[21] & 0x01) != 0;
			_doubleSpeed = (ptr[21] & 0x02) != 0;
			_halted = (ptr[21] & 0x04) != 0;
			_stopped = (ptr[21] & 0x08) != 0;
			_ime = (ptr[21] & 0x10) != 0;
			_cycles = conv->convi32(*(int32_t *)(ptr + 22));
			_div_counter = conv->convi32(*(int32_t *)(ptr + 26));
			_tim_counter = conv->convi32(*(int32_t *)(ptr + 30));
			_timerClocks = conv->convi32(*(int32_t *)(ptr + 34));
			_dividerClocks = conv->convi32(*(int32_t *)(ptr + 38));
			return true;
		}
		ptr += len;
		miscLen -= len;
	}
	return false;
}

bool GbCpu::SaveState(std::vector<uint8_t> &data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	int dataLen = 42;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;
	*(uint32_t *)(ptr + 0) = conv->convu32(StateCPUid);
	*(uint32_t *)(ptr + 4) = conv->convu32(dataLen);
	*(uint16_t *)(ptr + 8) = conv->convu16(_regAF);
	*(uint16_t *)(ptr + 10) = conv->convu16(_regBC);
	*(uint16_t *)(ptr + 12) = conv->convu16(_regDE);
	*(uint16_t *)(ptr + 14) = conv->convu16(_regHL);
	*(uint16_t *)(ptr + 16) = conv->convu16(_regSP);
	*(uint16_t *)(ptr + 18) = conv->convu16(_regPC);
	ptr[20] = (uint8_t)_runningMode;
	ptr[21] =
		(_tim_enabled ? 0x01 : 0x00) |
		(_doubleSpeed ? 0x02 : 0x00) |
		(_halted ? 0x04 : 0x00) |
		(_stopped ? 0x08 : 0x00) |
		(_ime ? 0x10 : 0x00);
	*(int32_t *)(ptr + 22) = conv->convi32(_cycles);
	*(int32_t *)(ptr + 26) = conv->convi32(_div_counter);
	*(int32_t *)(ptr + 30) = conv->convi32(_tim_counter);
	*(int32_t *)(ptr + 34) = conv->convi32(_timerClocks);
	*(int32_t *)(ptr + 38) = conv->convi32(_dividerClocks);
	return true;
}

