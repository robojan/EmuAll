#pragma once

#include <emuall/common.h>

DLLEXPORT const char * HexToString(unsigned int data, int minPrint = 0);
DLLEXPORT void HexToString(char *dst, unsigned int data, int minPrint = 0);
DLLEXPORT void DecToString(char *dst, unsigned int data, int minPrint = 0);
