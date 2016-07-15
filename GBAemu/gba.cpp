#include <GBAemu/gba.h>
#include <GBAemu/defines.h>
#include <GBAemu/util/log.h>
#include <GBAemu/cpu/armException.h>

Gba::Gba() :
	_memory(*this), _cpu(*this), _gpu(*this), _input(*this), 
	_if(0), _halted(false), _stopped(false), _running(false), _disassembler(*this),
	_timerInfo{ {0}, {0}, {0}, {0} }
{
	_memory.RegisterEvent(IE, this);
	_memory.RegisterEvent(IME, this);
	_memory.RegisterEvent(POSTFLG, this);
	_memory.RegisterEvent(TM0CNT_L, this);
	_memory.RegisterEvent(TM1CNT_L, this);
	_memory.RegisterEvent(TM2CNT_L, this);
	_memory.RegisterEvent(TM3CNT_L, this);

	InitRegisters();
}

Gba::~Gba()
{

}

int Gba::Init()
{
	return 1;
}

int Gba::Load(const SaveData_t *data)
{
	return _memory.Load(data);
}

void Gba::Step()
{
	if (!_stopped) {
		while (_memory.IsDMAActive()) {
			_memory.Tick();
			_gpu.Tick();
			TimerTick();
		}
		TimerTick();
		_memory.Tick();
		if (!_halted) {
			_cpu.Tick(true);
		}
		else {
			uint8_t *registers = _memory.GetRegisters();
			if ((IOREG8(registers, IF) & IOREG8(registers, IE) & IRQ_MASK) != 0) {
				_halted = false;
				_stopped = false;
			}
		}
		_gpu.Tick();
	}
	else {
		uint8_t *registers = _memory.GetRegisters();
		if ((IOREG8(registers, IF) & IOREG8(registers, IE) & IRQ_MASK) != 0) {
			_halted = false;
			_stopped = false;
		}
	}
}

int Gba::Tick(unsigned int time)
{
	if (!IsRunning())
		return 0;
	int execute = ((uint64_t)time * FCPU + 500000) / 1000000;
	// Clear breakpoint
	Step(); Step();
	for (int i = execute; i != 0; --i)
	{
		if (!_stopped) {
			TimerTick();
			_memory.Tick();
			if (!_halted && !_memory.IsDMAActive()) {
				try {
					_cpu.Tick();
				}
				catch (BreakPointARMException &e) {
					Log(Message, "Breakpoint hit %d", e._value);
					Run(false);
					break;
				}
				catch (DataAbortARMException &) {
					Run(false);
					Log(Error, "Data Abort ARM Exception");
					break;
				}
			}
			else {
				uint8_t *registers = _memory.GetRegisters();
				if ((IOREG8(registers, IF) & IOREG8(registers, IE) & IRQ_MASK) != 0) {
					_halted = false;
					_stopped = false;
				}
			}
			_gpu.Tick();
		}
		else {
			uint8_t *registers = _memory.GetRegisters();
			if ((IOREG8(registers, IF) & IOREG8(registers, IE) & IRQ_MASK) != 0) {
				_halted = false;
				_stopped = false;
			}
		}
	}
	return 1;
}

void Gba::Run(bool run /*= true*/)
{
	_running = run;
	if (run)
	{
		Log(Message, "Start running the cpu");
	}
	else {
		Log(Message, "Stop running the cpu");
	}
}

bool Gba::IsRunning() const
{
	return _running;
}

void Gba::RequestIRQ(int mask)
{
	uint8_t *registers = _memory.GetRegisters();
	_if |= mask;
	IOREG16(registers, IF) = _if;
}

