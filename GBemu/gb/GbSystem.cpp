
#include "GbSystem.h"
#include "defines.h"
#include "Gameboy.h"
#include "../util/log.h"

GbSystem::GbSystem()
{
	m_breakpoint = -1;
	cycles = 0;
}

void GbSystem::setBreakpoint(int breakpoint)
{
	m_breakpoint = breakpoint;
}

void GbSystem::step(void)
{
	cycles = 1;
	Log(Message, "Step once");
}

void GbSystem::run(bool run)
{
	if(run)
	{
		cycles = -1;
		/*if(Gameboy::gbInfo.sound->getAudioStream() != NULL)
		{
			Gameboy::gbInfo.sound->getAudioStream()->Start();
		}*/
		Log(Message, "Start running the cpu");
	} else {
		cycles = 0;
		/*if(Gameboy::gbInfo.sound->getAudioStream() != NULL)
		{
			Gameboy::gbInfo.sound->getAudioStream()->Stop();
		}*/
		Log(Message, "Stop running the cpu");
	}
}

void GbSystem::tick(int time) 
{
	if(cycles == 0)
		return;
	bool limited = false;
	if(cycles > 0)
	{
		limited = true;
		--cycles;
	}
	int execute = time * FCPU_MS;
	if(Gameboy::gbInfo.cpu->doubleSpeed) // CPU in double speed mode?
	{
		execute = execute *2;
	}
	// sync sound with cpu
	Gameboy::gbInfo.sound->sync();
	for(int i = execute; i != 0; --i)
	{
		if(m_breakpoint != -1 && Gameboy::gbInfo.cpu->regPC == m_breakpoint && Gameboy::gbInfo.cpu->cycles == 0)
		{
			run(false);
			return;
		}
		if(Gameboy::gbInfo.cpu->stopped)
		{
			gbByte key1 = Gameboy::gbInfo.mem->read(KEY1);
			if(key1 & KEY1_PREP) // if the cpu is stopped and the switch to double frequency is set
			{
				if(key1 & KEY1_CURSPEED) // if the cpu is in double speed
				{
					i = i/2; // half the execution speed;
					Gameboy::gbInfo.mem->write(KEY1, (gbByte) 0x00);
					Gameboy::gbInfo.cpu->doubleSpeed = false;
				} else {
					i = i*2; // double the execution speed;
					Gameboy::gbInfo.mem->write(KEY1, (gbByte) KEY1_CURSPEED);
					Gameboy::gbInfo.cpu->doubleSpeed = true;
				}
				// And continue the cpu
				Gameboy::gbInfo.cpu->stopped = false;
			}
		}
		
		if(Gameboy::gbInfo.cpu->tick()&&limited)
		{
			break;
		}
		// TODO: add serial port
		if((Gameboy::gbInfo.cpu->doubleSpeed && (i&1)) || !Gameboy::gbInfo.cpu->doubleSpeed)
		{
			// In double speed this is stil normal
			Gameboy::gbInfo.gpu->tick();
			// keep sound in sync
			Gameboy::gbInfo.sound->m_cpuCounter++;
			Gameboy::gbInfo.sound->tick();
		}
	}
}