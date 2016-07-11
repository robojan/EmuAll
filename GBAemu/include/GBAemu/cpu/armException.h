#ifndef ARMEXCEPTION_H_
#define ARMEXCEPTION_H_

#include <stdint.h>

class ArmException {
public:
	ArmException();
	~ArmException();
};

class UndefinedInstructionARMException : public ArmException {
public:
	UndefinedInstructionARMException();
	~UndefinedInstructionARMException();
};

class DataAbortARMException : public ArmException {
public:
	DataAbortARMException(uint32_t address);
	~DataAbortARMException();

	uint32_t _address;
};

class BreakPointARMException : public ArmException {
public:
	BreakPointARMException(unsigned int value);
	~BreakPointARMException();

	int _value;
};

#endif