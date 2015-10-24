#include "../../util/memDbg.h"



#include "GbCpu.h"
#include "../defines.h"
#include "../../util/log.h"
#include "../../util/exceptions.h"
#include "Gbopcodes.h"
#include "GbInterrupt.h"

GbCpu::GbCpu(Gameboy *gb) : 
	gb(gb), stopped(false), halted(false), ime(false), 
	regAF(0), regBC(0), regDE(0), regHL(0), regSP(0), regPC(0),
	m_tim_enabled(false), dividerClocks(FCPU / FDIV), 
	timerClocks(TIMER_CLK0_CLKS * 2), runningMode(GbCpuRunningMode::Normal)
{
	
}

GbCpu::~GbCpu()
{

}

bool GbCpu::Tick()
{
	bool executed = false;
	m_div_counter++;
	if (doubleSpeed)
	{
		m_tim_counter += 1; // double speed
	}
	else {
		m_tim_counter += 2; // single speed
	}
	if (cycles == 0 || halted || stopped)
	{
		if (!(halted || stopped))
		{
			handleInstruction();
			executed = true;
		}
		this->interrupt();
	}
	if (!(halted || stopped))
	{
		cycles--;
	}
	if ((m_div_counter >= dividerClocks))
	{
		this->gb->_mem->write(DIV, this->gb->_mem->read(DIV) + 1, false);
		m_div_counter = 0;
	}
	if ((m_tim_counter >= timerClocks) && (m_tim_enabled))
	{
		m_tim_counter = 0;
		gbByte tima = this->gb->_mem->read(TIMA);
		tima++;
		this->gb->_mem->write(TIMA, tima, false);
		if (tima == 0x00) // overflow
		{
			this->gb->_mem->write(IF, this->gb->_mem->read(IF) | INT_TIMER);
			this->gb->_mem->write(TIMA, this->gb->_mem->read(TMA));
		}
	}
	if (runningMode == GbCpuRunningMode::Step && cycles == 1)
		throw  BreakPointException(regPC);
	return executed;
}

void GbCpu::registerEvents(void)
{
	this->gb->_mem->registerEvent(DIV, this);
	this->gb->_mem->registerEvent(TAC, this);
}

void GbCpu::MemEvent(address_t address, gbByte val)
{
	switch(address)
	{
	case DIV:
		this->gb->_mem->write(address, 0, false);
		break;
	case TAC: // Timer frequency clks times 2 for double speed
		switch(val & TAC_CLK)
		{
		case 0:
			timerClocks = TIMER_CLK0_CLKS*2; 
			break;
		case 1:
			timerClocks = TIMER_CLK1_CLKS*2;
			break;
		case 2:
			timerClocks = TIMER_CLK2_CLKS*2;
			break;
		case 3:
			timerClocks = TIMER_CLK3_CLKS*2;
			break;
		}
		m_tim_enabled = (val & TAC_EN) != 0;
		break;
	default:
		return;
	}
}

void GbCpu::reset(address_t address)
{
	// reset all the registers
	regAF = 0;
	regBC = 0;
	regDE = 0;
	regHL = 0;

	halted = false;
	ime = false;
	doubleSpeed = false;
	regSP=0;
	m_div_counter = 0;
	m_tim_counter = 0;
	cycles = 0;
	// set the Program counter to the reset address
	regPC=address;
}

void GbCpu::bootup(void)
{
	// Setup the stack
	if(this->gb->_mem->read(CGB) & 0x80)
	{
		regSP = 0xFFFE;
		regA = 0x11;
		regF = 0x80;
		regB = 0x00;
		regC = 0x00;
		regD = 0xFF;
		regE = 0x56;
		regH = 0x00;
		regL = 0x0D;
	} else {
		regA = 0x01;
		regF = 0xB0;
		regB = 0x00;
		regC = 0x13;
		regD = 0x00;
		regE = 0xd8;
		regH = 0x01;
		regL = 0x4d;
	}
	this->gb->_mem->write(0xFF05, 0x00);
	this->gb->_mem->write(0xFF06, 0x00);
	this->gb->_mem->write(0xFF07, 0x00);
	this->gb->_mem->write(0xFF10, 0x80);
	this->gb->_mem->write(0xFF11, 0xBF);
	this->gb->_mem->write(0xFF12, 0xF3);
	this->gb->_mem->write(0xFF14, 0xBF);
	this->gb->_mem->write(0xFF16, 0x3F);
	this->gb->_mem->write(0xFF17, 0x00);
	this->gb->_mem->write(0xFF19, 0xBF);
	this->gb->_mem->write(0xFF1A, 0x7F);
	this->gb->_mem->write(0xFF1B, 0xFF);
	this->gb->_mem->write(0xFF1C, 0x9F);
	this->gb->_mem->write(0xFF1E, 0xBF);
	this->gb->_mem->write(0xFF20, 0xFF);
	this->gb->_mem->write(0xFF21, 0x00);
	this->gb->_mem->write(0xFF22, 0x00);
	this->gb->_mem->write(0xFF23, 0xBF);
	this->gb->_mem->write(0xFF24, 0x77);
	this->gb->_mem->write(0xFF25, 0xF3);
	this->gb->_mem->write(0xFF26, 0xF0);
	this->gb->_mem->write(0xFF40, 0x91);
	this->gb->_mem->write(0xFF42, 0x00);
	this->gb->_mem->write(0xFF43, 0x00);
	this->gb->_mem->write(0xFF45, 0x00);
	this->gb->_mem->write(0xFF47, 0xFC);
	this->gb->_mem->write(0xFF48, 0xFF);
	this->gb->_mem->write(0xFF49, 0xFF);
	this->gb->_mem->write(0xFF4A, 0x00);
	this->gb->_mem->write(0xFF4B, 0x00);
	this->gb->_mem->write(0xFF51, 0xFF);
	this->gb->_mem->write(0xFF52, 0xFF);
	this->gb->_mem->write(0xFF53, 0xFF);
	this->gb->_mem->write(0xFF54, 0xFF);
	this->gb->_mem->write(0xFF55, 0xFF);
	this->gb->_mem->write(0xFFFF, 0x00);
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
	case REG_B: return regB;
	case REG_C: return regC;
	case REG_D: return regD;
	case REG_E: return regE;
	case REG_H: return regH;
	case REG_L: return regL;
	case REG_F: return regF;
	case REG_A: return regA;
	}
	return 0;
}

uint16_t GbCpu::GetRegisterPair(int id) const
{
	switch (id)
	{
	case REG_BC: return regBC;
	case REG_DE: return regDE;
	case REG_HL: return regHL;
	case REG_SP: return regSP;
	case REG_PC: return regPC;
	}
	return 0;
}


