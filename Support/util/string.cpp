
#include <emuall/util/string.h>
#include <sstream>
#include <iomanip>

static const char hexChars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

const char *HexToString(unsigned int data, int minPrint)
{
	static char buffer[9] = { 0 };
	int i = 7;
	do
	{
		buffer[i] = hexChars[data & 0xF];
		data >>= 4;
		--i;
		--minPrint;
	} while ((data || minPrint > 0) && i >= 0);
	return &buffer[i + 1];
}

void HexToString(char *dst, unsigned int data, int minPrint /*= 0*/)
{
	std::ostringstream oss;
	oss << std::hex << std::setw(minPrint) << std::setfill('0') << data;
	strcpy(dst, oss.str().c_str());
}

void DecToString(char *dst, unsigned int data, int minPrint /*= 0*/)
{
	std::ostringstream oss;
	oss << std::setw(minPrint) << std::setfill('0') << data;
	strcpy(dst, oss.str().c_str());
}