void Gba::HandleEvent(uint32_t address, int size)
{
	uint8_t *registers = _memory.GetRegisters();
	if (address <= IE + 1 && IE < address + size) {
		IOREG16(registers, IE) |= 0xC000;
	}
	if (address <= IF + 1 && IF < address + size) {
		_if &= (~IOREG16(registers, IF) | 0xC000);
		IOREG16(registers, IF) = _if;
	}
	if (address <= IME + 3 && IME < address + size) {
		IOREG32(registers, IME) |= 0xFFFFFFFE;
	}
	if (address <= HALTCNT && HALTCNT < address + size) {
		if ((IOREG8(registers, HALTCNT) & 0x80) != 0) {
			PowerModeStop();
		}
		else {
			PowerModeHalt();
		}
		IOREG8(registers, HALTCNT) = 0;
	}
	if (address <= TM0CNT_L + 1 && TM0CNT_L < address + size) {
		_timerInfo[0].reload = IOREG16(registers, TM0CNT_L);
		IOREG16(registers, TM0CNT_L) = _timerInfo[0].val;
	}
	if (address <= TM1CNT_L + 1 && TM1CNT_L < address + size) {
		_timerInfo[1].reload = IOREG16(registers, TM1CNT_L);
		IOREG16(registers, TM1CNT_L) = _timerInfo[1].val;
	}
	if (address <= TM2CNT_L + 1 && TM2CNT_L < address + size) {
		_timerInfo[2].reload = IOREG16(registers, TM2CNT_L);
		IOREG16(registers, TM2CNT_L) = _timerInfo[2].val;
	}
	if (address <= TM3CNT_L + 1 && TM3CNT_L < address + size) {
		_timerInfo[3].reload = IOREG16(registers, TM3CNT_L);
		IOREG16(registers, TM3CNT_L) = _timerInfo[3].val;
	}
	if (address <= TM0CNT_H + 1 && TM0CNT_H < address + size) {
		uint16_t newVal = IOREG16(registers, TM0CNT_H);
		if ((_timerInfo[0].cnt ^ newVal) & (1<<7)) {
			_timerInfo[0].val = _timerInfo[0].reload;
			IOREG16(registers, TM0CNT_L) = _timerInfo[0].val;
		}
		switch (newVal & 3) {
		case 0: _timerInfo[0].prescalerMatch = 0; break;
		case 1: _timerInfo[0].prescalerMatch = 63; break;
		case 2: _timerInfo[0].prescalerMatch = 255; break;
		case 3: _timerInfo[0].prescalerMatch = 1023; break;
		}
		_timerInfo[0].cnt = newVal;
	}
	if (address <= TM1CNT_H + 1 && TM1CNT_H < address + size) {
		uint16_t newVal = IOREG16(registers, TM1CNT_H);
		if ((_timerInfo[1].cnt ^ newVal) & (1 << 7)) {
			_timerInfo[1].val = _timerInfo[1].reload;
			IOREG16(registers, TM1CNT_L) = _timerInfo[1].val;
		}
		switch (newVal & 3) {
		case 0: _timerInfo[1].prescalerMatch = 0; break;
		case 1: _timerInfo[1].prescalerMatch = 63; break;
		case 2: _timerInfo[1].prescalerMatch = 255; break;
		case 3: _timerInfo[1].prescalerMatch = 1023; break;
		}
		_timerInfo[1].cnt = newVal;
	}
	if (address <= TM2CNT_H + 1 && TM2CNT_H < address + size) {
		uint16_t newVal = IOREG16(registers, TM2CNT_H);
		if ((_timerInfo[2].cnt ^ newVal) & (1 << 7)) {
			_timerInfo[2].val = _timerInfo[2].reload;
			IOREG16(registers, TM2CNT_L) = _timerInfo[2].val;
		}
		switch (newVal & 3) {
		case 0: _timerInfo[2].prescalerMatch = 0; break;
		case 1: _timerInfo[2].prescalerMatch = 63; break;
		case 2: _timerInfo[2].prescalerMatch = 255; break;
		case 3: _timerInfo[2].prescalerMatch = 1023; break;
		}
		_timerInfo[2].cnt = newVal;
	}
	if (address <= TM3CNT_H + 1 && TM3CNT_H < address + size) {
		uint16_t newVal = IOREG16(registers, TM3CNT_H);
		if ((_timerInfo[3].cnt ^ newVal) & (1 << 7)) {
			_timerInfo[3].val = _timerInfo[3].reload;
			IOREG16(registers, TM3CNT_L) = _timerInfo[3].val;
		}
		switch (newVal & 3) {
		case 0: _timerInfo[3].prescalerMatch = 0; break;
		case 1: _timerInfo[3].prescalerMatch = 63; break;
		case 2: _timerInfo[3].prescalerMatch = 255; break;
		case 3: _timerInfo[3].prescalerMatch = 1023; break;
		}
		_timerInfo[3].cnt = newVal;
	}
}


void Gba::OnKey(int id, bool pressed)
{
	_input.OnKeyPressed(id, pressed);
}

void Gba::InitRegisters()
{
	uint8_t *registers = _memory.GetRegisters();
	IOREG16(registers, IE) = 0xC000;
	_if = 0xC000;
	IOREG16(registers, IF) = _if;
	IOREG32(registers, IME) = 0xFFFFFFFE;
}

void Gba::PowerModeStop()
{
	_halted = true;
	_stopped = true;
}

void Gba::PowerModeHalt()
{
	_halted = true;
	_stopped = false;
}

void Gba::TimerTick()
{
	bool lastOverflow = false;
	for (int i = 0; i < 4; i++) {
		if (_timerInfo[i].cnt & (1 << 7)) {
			// enabled
			if (_timerInfo[i].cnt & (1 << 2) && lastOverflow) {
				// Count up
				if (_timerInfo[i].val == 0xFFFF) {
					_timerInfo[i].val = _timerInfo[i].reload;
					lastOverflow = true;
					if (_timerInfo[i].cnt & (1 << 6)) {
						RequestIRQ(IRQ_TIM0 << i);
					}
				}
				else {
					_timerInfo[i].val++;
					lastOverflow = false;
				}
				IOREG16(_memory.GetRegisters(), TM0CNT_L + 4 * i) = _timerInfo[i].val;
				_timerInfo[i].prescaler = 0;
			}
			else if(_timerInfo[i].prescaler == _timerInfo[i].prescalerMatch){
				_timerInfo[i].prescaler = 0;
				if (_timerInfo[i].val == 0xFFFF) {
					_timerInfo[i].val = _timerInfo[i].reload;
					lastOverflow = true;
					if (_timerInfo[i].cnt & (1 << 6)) {
						RequestIRQ(IRQ_TIM0 << i);
					}
				}
				else {
					_timerInfo[i].val++;
					lastOverflow = false;
				}
				IOREG16(_memory.GetRegisters(), TM0CNT_L + 4 * i) = _timerInfo[i].val;
			}
			else {
				lastOverflow = false;
				_timerInfo[i].prescaler++;
			}
		}
	}

}
