#ifndef EEPROM_H_
#define EEPROM_H_

#include <GBAemu/memory/cartridgeStorage.h>

class Eeprom : public CartridgeStorage {
public:
	explicit Eeprom();
	virtual ~Eeprom();

	virtual uint32_t GetSize() override;
	virtual uint32_t Read32(uint32_t address) override;
	virtual uint32_t Read16(uint32_t address) override;
	virtual uint32_t Read8(uint32_t address) override;
	virtual void Write32(uint32_t address, uint32_t val) override;
	virtual void Write16(uint32_t address, uint16_t val) override;
	virtual void Write8(uint32_t address, uint8_t val) override;

};

#endif