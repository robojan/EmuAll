#pragma once

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
	DataAbortARMException();
	~DataAbortARMException();
};

class BreakPointARMException : public ArmException {
public:
	BreakPointARMException(unsigned int value);
	~BreakPointARMException();

	int _value;
};