#ifndef _GBINTERRUPT_H
#define _GBINtERRUPT_H

inline void GbCpu::interrupt()
{
	unsigned char intF = this->gb->_mem->read(IF);
	if((intF == 0 || (ime == false))&&!(stopped || halted)) return;
	if(ime == false && (halted == true || stopped == true) && intF)
	{
		halted = false;
		cycles = 1;
		return;
	}
	unsigned char intE = this->gb->_mem->read(IE);
	if(intE & intF & INT_VBLANK) // vblank interrupt
	{
		ime = false;
		halted = false;
		stopped = false;
		cycles = 1;
		this->gb->_mem->write(IF, intF & ~INT_VBLANK);
		this->gb->_mem->write(regSP - 1, regPC >> 8);
		this->gb->_mem->write(regSP - 2, regPC & 0xFF);
		regSP-= 2;
		regPC = INT_VBLANK_ADDR;
	} else if(intE & intF & INT_LCDSTAT) // lcd status interrupt
	{
		ime = false;
		halted = false;
		stopped = false;
		cycles = 1;
		this->gb->_mem->write(IF, intF & ~INT_LCDSTAT);
		this->gb->_mem->write(regSP - 1, regPC >> 8);
		this->gb->_mem->write(regSP - 2, regPC & 0xFF);
		regSP-= 2;
		regPC = INT_LCDSTAT_ADDR;
	} else if(intE & intF & INT_TIMER) // timer interrupt
	{
		ime = false;
		halted = false;
		stopped = false;
		cycles = 1;
		this->gb->_mem->write(IF, intF & ~INT_TIMER);
		this->gb->_mem->write(regSP - 1, regPC >> 8);
		this->gb->_mem->write(regSP - 2, regPC & 0xFF);
		regSP-= 2;
		regPC = INT_TIMER_ADDR;
	} else if(intE & intF & INT_SERIAL) // vblank interrupt
	{
		ime = false;
		halted = false;
		stopped = false;
		cycles = 1;
		this->gb->_mem->write(IF, intF & ~INT_SERIAL);
		this->gb->_mem->write(regSP - 1, regPC >> 8);
		this->gb->_mem->write(regSP - 2, regPC & 0xFF);
		regSP-= 2;
		regPC = INT_SERIAL_ADDR;
	} else if(intE & intF & INT_JOYPAD) // vblank interrupt
	{
		ime = false;
		halted = false;
		stopped = false;
		cycles = 1;
		this->gb->_mem->write(IF, intF & ~INT_JOYPAD);
		this->gb->_mem->write(regSP - 1, regPC >> 8);
		this->gb->_mem->write(regSP - 2, regPC & 0xFF);
		regSP-= 2;
		regPC = INT_JOYPAD_ADDR;
	}
}

#endif