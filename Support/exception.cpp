
#include <emuall/exception.h>
#include <sstream>
#include <iomanip>

#ifdef _MSC_VER
#include <windows.h>
#include <DbgHelp.h>
#endif

BaseException::BaseException(const std::string &msg) :
	 _msg(nullptr), _stacktrace(nullptr)
{
	CreateStacktrace();
}

BaseException::BaseException(const char *msg) :
	_msg(nullptr), _stacktrace(nullptr)
{
	_msg = new std::string(msg);
	_stacktrace = new std::string;
	CreateStacktrace();
}

BaseException::BaseException(BaseException &other)
{
	_msg = new std::string(*other._msg);
	_stacktrace = new std::string(*other._stacktrace);
}

BaseException & BaseException::operator=(BaseException &other)
{
	_msg = new std::string(*other._msg);
	_stacktrace = new std::string(*other._stacktrace);
	return *this;
}

BaseException::~BaseException()
{
	if (_msg != nullptr) delete _msg;
	if (_stacktrace != nullptr) delete _stacktrace;
}

const char * BaseException::GetMsg() const
{
	return _msg->c_str();
}

const char * BaseException::GetStacktrace() const
{
	return _stacktrace->c_str();
}

#define MAX_STACK_FRAMES 60
#define MAX_FUNCTION_NAME_LENGTH 256

void BaseException::CreateStacktrace()
{
	std::stringstream ss;
#ifdef _MSC_VER
	void *stack[60];
	USHORT frames = CaptureStackBackTrace(3, MAX_STACK_FRAMES, stack, NULL);
	HANDLE process = GetCurrentProcess();
	if (SymInitialize(process, NULL, TRUE) == FALSE) {
		*_stacktrace = "Could not generate stack trace";
		return;
	}
	size_t symbolLength = sizeof(SYMBOL_INFO) + (MAX_FUNCTION_NAME_LENGTH - 1) * sizeof(TCHAR);
	SYMBOL_INFO *symbol = (SYMBOL_INFO *)malloc(symbolLength);
	ZeroMemory(symbol, symbolLength);
	symbol->MaxNameLen = MAX_FUNCTION_NAME_LENGTH;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	DWORD displacement;
	IMAGEHLP_LINE64 line;
	ZeroMemory(&line, sizeof(IMAGEHLP_LINE64));
	line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
	for (int i = 0; i < frames; ++i) {
		DWORD64 address = (DWORD64)(stack[i]);
		SymFromAddr(process, address, NULL, symbol);
		if (SymGetLineFromAddr64(process, address, &displacement, &line)) {
			ss << "\tat " << symbol->Name;
			ss << " in " << line.FileName;
			ss << ":" << line.LineNumber; 
			ss << "(address: 0x";
			ss << std::setfill('0') << std::setw(sizeof(ULONG64) * 2) << std::hex;
			ss << symbol->Address << std::dec << ")" << std::endl;
		}
		else {
			ss << "\tat " << symbol->Name << "(address: 0x";
			ss << std::setfill('0') << std::setw(sizeof(ULONG64) * 2) << std::hex;
			ss << symbol->Address << ")" << std::endl;
		}
	}
	SymCleanup(process);
	free(symbol);
	*_stacktrace = ss.str();
#else 
	*_stacktrace = "Not implemented";
#endif
}

