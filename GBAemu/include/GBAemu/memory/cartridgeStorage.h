#ifndef CARTRIDGESTORAGE_H_
#define CARTRIDGESTORAGE_H_

class CartridgeStorage {
public:
	explicit CartridgeStorage();
	virtual ~CartridgeStorage();

	virtual uint32_t GetSize();
	virtual uint32_t Read32(uint32_t address);
	virtual uint32_t Read16(uint32_t address);
	virtual uint32_t Read8(uint32_t address);
	virtual void Write32(uint32_t address, uint32_t val);
	virtual void Write16(uint32_t address, uint16_t val);
	virtual void Write8(uint32_t address, uint8_t val);
	
};

#endif