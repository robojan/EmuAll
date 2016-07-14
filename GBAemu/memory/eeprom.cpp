
#include <stdint.h>
#include <GBAemu/memory/eeprom.h>
#include <GBAemu/util/log.h>
#include <GBAemu/cpu/armException.h>

Eeprom::Eeprom(Memory *memory) : 
	_memory(memory), _state(Idle), _address(0), _counter(0), _actionTimeStart(0)
{
	memset(_dataStorage, 0xFF, sizeof(_dataStorage));
}

Eeprom::~Eeprom()
{

}

uint32_t Eeprom::GetSize()
{
	return 0x400;
}

uint8_t Eeprom::ReadMemory(uint32_t address)
{
	if (address >= GetSize()) {
		throw DataAbortARMException(address);
	}
	return _dataStorage[address];
}

uint32_t Eeprom::Read32(uint32_t address)
{
	if (address >> 24 == 0xd) {
		return Read();
	}
	else {
		return CartridgeStorage::Read32(address);
	}
}

uint32_t Eeprom::Read16(uint32_t address)
{
	if (address >> 24 == 0xd) {
		return Read();
	}
	else {
		return CartridgeStorage::Read16(address);
	}
}

uint32_t Eeprom::Read8(uint32_t address)
{
	if (address >> 24 == 0xd) {
		return Read();
	}
	else {
		return CartridgeStorage::Read8(address);
	}
}

void Eeprom::Write32(uint32_t address, uint32_t val)
{
	if (address >> 24 == 0xd) {
		Write(val & 1);
	}
	else {
		CartridgeStorage::Write32(address, val);
	}
}

void Eeprom::Write16(uint32_t address, uint16_t val)
{
	if (address >> 24 == 0xd) {
		Write(val & 1);
	}
	else {
		CartridgeStorage::Write16(address, val);
	}
}

void Eeprom::Write8(uint32_t address, uint8_t val)
{
	if (address >> 24 == 0xd) {
		Write(val & 1);
	}
	else {
		CartridgeStorage::Write8(address, val);
	}
}

void Eeprom::Write(int bit)
{
	switch (_state) {
	case Idle: {
		_counter = 1;
		_address = bit;
		// Check if dma access is occurring and dma size
		int dmaSize = GetDMAAccessSize();
		if (dmaSize > 0) {
			_state = ReadAddress;
			_dmaAccessSize = dmaSize;
			if (dmaSize == 9 || dmaSize == 73) {
				_addressSize = 6;
			}
			else {
				_addressSize = 14;
			}
		}
		else {
			Log(Warn, "Non valid DMA access to EEPROM");
		}
		break;
	}
	case ReadAddress: {
		_counter++;
		_address = (_address << 1) | bit;
		if (_counter == _addressSize + 2) {
			int cmd = (_address >> _addressSize);
			_address = _address & ((1 << _addressSize) - 1);
			_address = ((_address & 0x3FF) << 3);
			if (cmd == 3) {
				Log(Debug, "EEPROM Read request addr=0x%x", _address);
				_state = ReadData;
			}
			else if (cmd == 2) {
				_state = WriteData;
			}
			_counter = 0;
		}
		break;
	}
	case ReadData: {
		if (bit != 0) {
			Log(Warn, "EEPROM: expected 0 bit in data read");
		}
		_state = Reading;
		_counter = -4;
		break;
	}
	case WriteData:{
		if (_counter < 64) {
			_dataBuffer[7 - _counter / 8] = (_dataBuffer[7 - _counter / 8] << 1) | bit;
		}
		else if (_counter == 64) {
			if (bit != 0) {
				Log(Warn, "EEPROM: expected 0 bit in data write at pos 65");
			}
			Log(Debug, "EEPROM Write request addr=0x%x, data=%02x %02x %02x %02x %02x %02x %02x %02x", _address,
				_dataBuffer[0], _dataBuffer[1], _dataBuffer[2], _dataBuffer[3],
				_dataBuffer[4], _dataBuffer[5], _dataBuffer[6], _dataBuffer[7]);
			memcpy(_dataStorage + _address, _dataBuffer, 8);
			_state = Writing;
			_actionTimeStart = _memory->_tickCounter;
		}
		_counter++;
		break;
	}
	case Writing: {
		if (_memory->_tickCounter - _actionTimeStart > 10836) {
			_state = Idle;
			Write(bit);
		}
		else {
			Log(Warn, "EEPROM: Writing command while writing");
		}
		break;
	}
	case Reading: {
		Log(Warn, "EEPROM: Unexpected write during reading");
		break;
	}
	default:
		break;

	}
}

int Eeprom::Read()
{
	switch (_state) {
	default:
	case WriteData:
	case ReadAddress:
	case Idle: {
		return 1;
	}
	case ReadData:
		return 1;
	case Writing:
		if (_memory->_tickCounter - _actionTimeStart > 108368) {
			_state = Idle;
			return 1;
		}
		return 0;
	case Reading: {
		int bit;
		if (_counter >= 0) {
			bit = (_dataStorage[7 - _counter / 8 + _address] >> (7 - (_counter & 7))) & 1;
		}
		else {
			bit = 1;
		}
		_counter++;
		return bit;
	}
	}
}

int Eeprom::GetDMAAccessSize()
{
	if (_memory->_dma[3].active && (_memory->_dma[3].dst >> 24) == 0xD) {
		return _memory->_dma[3].wc;
	}
	return 0;
}
