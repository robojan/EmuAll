#include "../../util/memDbg.h"


#include "gbMem.h"
#include "../../types/general.h"
#include "../../util/log.h"
#include <iostream>
#include <fstream>

/*
	Memory Map:
__________________________________________________________________________________
	
  0000-3FFF   16KB ROM Bank 00     (in cartridge, fixed at bank 00)
  4000-7FFF   16KB ROM Bank 01..NN (in cartridge, switchable bank number)
  8000-9FFF   8KB Video RAM (VRAM) (switchable bank 0-1 in CGB Mode)
  A000-BFFF   8KB External RAM     (in cartridge, switchable bank, if any)
  C000-CFFF   4KB Work RAM Bank 0 (WRAM)
  D000-DFFF   4KB Work RAM Bank 1 (WRAM)  (switchable bank 1-7 in CGB Mode)
  E000-FDFF   Same as C000-DDFF (ECHO)    (typically not used)
  FE00-FE9F   Sprite Attribute Table (OAM)
  FEA0-FEFF   Not Usable
  FF00-FF7F   I/O Ports
  FF80-FFFE   High RAM (HRAM)
  FFFF        Interrupt Enable Register
*/

GbMem::GbMem(Gameboy *master)
{
	// Constructor
	// set all pointers to NULL
	rom = NULL;
	for(int i = 0; i < VRAM_BANKS; i++)
	{
		vram[i] = NULL;
	}
	for(int i = 0; i < WRAM_BANKS; i++)
	{
		wram[i] = NULL;
	}
	eram = NULL;
	regs = NULL;
	romBank = 1;
	vramBank = 0;
	wramBank = 1;
	eramBank = 0;
	vramAccess = true;
	oamAccess = true;
	m_rtc = false;
	m_timeStamp.day = 0;
	m_timeStamp.hours = 0;
	m_timeStamp.minutes = 0;
	m_timeStamp.seconds = 0;
	m_timeStamp.systemTime = time(NULL);
	m_rtcRegister = 0x08;
	m_rtcActive = true;
	m_lastLatch = 0xFF;
	eventTable.destroy_tree();
}

GbMem::~GbMem()
{
	// Destructor
	// Free all the used memory
	DeleteRam();
}

uint32_t GbMem::GetUnbankedAddress(address_t address) const
{
	// Mem address
	// lowest 16 bit physical address
	// upper 16 bit bank
	if (address >= 0xE000 && address < 0xFE00)
		address -= 0x2000;
	if (address < 0x4000) // ROM bank 0
	{
		return (address & 0xFFFF);
	}
	else if (address >= 0x4000 && address < 0x8000) // ROM banked
	{
		return (romBank << 16) | (address & 0xFFFF);
	}
	else if (address >= 0x8000 && address < 0xA000) // VRAM banked
	{
		return (vramBank << 16) | (address & 0xFFFF);
	}
	else if (address >= 0xA000 && address < 0xC000) // ERAM banked
	{
		return (eramBank << 16) | (address & 0xFFFF);
	}
	else if (address >= 0xC000 && address < 0xD000) // WRAM bank 0
	{
		return (address & 0xFFFF);
	}
	else if (address >= 0xD000 && address < 0xE000) // WRAM banked
	{
		return (wramBank << 16) | (address & 0xFFFF);
	}
	else {
		return address & 0xFFFF;
	}
}

int GbMem::GetROMSize() const
{
	return rom->length;
}

int GbMem::GetRAMSize() const
{
	return eramSize*ERAM_BANK_SIZE;
}

uint8_t GbMem::GetROMData(unsigned int address) const
{
	if (address >= rom->length)
		return 0xFF;
	return rom->data[address];
}

uint8_t GbMem::GetERAMData(unsigned int address) const
{
	if (address >= (unsigned int)eramSize * ERAM_BANK_SIZE)
		return 0xFF;
	return eram[address / ERAM_BANK_SIZE][address % ERAM_BANK_SIZE];
}

uint8_t GbMem::GetVRAMData(unsigned int address) const
{
	if (address >= VRAM_BANKS * VRAM_BANK_SIZE)
		return 0xFF;
	return vram[address / VRAM_BANK_SIZE][address % VRAM_BANK_SIZE];
}

uint8_t GbMem::GetWRAMData(unsigned int address) const
{
	if (address >= WRAM_BANKS * WRAM_BANK_SIZE)
		return 0xFF;
	return wram[address / WRAM_BANK_SIZE][address % WRAM_BANK_SIZE];
}

