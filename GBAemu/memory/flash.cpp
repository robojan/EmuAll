
#include <GBAemu/memory/flash.h>
#include <GBAemu/memory/memory.h>
#include <GBAemu/cpu/armException.h>
#include <GBAemu/util/log.h>
#include <GBAemu/defines.h>
#include <string.h>

Flash::Flash(Memory &memory, unsigned int size) :
	CartridgeStorage(memory), _dataSize(size), _dataStorage(new uint8_t[size]), 
	_state(Idle), _highAddress(0)
{
	// Set chipID, Emulating LE39FW512 / LE39FW010
	_chipId[0] = 0xBF;
	if (size == 128 * 1024) {
		_chipId[1] = 0xD5;
	}
	else if(size == 64 * 1024) {
		_chipId[1] = 0xD4;
	}
	else {
		Log(Error, "Invalid Flash size %u", size);
		_chipId[1] = 0xD5;
	}
	_chipEraseCycles = (100UL * FCPU) / 1000UL; // 100ms
	_sectorEraseCycles = (25UL * FCPU) / 1000UL; // 25ms
	_byteProgramCycles = (20UL * FCPU) / 1000000UL; // 20 us

	memset(_dataStorage, 0xFF, size);
}

Flash::~Flash()
{
	if (_dataStorage != nullptr) {
		delete[] _dataStorage;
		_dataStorage = nullptr;
	}
}

uint32_t Flash::GetSize()
{
	return _dataSize;
}

uint8_t Flash::ReadMemory(uint32_t address)
{
	if (address < _dataSize) {
		return _dataStorage[address];
	}
	else {
		throw DataAbortARMException(address);
	}
}

uint32_t Flash::Read32(uint32_t address)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "32 bit read from Flash");
		return Read(address);
	}
	return CartridgeStorage::Read32(address);
}

uint32_t Flash::Read16(uint32_t address)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "16 bit read from Flash");
		return Read(address);
	}
	return CartridgeStorage::Read16(address);
}

uint32_t Flash::Read8(uint32_t address)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		return Read(address);
	}
	return CartridgeStorage::Read8(address);
}

void Flash::Write32(uint32_t address, uint32_t val)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "32 bit write to Flash");
		Write(address, val & 0xFF);
	}
	else {
		CartridgeStorage::Write32(address, val);
	}
}

void Flash::Write16(uint32_t address, uint16_t val)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Log(Warn, "16 bit write to Flash");
		Write(address, val & 0xFF);
	}
	else {
		CartridgeStorage::Write16(address, val);
	}
}

void Flash::Write8(uint32_t address, uint8_t val)
{
	int index = (address >> 24) & 0xF;
	if (index == 0xe) {
		Write(address, val);
	}
	else {
		CartridgeStorage::Write8(address, val);
	}
}

uint8_t Flash::Read(uint_fast16_t address)
{
	address = address & (_dataSize - 1);
	switch (_state) {
	case ProtectCycle1:
	case ProtectCycle2:
	case ProtectCycle3:
	case ProtectCycle4:
	case ByteProgramCycle:
	case EraseCycle:
	case BankSwitching:
		_state = Idle;
	case Idle:
		return _dataStorage[(_highAddress << 16) | address];
	case SoftwareId:
		return _chipId[address & 1];
	case SectorErase:
	case ChipErase:
	case ByteProgram:
		if (_memory._tickCounter - _actionStart > _actionLength) {
			_state = Idle;
			return Read(address);
		}
		else {
			_lastRead ^= (1 << 6);
			return _lastRead;
		}
		break;
	}
	return 0;
}

void Flash::Write(uint_fast16_t address, uint8_t val)
{
	address = address & (_dataSize - 1);
	switch (_state) {
	case Idle:
		if (address == 0x5555 && val == 0xAA) {
			_state = ProtectCycle1;
		}
		break;
	case ProtectCycle1:
		if (address == 0x2AAA && val == 0x55) {
			_state = ProtectCycle2;
		}
		else {
			_state = Idle;
		}
		break;
	case ProtectCycle2:
		if (address == 0x5555 && val == 0xA0) {
			_state = ByteProgramCycle;
		}
		else if (address == 0x5555 && val == 0x80) {
			_state = ProtectCycle3;
		}
		else if (address == 0x5555 && val == 0x90) {
			_state = SoftwareId;
			Log(Debug, "FLASH: Entering software ID mode");
		}
		else if (address == 0x5555 && val == 0xB0) {
			_state = BankSwitching;
		}
		else {
			_state = Idle;
		}
		break;
	case ProtectCycle3:
		if (address == 0x5555 && val == 0xAA) {
			_state = ProtectCycle4;
		}
		else {
			_state = Idle;
		}
		break;
	case ProtectCycle4:
		if (address == 0x2AAA && val == 0x55) {
			_state = EraseCycle;
		}
		else {
			_state = Idle;
		}
		break;
	case SoftwareId:
		if(val == 0xF0) {
			_state = Idle;
			Log(Debug, "FLASH: Exiting software ID mode");
		}
		else if (address == 0x5555 && val == 0xAA) {
			_state = ProtectCycle1;
			Log(Debug, "FLASH: Exiting software ID mode");
		}
		break;
	case EraseCycle:
		if (val == 0x30) {
			_state = SectorErase;
			Log(Debug, "FLASH: Sector erase SA: 0x%04x", address);
			DoSectorErase(address);
		}
		else if (val == 0x10 && address == 0x5555) {
			_state = ChipErase;
			Log(Debug, "FLASH: Chip erase");
		}
		break;
	case ByteProgramCycle:
		Log(Debug, "FLASH: Byte program 0x%04x=0x%02x", address, val);
		DoByteProgramming(address, val);
		_state = ByteProgram;
		break;
	case BankSwitching:
		Log(Debug, "FLASH: Bank switching: %d", val);
		DoBankSwitching(val);
		_state = Idle;
		break;
	case SectorErase:
	case ChipErase:
	case ByteProgram:
		if (_memory._tickCounter - _actionStart > _actionLength) {
			_state = Idle;
			Write(address, val);
		}
		break;
	}
}

void Flash::DoSectorErase(uint_fast16_t address)
{
	address &= 0xF000;
	memset(_dataStorage + ((_highAddress<<16) | address), 0xFF, 0x1000);
	_actionStart = _memory._tickCounter;
	_actionLength = _sectorEraseCycles;
	_lastRead = 0;
}

void Flash::DoChipErase()
{
	memset(_dataStorage, 0xFF, _dataSize);
	_actionStart = _memory._tickCounter;
	_actionLength = _chipEraseCycles;
	_lastRead = 0;
}

void Flash::DoBankSwitching(uint8_t val)
{
	int mask = (_dataSize - 1) >> 16;
	_highAddress = val & mask;
}

void Flash::DoByteProgramming(uint_fast16_t address, uint8_t val)
{
	_dataStorage[(_highAddress << 16) | address] &= val; // can only make bits 0
	_actionStart = _memory._tickCounter;
	_actionLength = _byteProgramCycles;
	_lastRead = ~val;
}

