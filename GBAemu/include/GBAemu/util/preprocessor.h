#pragma once

#define UNROLL1(x) x
#define UNROLL2(x) \
	UNROLL1(x) \
	UNROLL1(x)
#define UNROLL4(x) \
	UNROLL2(x) \
	UNROLL2(x)
#define UNROLL8(x) \
	UNROLL4(x) \
	UNROLL4(x)
#define UNROLL16(x) \
	UNROLL8(x) \
	UNROLL8(x)
#define UNROLL32(x) \
	UNROLL16(x) \
	UNROLL16(x)
#define UNROLL64(x) \
	UNROLL32(x) \
	UNROLL32(x)
#define UNROLL128(x) \
	UNROLL64(x) \
	UNROLL64(x)
#define UNROLL256(x) \
	UNROLL128(x) \
	UNROLL128(x)

#define CASE_RANGE1(x) CASE_RANGE1_OFFSET(x, 0)
#define CASE_RANGE2(x) CASE_RANGE2_OFFSET(x, 0)
#define CASE_RANGE4(x) CASE_RANGE4_OFFSET(x, 0)
#define CASE_RANGE8(x) CASE_RANGE8_OFFSET(x, 0)
#define CASE_RANGE16(x) CASE_RANGE16_OFFSET(x, 0)
#define CASE_RANGE32(x) CASE_RANGE32_OFFSET(x, 0)
#define CASE_RANGE64(x) CASE_RANGE64_OFFSET(x, 0)
#define CASE_RANGE128(x) CASE_RANGE128_OFFSET(x, 0)
#define CASE_RANGE256(x) CASE_RANGE256_OFFSET(x, 0)

#define CASE_RANGE1_OFFSET(x, o) case (x):
#define CASE_RANGE2_OFFSET(x, o) \
	CASE_RANGE1_OFFSET(x, o) \
	CASE_RANGE1_OFFSET(x + (1 << o), 0) 
#define CASE_RANGE4_OFFSET(x, o) \
	CASE_RANGE2_OFFSET(x, o) \
	CASE_RANGE2_OFFSET(x + (2 << o), o) 
#define CASE_RANGE8_OFFSET(x, o) \
	CASE_RANGE4_OFFSET(x, o) \
	CASE_RANGE4_OFFSET(x + (4 << o), o) 
#define CASE_RANGE16_OFFSET(x, o) \
	CASE_RANGE8_OFFSET(x, o) \
	CASE_RANGE8_OFFSET(x + (8 << o), o) 
#define CASE_RANGE32_OFFSET(x, o) \
	CASE_RANGE16_OFFSET(x, o) \
	CASE_RANGE16_OFFSET(x + (16 << o), o) 
#define CASE_RANGE64_OFFSET(x, o) \
	CASE_RANGE32_OFFSET(x, o) \
	CASE_RANGE32_OFFSET(x + (32 << o), o) 
#define CASE_RANGE128_OFFSET(x, o) \
	CASE_RANGE64_OFFSET(x, o) \
	CASE_RANGE64_OFFSET(x + (64 << o), o) 
#define CASE_RANGE256_OFFSET(x, o) \
	CASE_RANGE128_OFFSET(x, o) \
	CASE_RANGE128_OFFSET(x + (128 << o), o)