static const uint32_t StateMEMid = 0x4D454D20;

// 0 - id
// 4 - size
// 8 - romBank
// 12 - vramBank
// 16 - wramBank
// 20 - eramBank
// 24 - vramAccess << 3 | oamAccess << 2 | rtc << 1 | rtcActive << 0
// 25 - rtcRegister
// 26 - lastLatch
// 27 - seconds
// 28 - minutes
// 29 - hours
// 30 - days
// 34 - systemtime
// 42 - vram[VRAM_BANKS * VRAM_BANK_SIZE]
// 16462 - wram[WRAM_BANKS * WRAM_BANK_SIZE]
// 49194 - regs[REGS_BANK_SIZE]
// 53290

bool GbMem::LoadState(const SaveData_t *data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	uint8_t *ptr = (uint8_t *)data->miscData;
	int miscLen = data->miscDataLen;
	int expectedDataLen = 42 + VRAM_BANKS * VRAM_BANK_SIZE + WRAM_BANKS * WRAM_BANK_SIZE + REGS_BANK_SIZE;
	// Find cpu segment
	while (miscLen >= 8) {
		uint32_t id = conv->convu32(*(uint32_t *)(ptr + 0));
		uint32_t len = conv->convu32(*(uint32_t *)(ptr + 4));
		if (id == StateMEMid && len >= (uint32_t)expectedDataLen) {
			break;
		}
		ptr += len;
		miscLen -= len;
	}
	if (miscLen < expectedDataLen) return false;
	for (int i = 0; i < eramSize && (unsigned int)((i + 1) * ERAM_BANK_SIZE) < data->ramDataLen; i++)
	{
		memcpy(eram[i], (uint8_t *)data->ramData + (i*ERAM_BANK_SIZE), ERAM_BANK_SIZE);
	}
	for (int i = 0; i < WRAM_BANKS; i++) {
		memcpy(wram[i], ptr + 42 + VRAM_BANKS * VRAM_BANK_SIZE + i * WRAM_BANK_SIZE, WRAM_BANK_SIZE);
	}
	for (int i = 0; i < VRAM_BANKS; i++) {
		memcpy(vram[i], ptr + 42 + VRAM_BANK_SIZE * i, VRAM_BANK_SIZE);
	}
	memcpy(regs, ptr + 42 + VRAM_BANKS * VRAM_BANK_SIZE + WRAM_BANKS * WRAM_BANK_SIZE, REGS_BANK_SIZE);
	switchBank(conv->convu32(*(uint32_t *)(ptr + 8)));
	switchVramBank(conv->convu32(*(uint32_t *)(ptr + 12)));
	switchWramBank(conv->convu32(*(uint32_t *)(ptr + 16)));
	switchEramBank(conv->convu32(*(uint32_t *)(ptr + 20)));
	m_rtcActive = (ptr[24] & 0x01) != 0;
	m_rtc = (ptr[24] & 0x02) != 0;
	oamAccess = (ptr[24] & 0x04) != 0;
	vramAccess = (ptr[24] & 0x08) != 0;
	m_rtcRegister = ptr[25];
	m_lastLatch = ptr[26];
	m_timeStamp.seconds = ptr[27];
	m_timeStamp.minutes = ptr[28];
	m_timeStamp.hours = ptr[29];
	m_timeStamp.day = conv->convu32(*(uint32_t *)(ptr + 30));
	m_timeStamp.systemTime = conv->convu64(*(uint64_t *)(ptr + 34));
	return true;
}

