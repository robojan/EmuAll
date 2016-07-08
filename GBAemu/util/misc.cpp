
#include <GBAemu/util/FixedPointGBA.h>

float GetFixedPoint28(int32_t val) {
	// sign extend
	if (val & (1 << 27)) val |= 0xF0000000;
	return float(val) / 256.0f;
}

float GetFixedPoint16(int16_t val) {
	return float(val) / 256.0f;
}