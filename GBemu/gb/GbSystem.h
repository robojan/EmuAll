#ifndef _GBSYSTEM_H
#define _GBSYSTEM_H

class GbSystem
{
public:
	GbSystem();
	void step(void);
	void run(bool run = true);
	void tick(int time);
	bool isRunning(void);
	void setBreakpoint(int breakpoint);
private:
	int cycles;
	int m_breakpoint;
};

inline bool GbSystem::isRunning(void) { return cycles != 0; }

#endif