bool GbMem::SaveState(rom_t *rom, rom_t *ram, std::vector<uint8_t> &data)
{
	const EndianFuncs *conv = getEndianFuncs(0);
	rom->data = NULL;
	rom->length = 0;
	ram->length = eramSize * ERAM_BANK_SIZE;
	ram->data = new uint8_t[ram->length];
	for (int i = 0; i < eramSize; i++) {
		memcpy(ram->data + i * ERAM_BANK_SIZE, eram[i], ERAM_BANK_SIZE);
	}
	int dataLen = 42 + VRAM_BANKS * VRAM_BANK_SIZE + WRAM_BANKS * WRAM_BANK_SIZE + REGS_BANK_SIZE;
	data.resize(data.size() + dataLen);
	uint8_t *ptr = data.data() + data.size() - dataLen;

	*(uint32_t *)(ptr + 0) = conv->convu32(StateMEMid);
	*(uint32_t *)(ptr + 4) = conv->convu32(dataLen);
	*(uint32_t *)(ptr + 8) = conv->convu32(romBank);
	*(uint32_t *)(ptr + 12) = conv->convu32(vramBank);
	*(uint32_t *)(ptr + 16) = conv->convu32(wramBank);
	*(uint32_t *)(ptr + 20) = conv->convu32(eramBank);
	ptr[24] =
		(m_rtcActive ? 0x01 : 0x00) |
		(m_rtc ? 0x02 : 0x00) |
		(oamAccess ? 0x04 : 0x00) |
		(vramAccess ? 0x08 : 0x00);
	ptr[25] = m_rtcRegister;
	ptr[26] = m_lastLatch;
	ptr[27] = m_timeStamp.seconds;
	ptr[28] = m_timeStamp.minutes;
	ptr[29] = m_timeStamp.hours;
	*(uint32_t *)(ptr + 30) = conv->convu32(m_timeStamp.day);
	*(uint64_t *)(ptr + 34) = conv->convu64(m_timeStamp.systemTime);
	for (int i = 0; i < VRAM_BANKS; i++) {
		memcpy(ptr + 42 + i * VRAM_BANK_SIZE, vram[i], VRAM_BANK_SIZE);
	}
	for (int i = 0; i < WRAM_BANKS; i++) {
		memcpy(ptr + 42 + VRAM_BANKS * VRAM_BANK_SIZE + i * WRAM_BANK_SIZE, wram[i], WRAM_BANK_SIZE);
	}
	memcpy(ptr + 42 + VRAM_BANKS * VRAM_BANK_SIZE + WRAM_BANKS * WRAM_BANK_SIZE, regs, REGS_BANK_SIZE);
	return true;
}

void GbMem::CalculateRTC(gbTime *timeStamp)
{
	time_t now = time(NULL);
	int difference = (int)difftime(now, timeStamp->systemTime);
	timeStamp->seconds += difference % 60;
	difference = difference / 60;
	timeStamp->minutes += difference % 60;
	difference = difference / 60;
	timeStamp->hours += difference % 24;
	difference = difference / 24;
	timeStamp->day += difference;

	timeStamp->minutes += timeStamp->seconds / 60;
	timeStamp->seconds %= 60;
	timeStamp->hours += timeStamp->minutes / 60;
	timeStamp->minutes %= 60;
	timeStamp->day += timeStamp->hours / 24;
	timeStamp->hours %= 24;
	timeStamp->systemTime = now;
}

void GbMem::DeleteRam(void)
{
	for(int i = 0; i < VRAM_BANKS; i++)
	{
		if(vram[i] != NULL) 
		{
			delete vram[i]; 
			vram[i] = NULL;
		}
	}
	for(int i = 0; i < WRAM_BANKS; i++)
	{
		if(wram[i] != NULL) 
		{ 
			delete wram[i]; 
			wram[i] = NULL;
		}
	}
	if(regs != NULL)
	{
		delete regs;
		regs = NULL;
	}
	for(int i = 0; i < eramSize; i++)
	{
		if(eram == NULL) break;
		if(eram[i] != NULL) 
		{ 
			delete eram[i]; 
			eram[i] = NULL;
		}
	}
	if(eram != NULL)
	{
		delete eram;
		eram = NULL;
	}
	eventTable.destroy_tree();
}

void GbMem::switchBank(unsigned int bank)
{
	int size = read(ROM_SIZE);
	if(size & 0x50)
	{
		size = 1024*1024+(32*1024<<(size&0xF));
	} else {
		size = (32*1024<<(size&0xF));
	}
	unsigned char bankmask = size / 0x4000 - 1;
	romBank = bank&bankmask;
	mem[0x4] = rom->data + ROM_BANK_SIZE * romBank + 0x0000 * sizeof(gbByte);
	mem[0x5] = rom->data + ROM_BANK_SIZE * romBank + 0x1000 * sizeof(gbByte);
	mem[0x6] = rom->data + ROM_BANK_SIZE * romBank + 0x2000 * sizeof(gbByte);
	mem[0x7] = rom->data + ROM_BANK_SIZE * romBank + 0x3000 * sizeof(gbByte);
}

