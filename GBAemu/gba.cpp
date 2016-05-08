#include <GBAemu/gba.h>
#include <GBAemu/defines.h>
#include <GBAemu/util/log.h>
#include <GBAemu/cpu/armException.h>

Gba::Gba() :
	_memory(*this), _cpu(*this), _gpu(*this), 
	_if(0), _halted(false), _stopped(false), _running(false), _disassembler(*this)
{
	_memory.RegisterEvent(IE, this);
	_memory.RegisterEvent(IME, this);
	_memory.RegisterEvent(POSTFLG, this);

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
		}
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
	if (address <= IME + 1 && IME < address + size) {
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
