#ifndef _ENDIAN_H
#define _ENDIAN_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{

	uint16_t(*convu16)(uint16_t i);
	int16_t(*convi16)(int16_t i);
	uint32_t(*convu32)(uint32_t i);
	int32_t(*convi32)(int32_t i);
	uint64_t(*convu64)(uint64_t i);
	int64_t(*convi64)(int64_t i);
	float(*convf)(float f);
	double(*convd)(double d);
} EndianFuncs;

const EndianFuncs *getEndianFuncs(char bigEndian);
int isHostLittleEndian();

#ifdef __cplusplus
}
#endif

#endif