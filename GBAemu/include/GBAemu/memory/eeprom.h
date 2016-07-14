#ifndef EEPROM_H_
#define EEPROM_H_

#include <GBAemu/memory/cartridgeStorage.h>
#include <GBAemu/memory/memory.h>

class Eeprom : public CartridgeStorage {
public:
	explicit Eeprom(Memory *memory);
	virtual ~Eeprom();

	virtual uint32_t GetSize() override;
	virtual uint8_t ReadMemory(uint32_t address) override;
	virtual uint32_t Read32(uint32_t address) override;
	virtual uint32_t Read16(uint32_t address) override;
	virtual uint32_t Read8(uint32_t address) override;
	virtual void Write32(uint32_t address, uint32_t val) override;
	virtual void Write16(uint32_t address, uint16_t val) override;
	virtual void Write8(uint32_t address, uint8_t val) override;

private:
	enum State {
		Idle,
		ReadAddress,
		ReadData,
		WriteData,
		Writing,
		Reading,
	};

	void Write(int bit);
	int Read();
	int GetDMAAccessSize();


	Memory *_memory;
	State _state;
	uint16_t _address;
	int _counter;
	int _dmaAccessSize;
	int _addressSize;
	uint8_t _dataBuffer[8];
	uint32_t _actionTimeStart;

	uint8_t _dataStorage[0x2000];

};

#endif