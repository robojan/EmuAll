#ifndef _GBINPUT_H
#define _GBINPUT_H

#include "../mem/GbMem.h"
#include "../Gameboy.h"
#include <vector>

class GbInput : public GbMemEvent 
{
public:
	GbInput(Gameboy *info);
	~GbInput();
	void update();

	void registerEvents();
	void Input(int key, bool pressed);
	void MemEvent(address_t address, gbByte val);

	bool LoadState(const SaveData_t *data);
	bool SaveState(std::vector<uint8_t> &data);
private:
	Gameboy			*_gb;
	bool			_u;
	bool			_d;
	bool			_l;
	bool			_r;
	bool			_a;
	bool			_b;
	bool			_se;
	bool			_st;
};

#endif