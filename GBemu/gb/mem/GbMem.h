#ifndef _GBMEM_H
#define _GBMEM_H

#include "../../types/tree.h"
#include "../../util/Endian.h"
#include "../defines.h"
#include "../../util/log.h"
#include <ctime>
#include "../Gameboy.h"

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
	bool load(rom_t *rom, rom_t *ram);
	void registerEvent(address_t address, GbMemEvent *evt);
	void registerEvent(address_t rangeLow, address_t rangeHigh, GbMemEvent *evt);
	void MemEvent(address_t address, gbByte val);
	bool Save(std::ofstream &file);
	uint32_t GetUnbankedAddress(address_t address) const;
	int GetROMSize() const;
	int GetRAMSize() const;
	uint8_t GetROMData(unsigned int address) const;
	uint8_t GetERAMData(unsigned int address) const;
	uint8_t GetVRAMData(unsigned int address) const;
	uint8_t GetWRAMData(unsigned int address) const;
	inline int GetROMBank() const { return romBank; }
	inline int GetERAMBank() const { return eramBank; }
	inline int GetVRAMBank() const { return vramBank; }
	inline int GetWRAMBank() const { return wramBank; }
	bank_t GetRegisterPointer() const { return mem[0xF];}

	inline void write(address_t address, gbByte val, bool event = true);
	inline gbByte read(address_t address) const;
private:
	// Memory data
	rom_t *rom;
	bank_t mem[0x10];
	bank_t vram[VRAM_BANKS];
	bank_t wram[WRAM_BANKS];
	bank_t *eram;
	bank_t regs;
	// Memory information
	int				eramSize;
	unsigned int	romBank;
	unsigned int	vramBank;
	unsigned int	wramBank;
	unsigned int	eramBank;
	bool			vramAccess;
	bool			oamAccess;
	bool			m_rtc;
	bool			m_rtcActive;
	gbByte			m_rtcRegister;
	gbTime			m_latchedTime;
	gbByte			m_lastLatch;
	gbTime			m_timeStamp;
	
	// Events
	tree			eventTable;

	void CalculateRTC(gbTime *time);
};


inline void GbMem::write(address_t address, gbByte val, bool event)
{
	// Write data to the memory
	static int mode = 0;
	// TODO: review code
	unsigned int t = (address>>12)&0xf;
	
	if(!vramAccess && (address >= 0x8000 && address < 0xA000) ||
		!oamAccess && (address >= 0xFE00 && address < 0xFEA0))
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
				switchBank(((unsigned char)val&0x1F) | (romBank & 0xE0));
			} else if(address>=0x4000 && address <= 0x5FFF) // RAM bank select
			{
				if(mode)
				{
					switchEramBank((unsigned char)val&0x3);
				} else {
					switchBank((romBank & 0x1F) | (((unsigned char)val&0x3)<<5));
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
					m_rtc = true;
					m_rtcRegister = val;
				} else {
					m_rtc = false;
					switchEramBank((unsigned char)val&0x3);
				}
			} else if(address>=0x6000 && address <= 0x7FFF) // latch clock data
			{
				if(m_lastLatch == 0x00 && val == 0x01)
				{
					if(m_rtcActive)
					{
						CalculateRTC(&m_timeStamp);
					}
					m_latchedTime = m_timeStamp;
				}
				m_lastLatch = val;
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
				switchBank((this->romBank&0x100)|((unsigned char)val));
			} else if(address>=0x3000 && address <= 0x3FFF) // ROM bank select
			{
				// least significant bit
				switchBank((this->romBank&0xFF)|(((unsigned char)val&1)<<8));
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
	if(m_rtc && address >= 0xA000 && address < 0xC000)
	{
		if(m_rtcActive)
		{
			CalculateRTC(&m_timeStamp);
		}
		switch(m_rtcRegister)
		{
		case 0x08:
			m_timeStamp.seconds = val;
			break;
		case 0x09:
			m_timeStamp.minutes = val;
			break;
		case 0x0A:
			m_timeStamp.hours = val;
			break;
		case 0x0B:
			m_timeStamp.day = (m_timeStamp.day & ~0xFF)|val;
			break;
		case 0x0C:
			m_timeStamp.day = (m_timeStamp.day & ~0x300)|((val&0x01)<<8)|((val&0x80)<<2);
			m_rtcActive = (val & 0x40) == 0;
			if(m_rtcActive)
			{
				m_timeStamp.systemTime = time(NULL);
			}
			break;
		}
	} else {
		if(address >= 0xE000 && address < 0xFEA0) address -= 0x2000; // copy of memory
		if(this->mem[t] == NULL)
		{
			//wxLogWarning("memory %04X does not exist", address);
			return;
		}
	
		this->mem[t][address&0xFFF] = val;
	}
	if(event)
	{
		node *evt = eventTable.search(address);
		while(evt != NULL)
		{
			if(((GbMemEvent *)evt->ptr) == NULL)
			{
				evt = evt->middle;
				continue;
			}
			((GbMemEvent *)evt->ptr)->MemEvent(address, val);
			evt = evt->middle;
		}
	}
}

inline gbByte GbMem::read(address_t address) const
{
	// Read data from memory
	if(address >= 0xE000 && address < 0xFEA0) 
		address -= 0x2000; // copy of memory
	unsigned int t = (address>>12) & 0xf;
	
	if(!vramAccess && (address >= 0x8000 && address < 0xA000) ||
		!oamAccess && (address >= 0xFE00 && address < 0xFEA0))
	{
		//wxLogWarning("Tried to read gpu memory when not available");
		return 0xFF;
	}
	
	if(m_rtc && address >= 0xA000 && address < 0xC000)
	{
		switch(m_rtcRegister)
		{
		case 0x08:
			return m_latchedTime.seconds;
			break;
		case 0x09:
			return m_latchedTime.minutes;
			break;
		case 0x0A:
			return m_latchedTime.hours;
			break;
		case 0x0B:
			return (gbByte)(m_latchedTime.day&0xFF);
			break;
		case 0x0C:
			return (gbByte)(((m_latchedTime.day&0x100)>>8)| (m_rtcActive?0x00:0x40) | ((m_latchedTime.day&0x200)? 0x80:0x00));
			break;
		}
	}
	if (this->mem[t] == NULL)
		return 0xFF;
	return this->mem[t][address&0xFFF];
}

#endif