#ifndef _GBDIS_H_
#define _GBDIS_H_

#include "../Gameboy.h"
#include <map>

typedef struct {
	unsigned int location;
	char size;
	std::string raw;
	std::string instr;
} DisassembleLine_t;

class GbDis
{
public:
	GbDis(Gameboy *master);
	~GbDis();

	char Disassemble(const char **rawStr, const char **instrStr, address_t address);
	char Disassemble(const char **rawStr, const char **instrStr, const unsigned char *data);

private:
	Gameboy *_master;
};

#endif