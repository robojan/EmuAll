#include <emuall/math/math.h>

int nextPowerOf2(int x)
{
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

bool isPowerOf2(int x)
{
	return (x & (x - 1)) == 0;
} 
 