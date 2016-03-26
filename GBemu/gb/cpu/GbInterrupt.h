#ifndef _GBINTERRUPT_H
#define _GBINtERRUPT_H

inline void GbCpu::interrupt()
{
	unsigned char intF = this->_gb->_mem->read(IF);
	if((intF == 0 || (_ime == false))&&!(_stopped || _halted)) return;
	if(_ime == false && (_halted == true || _stopped == true) && intF)
	{
		_halted = false;
		_cycles = 1;
		return;
	}
	unsigned char intE = this->_gb->_mem->read(IE);
	if(intE & intF & INT_VBLANK) // vblank interrupt
	{
		_ime = false;
		_halted = false;
		_stopped = false;
		_cycles = 1;
		this->_gb->_mem->write(IF, intF & ~INT_VBLANK);
		this->_gb->_mem->write(_regSP - 1, _regPC >> 8);
		this->_gb->_mem->write(_regSP - 2, _regPC & 0xFF);
		_regSP-= 2;
		_regPC = INT_VBLANK_ADDR;
	} else if(intE & intF & INT_LCDSTAT) // lcd status interrupt
	{
		_ime = false;
		_halted = false;
		_stopped = false;
		_cycles = 1;
		this->_gb->_mem->write(IF, intF & ~INT_LCDSTAT);
		this->_gb->_mem->write(_regSP - 1, _regPC >> 8);
		this->_gb->_mem->write(_regSP - 2, _regPC & 0xFF);
		_regSP-= 2;
		_regPC = INT_LCDSTAT_ADDR;
	} else if(intE & intF & INT_TIMER) // timer interrupt
	{
		_ime = false;
		_halted = false;
		_stopped = false;
		_cycles = 1;
		this->_gb->_mem->write(IF, intF & ~INT_TIMER);
		this->_gb->_mem->write(_regSP - 1, _regPC >> 8);
		this->_gb->_mem->write(_regSP - 2, _regPC & 0xFF);
		_regSP-= 2;
		_regPC = INT_TIMER_ADDR;
	} else if(intE & intF & INT_SERIAL) // vblank interrupt
	{
		_ime = false;
		_halted = false;
		_stopped = false;
		_cycles = 1;
		this->_gb->_mem->write(IF, intF & ~INT_SERIAL);
		this->_gb->_mem->write(_regSP - 1, _regPC >> 8);
		this->_gb->_mem->write(_regSP - 2, _regPC & 0xFF);
		_regSP-= 2;
		_regPC = INT_SERIAL_ADDR;
	} else if(intE & intF & INT_JOYPAD) // vblank interrupt
	{
		_ime = false;
		_halted = false;
		_stopped = false;
		_cycles = 1;
		this->_gb->_mem->write(IF, intF & ~INT_JOYPAD);
		this->_gb->_mem->write(_regSP - 1, _regPC >> 8);
		this->_gb->_mem->write(_regSP - 2, _regPC & 0xFF);
		_regSP-= 2;
		_regPC = INT_JOYPAD_ADDR;
	}
}

#endif