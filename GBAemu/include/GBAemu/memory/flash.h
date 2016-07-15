#ifndef FLASH_H_
#define FLASH_H_

#include <GBAemu/memory/cartridgeStorage.h>

class Memory;

class Flash : public CartridgeStorage {
public:
	explicit Flash(Memory &memory, unsigned int size);
	virtual ~Flash();

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
		ProtectCycle1,
		ProtectCycle2,
		ProtectCycle3,
		ProtectCycle4,
		ByteProgramCycle,
		SoftwareId,
		EraseCycle,
		SectorErase,
		ChipErase,
		ByteProgram,
		BankSwitching,
	};
	uint8_t Read(uint_fast16_t address);
	void Write(uint_fast16_t address, uint8_t val);
	void DoSectorErase(uint_fast16_t address);
	void DoChipErase();
	void DoBankSwitching(uint8_t val);
	void DoByteProgramming(uint_fast16_t address, uint8_t val);

	uint8_t *_dataStorage;
	uint32_t _dataSize;
	State _state;
	uint8_t _chipId[2];
	uint8_t _highAddress;
	unsigned int _sectorEraseCycles;
	unsigned int _chipEraseCycles;
	unsigned int _byteProgramCycles;
	unsigned int _actionStart;
	unsigned int _actionLength;
	uint8_t _lastRead;
};

#endif