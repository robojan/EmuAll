
#include <GBAemu/defines.h>
#include <GBAemu/gpu/gpu.h>
#include <GBAemu/gba.h>
#include <GBAemu/util/preprocessor.h>

Gpu::Gpu(Gba &system) :
	_system(system), _vcount(0), _hcount(0)
{
	uint8_t *registers = _system.GetMemory().GetRegisters();

	RegisterEvents();
}

Gpu::~Gpu()
{

}

void Gpu::Tick()
{
	if (_hcount == 308 * 4 - 1) {
		uint8_t *registers = _system.GetMemory().GetRegisters();
		_hcount = 0;
		uint16_t dispstat = IOREG16(registers, DISPSTAT);
		dispstat &= ~2;
		if (_vcount == 227) {
			_vcount = 0;
			dispstat &= ~1;
		}
		else {
			_vcount++;
			if (_vcount == 160) {
				dispstat |= 1;
				if ((dispstat & 0x8) != 0) {
					_system.RequestIRQ(IRQ_VBLANK);
				}
			}
			if (_vcount == dispstat >> 8) {
				dispstat |= 4;
				if ((dispstat & 0x20) != 0) {
					_system.RequestIRQ(IRQ_VMATCH);
				}
			}
			else {
				dispstat &= ~4;
			}
		}
		IOREG16(registers, DISPSTAT) = dispstat;
		IOREG16(registers, VCOUNT) = _vcount;
	}
	else {
		_hcount++;
		if (_hcount == 1006) {
			uint8_t *registers = _system.GetMemory().GetRegisters();
			uint16_t dispstat = IOREG16(registers, DISPSTAT);
			dispstat |= 1;
			if ((dispstat & 0x10) != 0) {
				_system.RequestIRQ(IRQ_HBLANK);
			}
		}
	}
}

void Gpu::HandleEvent(uint32_t address, int size)
{
	uint8_t *registers = _system.GetMemory().GetRegisters();
	if (address <= DISPSTAT + 1 && DISPSTAT < address + size) {
		uint8_t flags = 0;
		if (_vcount >= 160 && _vcount <= 226) flags |= 0x1;
		if (_hcount >= 1006 && _hcount <= 1232) flags |= 0x2;
		if (_vcount == registers[(DISPSTAT&IOREGISTERSMASK) + 1]) flags |= 0x4;
		IOREG16(registers, DISPSTAT) = (IOREG16(registers, DISPSTAT) & ~0x00C7) | flags;
	}
	if (address <= VCOUNT + 1 && VCOUNT < address + size) {
		IOREG16(registers, VCOUNT) = _vcount;
	}
}

void Gpu::RegisterEvents()
{
	//_system.GetMemory().RegisterEvent(DISPCNT, this);
	_system.GetMemory().RegisterEvent(DISPSTAT, this);
	//_system.GetMemory().RegisterEvent(BG0CNT, this);
	//_system.GetMemory().RegisterEvent(BG2CNT, this);
	//_system.GetMemory().RegisterEvent(BG0HOFS, this);
	//_system.GetMemory().RegisterEvent(BG1HOFS, this);
	//_system.GetMemory().RegisterEvent(BG2HOFS, this);
	//_system.GetMemory().RegisterEvent(BG3HOFS, this);

}


