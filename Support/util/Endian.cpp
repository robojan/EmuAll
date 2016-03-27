#include <emuall/util/endian.h>

uint16_t swapUInt16(uint16_t i)
{
	union
	{
		uint16_t i;
		uint8_t b[2];
	} dat1, dat2;
	dat1.i = i;

	dat2.b[0] = dat1.b[1];
	dat2.b[1] = dat1.b[0];

	return dat2.i;
}

uint16_t noSwapUInt16(uint16_t i)
{
	return i;
}

uint32_t swapUInt32(uint32_t i)
{
	union
	{
		uint32_t i;
		uint8_t b[4];
	} dat1, dat2;
	dat1.i = i;

	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];

	return dat2.i;
}

uint32_t noSwapUInt32(uint32_t i)
{
	return i;
}

uint64_t swapUInt64(uint64_t i)
{
	union
	{
		uint64_t i;
		uint8_t b[8];
	} dat1, dat2;
	dat1.i = i;

	dat2.b[0] = dat1.b[7];
	dat2.b[1] = dat1.b[6];
	dat2.b[2] = dat1.b[5];
	dat2.b[3] = dat1.b[4];
	dat2.b[4] = dat1.b[3];
	dat2.b[5] = dat1.b[2];
	dat2.b[6] = dat1.b[1];
	dat2.b[7] = dat1.b[0];

	return dat2.i;
}

uint64_t noSwapUInt64(uint64_t i)
{
	return i;
}

int16_t swapInt16(int16_t i)
{
	union
	{
		int16_t i;
		uint8_t b[2];
	} dat1, dat2;
	dat1.i = i;

	dat2.b[0] = dat1.b[1];
	dat2.b[1] = dat1.b[0];

	return dat2.i;
}

int16_t noSwapInt16(int16_t i)
{
	return i;
}

int32_t swapInt32(int32_t i)
{
	union
	{
		int32_t i;
		uint8_t b[4];
	} dat1, dat2;
	dat1.i = i;

	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];

	return dat2.i;
}

int32_t noSwapInt32(int32_t i)
{
	return i;
}

int64_t swapInt64(int64_t i)
{
	union
	{
		int64_t i;
		uint8_t b[8];
	} dat1, dat2;
	dat1.i = i;

	dat2.b[0] = dat1.b[7];
	dat2.b[1] = dat1.b[6];
	dat2.b[2] = dat1.b[5];
	dat2.b[3] = dat1.b[4];
	dat2.b[4] = dat1.b[3];
	dat2.b[5] = dat1.b[2];
	dat2.b[6] = dat1.b[1];
	dat2.b[7] = dat1.b[0];

	return dat2.i;
}

int64_t noSwapInt64(int64_t i)
{
	return i;
}

float swapFloat(float f)
{
	union
	{
		float f;
		uint8_t b[4];
	} dat1, dat2;
	dat1.f = f;

	dat2.b[0] = dat1.b[3];
	dat2.b[1] = dat1.b[2];
	dat2.b[2] = dat1.b[1];
	dat2.b[3] = dat1.b[0];

	return dat2.f;
}

float noSwapFloat(float f)
{
	return f;
}

double swapDouble(double d)
{
	union
	{
		double d;
		uint8_t b[8];
	} dat1, dat2;
	dat1.d = d;

	dat2.b[0] = dat1.b[7];
	dat2.b[1] = dat1.b[6];
	dat2.b[2] = dat1.b[5];
	dat2.b[3] = dat1.b[4];
	dat2.b[4] = dat1.b[3];
	dat2.b[5] = dat1.b[2];
	dat2.b[6] = dat1.b[1];
	dat2.b[7] = dat1.b[0];

	return dat2.d;
}

double noSwapDouble(double d)
{
	return d;
}

const uint8_t _endiannessTest[2] = { 0x01, 0x00 };
/*
const Endian _endianSwap = {
	swapUInt16, swapInt16,
	swapUInt32, swapInt32,
	swapUInt64, swapInt64,
	swapFloat, swapDouble
};
const Endian _endianNoSwap = {
	noSwapUInt16, noSwapInt16,
	noSwapUInt32, noSwapInt32,
	noSwapUInt64, noSwapInt64,
	noSwapFloat, noSwapDouble
};*/


Endian::Endian(bool bigEndian)
{
	if ((*(short *)_endiannessTest == 1/*Little Endian*/) && (!bigEndian))
	{
		convu16 = noSwapUInt16;
		convu32 = noSwapUInt32;
		convu64 = noSwapUInt64;
		convi16 = noSwapInt16;
		convi32 = noSwapInt32;
		convi64 = noSwapInt64;
		convf = noSwapFloat;
		convd = noSwapDouble;
	}
	else {
		convu16 = swapUInt16;
		convu32 = swapUInt32;
		convu64 = swapUInt64;
		convi16 = swapInt16;
		convi32 = swapInt32;
		convi64 = swapInt64;
		convf = swapFloat;
		convd = swapDouble;
	}
}

bool Endian::isHostLittleEndian()
{
	return (*(short *)_endiannessTest == 1/*Little Endian*/);
}

const Endian * Endian::getSingleton(bool bigEndian)
{
	return bigEndian ? &_BEConv : &_LEConv;
}

Endian Endian::_BEConv(true);
Endian Endian::_LEConv(false);
