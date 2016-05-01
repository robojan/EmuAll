#ifndef _GBMEM_H
#define _GBMEM_H

#include <emuall/util/tree.h>
#include <emuall/util/endian.h>
#include "../defines.h"
#include "../../util/log.h"
#include <ctime>
#include "../Gameboy.h"
#include <vector>

typedef struct {
	uint8_t	seconds;
	uint8_t	minutes;
	uint8_t	hours;
	uint32_t day;
	time_t	systemTime;
} gbTime;

class GbMemEvent
{
public:
	virtual void MemEvent(address_t address, gbByte val) = 0;
};


class GbMem : public GbMemEvent
{
	friend class GbGpu;
	friend class Gameboy;
public:
	GbMem(Gameboy *master);
	~GbMem();

	void DeleteRam(void);
	void switchBank(unsigned int bank);
	void switchWramBank(unsigned int bank);
	void switchVramBank(unsigned int bank);
	void switchEramBank(unsigned int bank);
	void setAccessable(bool vram, bool oam);
	bool load(rom_t *rom, rom_t *ram, rom_t *misc);
	void registerEvent(address_t address, GbMemEvent *evt);
	void registerEvent(address_t rangeLow, address_t rangeHigh, GbMemEvent *evt);
	void MemEvent(address_t address, gbByte val);
	bool Save(SaveData_t *data);
	uint32_t GetUnbankedAddress(address_t address) const;
	int GetROMSize() const;
	int GetRAMSize() const;
	uint8_t GetROMData(unsigned int address) const;
	uint8_t GetERAMData(unsigned int address) const;
	uint8_t GetVRAMData(unsigned int address) const;
	uint8_t GetWRAMData(unsigned int address) const;
	inline int GetROMBank() const { return _romBank; }
	inline int GetERAMBank() const { return _eramBank; }
	inline int GetVRAMBank() const { return _vramBank; }
	inline int GetWRAMBank() const { return _wramBank; }
	bank_t GetRegisterPointer() const { return _mem[0xF];}

	inline void write(address_t address, gbByte val, bool event = true);
	inline gbByte read(address_t address) const;

	bool LoadState(const SaveData_t *data);
	bool SaveState(rom_t *rom, rom_t *ram, std::vector<uint8_t> &data);
private:
	// Memory data
	rom_t *_rom;
	bank_t _mem[0x10];
	bank_t _vram[VRAM_BANKS];
	bank_t _wram[WRAM_BANKS];
	bank_t *_eram;
	bank_t _regs;
	// Memory information
	int				_eramSize;
	unsigned int	_romBank;
	unsigned int	_vramBank;
	unsigned int	_wramBank;
	unsigned int	_eramBank;
	bool			_vramAccess;
	bool			_oamAccess;
	bool			_rtc;
	bool			_rtcActive;
	gbByte			_rtcRegister;
	gbByte			_lastLatch;
	gbTime			_timeStamp;
	
	// Events
	AATree<address_t, GbMemEvent *> _eventTable;

	void CalculateRTC(gbTime *time);
};


