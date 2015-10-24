#ifndef _GENERAL_H
#define _GENERAL_H

typedef union {
	uint8_t ui8;
	uint16_t ui16;
	uint32_t ui32;
	uint64_t ui64;
	int8_t i8;
	int16_t i16;
	int32_t i32;
	int64_t i64;
	float f;
	double d;
} general_t;

#endif