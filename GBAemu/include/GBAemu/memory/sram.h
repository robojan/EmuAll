#ifndef SRAM_H_
#define SRAM_H_

#include <GBAemu/memory/cartridgeStorage.h>

class Memory;

class Sram : public CartridgeStorage {
public:
	explicit Sram(Memory &memory);
	virtual ~Sram();

	virtual uint32_t GetSize() override;
	virtual uint8_t ReadMemory(uint32_t address) override;
	virtual uint32_t Read32(uint32_t address) override;
	virtual uint32_t Read16(uint32_t address) override;
	virtual uint32_t Read8(uint32_t address) override;
	virtual void Write32(uint32_t address, uint32_t val) override;
	virtual void Write16(uint32_t address, uint16_t val) override;
	virtual void Write8(uint32_t address, uint8_t val) override;



private:
	uint8_t Read(uint_least16_t address);
	void Write(uint_least16_t address, uint8_t val);

	uint8_t _dataStorage[0x8000];

};

#endif