void GbMem::switchWramBank(unsigned int bank)
{
	// switch Wram bank
	wramBank = bank;
	mem[0xD] = wram[bank];
}

void GbMem::switchVramBank(unsigned int bank)
{
	// switch vram bank
	vramBank = bank;
	mem[0x8] = vram[bank];
	mem[0x9] = vram[bank]+0x1000*sizeof(gbByte);
}

void GbMem::switchEramBank(unsigned int bank)
{
	char ramsize = read(RAM_SIZE);
	eramBank = bank;
	if(ramsize == SIA_NONE) 
		return;
	mem[0xA] = eram[bank/2];
	if(ramsize == SIA_2KB) 
		return;
	mem[0xB] = eram[bank/2+1];
}

void GbMem::setAccessable(bool vram, bool oam)
{
	// TODO: add cgb palette data 
	this->vramAccess = vram;
	this->oamAccess = oam;
}

bool GbMem::load(rom_t *rom, rom_t *ram, rom_t *misc)
{
	// Load the rom
	this->rom = rom;

	try{
		DeleteRam();
		for(int i = 0; i< VRAM_BANKS; i++)
		{
			vram[i] = new gbByte[VRAM_BANK_SIZE]();
		}
		for(int i = 0; i< WRAM_BANKS; i++)
		{
			wram[i] = new gbByte[WRAM_BANK_SIZE]();
		}
		regs = new gbByte[REGS_BANK_SIZE]();
		mem[0x0] = rom->data+0x0000*sizeof(gbByte);
		mem[0x1] = rom->data+0x1000*sizeof(gbByte);
		mem[0x2] = rom->data+0x2000*sizeof(gbByte);
		mem[0x3] = rom->data+0x3000*sizeof(gbByte);
		switchBank(1);
		switchVramBank(0);
		mem[0xC] = wram[0];
		switchWramBank(1);
		eramBank = 0;
		switch (read(RAM_SIZE))
		{
		case SIA_NONE:
			eram = NULL;
			mem[0xA] = NULL;
			mem[0xB] = NULL;
			eramSize = 0;
			break;
		case SIA_2KB:
			// implement 4 KB because that fits in
			// TODO: improve this so that is is more real
			eram = new bank_t[1];
			eram[0] = new gbByte[ERAM_BANK_SIZE]();
			mem[0xA] = eram[0];
			mem[0xB] = NULL;
			eramSize = 1;
			break;
		case SIA_8KB:
			eram = new bank_t[2];
			eram[0] = new gbByte[ERAM_BANK_SIZE]();
			eram[1] = new gbByte[ERAM_BANK_SIZE]();
			mem[0xA] = eram[0];
			mem[0xB] = eram[1];
			eramSize = 2;
			break;
		case SIA_32KB:
			eram = new bank_t[8];
			for(int i = 0; i < 8; i++)
			{
				eram[i] = new gbByte[ERAM_BANK_SIZE]();
			}
			mem[0xA] = eram[0];
			mem[0xB] = eram[1];
			eramSize = 8;
			break;
		}

		gbByte type = read(CARTRIDGE_TYPE);
		if(type == MBC2 || type == MBC2_BAT)
		{
			eram = new bank_t[1];
			eram[0] = new gbByte[ERAM_BANK_SIZE]();
			mem[0xA] = eram[0];
			eramSize = 1;
		}
		if (ram->data != NULL && ram->length >= 0) // it is possible that there is rtc data
		{
			for (int i = 0; i < eramSize && (unsigned int)((i + 1) * ERAM_BANK_SIZE) < ram->length; i++)
			{
				memcpy(eram[i], ram->data + (i*ERAM_BANK_SIZE), ERAM_BANK_SIZE);
			}
		}
		if(misc->data != NULL && misc->length >= 16)
		{
			if((type == MBC3_TIM_BAT || type == MBC3_TIM_RAM_BAT) && 
				(ram->length >= eramSize*ERAM_BANK_SIZE + 16*sizeof(uint8_t)))
			{
				// Read RTC info:
				// little endian
				// 1 byte active
				// 1 byte seconds
				// 1 byte minutes
				// 1 byte hours
				// 4 bytes days
				// 8 bytes timestamp
				int j = eramSize * ERAM_BANK_SIZE;
				const EndianFuncs *conv = getEndianFuncs(0);
				m_rtcActive = ram->data[j++] != 0;
				m_timeStamp.seconds = ram->data[j++];
				m_timeStamp.minutes = ram->data[j++];
				m_timeStamp.hours = ram->data[j++];
				m_timeStamp.day = conv->convu32(*(uint32_t *)&ram->data[j]);
				j += 4;
				m_timeStamp.systemTime = (time_t)conv->convu64(*(uint64_t *)&ram->data[j]);
				j += 8;
			}
		}
		mem[0xe] = mem[0xc];
		mem[0xf] = regs;
		
		// Register events
		registerEvent(DMA, this);
		registerEvent(SVBK, this);
		registerEvent(HDMA5, this);
		registerEvent(VBK, this);
	} catch(std::bad_alloc){
		// Memory alloc fail
		DeleteRam();
		Log(Error,"Could not allocate memory");
		return false;
	}
	return true;
}