inline void GbMem::write(address_t address, gbByte val, bool event)
{
	// Write data to the memory
	static int mode = 0;
	// TODO: review code
	unsigned int t = (address>>12)&0xf;
	
	if(!_vramAccess && (address >= 0x8000 && address < 0xA000) ||
		!_oamAccess && (address >= 0xFE00 && address < 0xFEA0))
	{
		//wxLogWarning("Tried to write gpu memory when not available");
		return;
	}

	if(address <= 0x7FFF) 
	{
		// write to MBC
		switch(read(CARTRIDGE_TYPE))
		{
		case ROM_ONLY:
			break;
		case MBC1:
		case MBC1_RAM:
		case MBC1_RAM_BAT:
			if(address<=0x1FFF)
			{
				//warning("change ram status");
				// enable /disable ram
				// TODO: change ram status
			} else if(address>=0x2000 && address <= 0x3FFF) // ROM bank select
			{
				if(val ==0) val = 1;
				switchBank(((unsigned char)val&0x1F) | (_romBank & 0xE0));
			} else if(address>=0x4000 && address <= 0x5FFF) // RAM bank select
			{
				if(mode)
				{
					switchEramBank((unsigned char)val&0x3);
				} else {
					switchBank((_romBank & 0x1F) | (((unsigned char)val&0x3)<<5));
				}
			} else if(address>=0x6000)
			{
				mode = val;
			}
			break;
		case MBC2:
		case MBC2_BAT:
			if(address<=0x1FFF)
			{
				// enable /disable ram
			} else if(address>=0x2000 && address <= 0x3FFF) // ROM bank select
			{
				if(val ==0) val = 1;
				switchBank((unsigned char)val&0x0F);
			} 
			break;
		case MBC3:
		case MBC3_RAM:
		case MBC3_RAM_BAT:
		case MBC3_TIM_BAT:
		case MBC3_TIM_RAM_BAT:
			if(address<=0x1FFF)
			{
				//warning("change ram status");
				// enable /disable ram rtc
			} else if(address>=0x2000 && address <= 0x3FFF) // ROM bank select
			{
				if(val ==0) val = 1;
				switchBank((unsigned char)val&0x7F);
			} else if(address>=0x4000 && address <= 0x5FFF) // RAM bank select
			{
				if(val & 0x08)
				{
					// rtc access
					_rtc = true;
					_rtcRegister = val;
				} else {
					_rtc = false;
					switchEramBank((unsigned char)val&0x3);
				}
			} else if(address>=0x6000 && address <= 0x7FFF) // latch clock data
			{
				if(_lastLatch == 0x00 && val == 0x01)
				{
					if(_rtcActive)
					{
						CalculateRTC(&_timeStamp);
					}
				}
				_lastLatch = val;
			} 
			break;
		case MBC4:
		case MBC4_RAM:
		case MBC4_RAM_BAT:
			Log(Error, "MBC4 memory found");
			// TODO: add MBC4 support
			//break;
		case MBC5:
		case MBC5_RAM:
		case MBC5_RAM_BAT:
		case MBC5_RUM:
		case MBC5_RUM_RAM:
		case MBC5_RUM_RAM_BAT:
			if(address<=0x1FFF)
			{
				//warning("change ram status");
				// enable /disable ram
			} else if(address>=0x2000 && address <= 0x2FFF) // ROM bank select
			{
				// 8 most significant bits
				switchBank((this->_romBank&0x100)|((unsigned char)val));
			} else if(address>=0x3000 && address <= 0x3FFF) // ROM bank select
			{
				// least significant bit
				switchBank((this->_romBank&0xFF)|(((unsigned char)val&1)<<8));
			} else if(address>=0x4000 && address <= 0x5FFF) // RAM bank select
			{
				switchEramBank((unsigned char)val&0xF);
			}
			break;
		default:
			Log(Error, "Unknown memory type");
			break;
		}
		return;
	}
	if(_rtc && address >= 0xA000 && address < 0xC000)
	{
		switch(_rtcRegister)
		{
		case 0x08:
			_timeStamp.seconds = val;
			break;
		case 0x09:
			_timeStamp.minutes = val;
			break;
		case 0x0A:
			_timeStamp.hours = val;
			break;
		case 0x0B:
			_timeStamp.day = (_timeStamp.day & ~0xFF)|val;
			break;
		case 0x0C:
			_timeStamp.day = (_timeStamp.day & ~0x300)|((val&0x01)<<8)|((val&0x80)<<2);
			_rtcActive = (val & 0x40) == 0;
			if(_rtcActive)
			{
				_timeStamp.systemTime = time(NULL);
			}
			break;
		}
	} else {
		if(address >= 0xE000 && address < 0xFEA0) address -= 0x2000; // copy of memory
		if(this->_mem[t] == NULL)
		{
			//wxLogWarning("memory %04X does not exist", address);
			return;
		}
	
		this->_mem[t][address&0xFFF] = val;
	}
	if(event)
	{
		auto &it = _eventTable.Find(address);
		while (it != _eventTable.End()) {
			it->MemEvent(address, val);
			it++;
		}		
	}
}

inline gbByte GbMem::read(address_t address) const
{
	// Read data from memory
	if(address >= 0xE000 && address < 0xFEA0) 
		address -= 0x2000; // copy of memory
	unsigned int t = (address>>12) & 0xf;
	
	if(!_vramAccess && (address >= 0x8000 && address < 0xA000) ||
		!_oamAccess && (address >= 0xFE00 && address < 0xFEA0))
	{
		//wxLogWarning("Tried to read gpu memory when not available");
		return 0xFF;
	}
	
	if(_rtc && address >= 0xA000 && address < 0xC000)
	{
		switch(_rtcRegister)
		{
		case 0x08:
			return _timeStamp.seconds;
			break;
		case 0x09:
			return _timeStamp.minutes;
			break;
		case 0x0A:
			return _timeStamp.hours;
			break;
		case 0x0B:
			return (gbByte)(_timeStamp.day&0xFF);
			break;
		case 0x0C:
			return (gbByte)(((_timeStamp.day&0x100)>>8)| 
				(_rtcActive?0x00:0x40) | 
				((_timeStamp.day&0x200)? 0x80:0x00));
			break;
		}
	}
	if (this->_mem[t] == NULL)
		return 0xFF;
	return this->_mem[t][address&0xFFF];
}

#endif