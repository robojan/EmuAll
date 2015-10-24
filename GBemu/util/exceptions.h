#ifndef _EXCEPTIONS_H_
#define _EXCEPTIONS_H_

#include <exception>
#include "../gb/Gameboy.h"

class BreakPointException : public std::exception
{
public:
	BreakPointException(address_t address) :
		_address(address)
	{}
	virtual const char *what() const throw()
	{
		return "Breakpoint exception";
	}

	address_t GetAddress() const throw()
	{
		return _address;
	}
private:
	address_t _address;
};

#endif