bool GbMem::Save(SaveData_t *data)
{
	gbByte type = read(CARTRIDGE_TYPE);
	gbByte size = read(RAM_SIZE);
	data->ramDataLen = eramSize * ERAM_BANK_SIZE;
	data->ramData = new uint8_t[data->ramDataLen];
	for(int i = 0; i < eramSize; i++)
	{
		memcpy(((char *)data->ramData) + i * ERAM_BANK_SIZE, eram[i], ERAM_BANK_SIZE);
	}
	data->miscDataLen = 0;
	data->miscData = NULL;
	if(type == MBC3_TIM_BAT || type == MBC3_TIM_RAM_BAT) // if the cardridge has a timer
	{
		// Write RTC info:
		// little endian
		// 1 byte active
		// 1 byte seconds
		// 1 byte minutes
		// 1 byte hours
		// 4 bytes days
		// 8 bytes timestamp
		data->miscDataLen = 16;
		data->miscData = new uint8_t[data->miscDataLen];
		const EndianFuncs *conv = getEndianFuncs(0);
		char active = m_rtcActive ? 0x01 : 0x00;
		uint32_t day = conv->convu32(m_timeStamp.day);
		uint64_t systemtime = conv->convu64(m_timeStamp.systemTime);
		((char *)data->miscData)[0] = active;
		((char *)data->miscData)[1] = m_timeStamp.seconds;
		((char *)data->miscData)[2] = m_timeStamp.minutes;
		((char *)data->miscData)[3] = m_timeStamp.hours;
		*(uint32_t *)((char *)data->miscData + 4) = m_timeStamp.day;
		*(uint64_t *)((char *)data->miscData + 8) = m_timeStamp.systemTime;
	}
	return true;
}

void GbMem::registerEvent(address_t address, GbMemEvent *evt)
{
	eventTable.insert(address, evt);
}

void GbMem::registerEvent(address_t rangeLow, address_t rangeHigh, GbMemEvent *evt)
{
	for(address_t i = rangeLow; i <= rangeHigh; i++)
	{
		registerEvent(i, evt);
	}
}

void GbMem::MemEvent(address_t address, gbByte val)
{
	switch(address)
	{
	case VBK:
		switchVramBank(val & 1);
		break;
	case DMA:
		{
			int i;
			if(val != 0)
			{
				for(i= 0; i < 0xA0; i++)
				{
					write(0xFE00 + i, read(((unsigned char)val<<8) + i));
				}
				write(address, 0);
			}
		}
		break;
	case SVBK:
		{
			if (val == 0) val = 1;
			switchWramBank((unsigned char)val & 7);
		}
		break;
	case HDMA5:
		{
			if(val & (1<<7)) return;
			gbByte hdma1 = read(HDMA1);
			gbByte hdma2 = read(HDMA2);
			gbByte hdma3 = read(HDMA3);
			gbByte hdma4 = read(HDMA4);
			int source = ((hdma1<<8) | hdma2)&0xFFF0;
			int destination = (((hdma3<<8) | hdma4)&0x1FF0)|0x8000;
			gbByte hdma5 = val;
			while(hdma5 != 0xFF)
			{
				for(int i = 0; i < 0x10; i++)
				{
					write(destination++,  read(source++));
				}
				hdma5--;
			}
			write(HDMA1, source >> 8, false);
			write(HDMA2, source & 0xFF, false);
			write(HDMA3, destination >> 8, false);
			write(HDMA4, destination & 0xFF, false);
			write(HDMA5, hdma5, false);
		}
		break;
	default:
		return;
	}
	return;
}

