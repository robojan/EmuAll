
#include <stdint.h>
#include <GBAemu/cpu/armException.h>

ArmException::ArmException()
{

}

ArmException::~ArmException()
{

}

UndefinedInstructionARMException::UndefinedInstructionARMException()
{

}

UndefinedInstructionARMException::~UndefinedInstructionARMException()
{

}

DataAbortARMException::DataAbortARMException(uint32_t address) :
	_address(address)
{

}

DataAbortARMException::~DataAbortARMException()
{

}

BreakPointARMException::BreakPointARMException(unsigned int value) :
	_value(value)
{

}

BreakPointARMException::~BreakPointARMException()
{

}
