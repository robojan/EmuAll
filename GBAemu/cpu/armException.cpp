
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

DataAbortARMException::DataAbortARMException()
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
