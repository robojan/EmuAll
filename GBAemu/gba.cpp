#include <GBAemu/gba.h>
#include <GBAemu/defines.h>
#include <GBAemu/util/log.h>
#include <GBAemu/cpu/armException.h>

Gba::Gba() :
	_memory(*this), _cpu(*this), _disassembler(*this)
{

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
	_memory.Tick();
	_cpu.Tick(true);
}

int Gba::Tick(unsigned int time)
{
	if (!IsRunning())
		return 0;
	int execute = ((uint64_t)time * FCPU + 500000) / 1000000;
	for (int i = execute; i != 0; --i)
	{
		_memory.Tick();
		try {
			_cpu.Tick();
		}
		catch (BreakPointARMException &e) {
			Log(Message, "Breakpoint hit %d", e._value);
			Run(false);
			break;
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

void Gba::Draw(int id)
{

}
