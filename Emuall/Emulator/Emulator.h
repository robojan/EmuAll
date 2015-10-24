#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <list>
#include <vector>
#include <string>
#include <stdint.h>
#include <map>
#include "../util/xml/pugixml.hpp"
#include "../gui/debugger/DebuggerRoot.h"

class Emulator;

#define EMU_INPUT_ANALOG 1
#define EMU_INPUT_PASS 2

typedef struct {
	std::string name;
	int key;
	int defaultKey;
	int flags;
} EmulatorInput_t;

typedef struct {
	std::string name;
	int id;
	int size;
	bool readonly;
} EmulatorRegister_t;

typedef struct {
	enum Mode {
		dec,
		hex,
		oct,
		flag, 
		string
	};
	std::string name;
	int id;
	int size;
	Mode mode;
} EmulatorValue_t;

typedef struct {
	std::string name;
	unsigned int size;
	int id;
} EmulatorMemory_t;

typedef struct {
	enum Type {
		Checkbox,
		RadioBox,
		Combobox,
		Numeric
	};
	std::string name;
	int id;
	Type type;
	std::map<std::string, int> options;
} EmulatorOption_t;

typedef struct {
	bool disassembler;
	bool breakpointSupport;
	unsigned int disassemblerSize;
	int curLineId;
	bool step;
	bool stepOut;
	bool stepOver;
	std::vector<EmulatorRegister_t> registers;
	std::vector<EmulatorRegister_t> flags;
} CpuDebuggerInfo_t;

typedef struct {
	std::vector<EmulatorMemory_t> memories;
	std::vector<EmulatorValue_t> infos;
	bool readOnly;
} MemDebuggerInfo_t;

typedef struct {
	int audioSources;
} EmulatorSettings_t;

typedef void *EMUHANDLE;
typedef struct
{
	void(*Log)(enum loglevel, char *, ...);
} callBackfunctions_t;

class EmulatorInterface
{
public:
	EmulatorInterface(const std::string &dllName);
	~EmulatorInterface();

	bool IsValid() {return mValid;}

	std::string GetFileFilterEntry() const;
	std::string GetName() const;
	std::list<EmulatorInput_t> GetEmulatorInputs() const;
	CpuDebuggerInfo_t GetCpuDebuggerInfo() const;
	MemDebuggerInfo_t GetMemDebuggerInfo(EMUHANDLE handle) const;
	Debugger::DebuggerRoot *GetGpuDebuggerInfo(Emulator *emu) const;
	EmulatorSettings_t GetEmulatorSettings() const;

	uint32_t GetValU(EMUHANDLE handle, int id) const;
	int32_t GetValI(EMUHANDLE handle, int id) const;
	const char *GetString(EMUHANDLE handle, int id) const;
	void SetValU(EMUHANDLE handle, int id, uint32_t val) const;
	void SetValI(EMUHANDLE handle, int id, int32_t val) const;

	EMUHANDLE CreateEmulator();
	void ReleaseEmulator(EMUHANDLE handle);
	int Init(EMUHANDLE handle, callBackfunctions_t funcs);
	int Load(EMUHANDLE handle, const char *filename);
	bool InitGL(EMUHANDLE handle, int id);
	void DestroyGL(EMUHANDLE handle, int id);

	const char *GetDescription(unsigned int *size);
	int IsCompatible(const char *filename);
	void Run(EMUHANDLE handle, int run);
	void Step(EMUHANDLE handle);
	int IsRunning(EMUHANDLE handle);

	int Tick(EMUHANDLE handle, uint32_t time);
	void Input(EMUHANDLE handle, int key, int pressed);
	void Draw(EMUHANDLE handle, int id);
	void Reshape(EMUHANDLE handle, int width, int height, bool keepAspect);

	int Save(EMUHANDLE handle, const char *filename);

	// Debugging
	char Disassemble(EMUHANDLE handle, unsigned int pos, const char **raw, const char **instr);
	void AddBreakpoint(EMUHANDLE handle, unsigned int pos);
	void RemoveBreakpoint(EMUHANDLE handle, unsigned int pos);
	int IsBreakpoint(EMUHANDLE handle, unsigned int pos);
	char GetMemoryData(EMUHANDLE handle, int memory, unsigned int address);
private:
	unsigned int GetXMLVal(EMUHANDLE handle, pugi::xml_node &node, const char *element) const;

	void *mhDLL;
	bool mValid;
	pugi::xml_document mDescription;
	pugi::xml_node mRoot;



	// DLL functions
	// Initialization functions
	EMUHANDLE( __stdcall *mCreateEmulator)();
	void(__stdcall *mReleaseEmulator)(EMUHANDLE);
	int32_t(__stdcall *mInit)(EMUHANDLE, callBackfunctions_t);
	int32_t(__stdcall *mLoad)(EMUHANDLE, const uint8_t *);
	uint32_t(__stdcall *mInitGL)(EMUHANDLE, int32_t);
	void(__stdcall *mDestroyGL)(EMUHANDLE, int32_t);

	// Misc functions
	int32_t(__stdcall *mGetValI)(EMUHANDLE, int32_t);
	uint32_t(__stdcall *mGetValU)(EMUHANDLE, int32_t);
	const uint8_t *(__stdcall *mGetString)(EMUHANDLE, int32_t);
	void(__stdcall *mSetValI)(EMUHANDLE, int32_t, int32_t);
	void(__stdcall *mSetValU)(EMUHANDLE, int32_t, uint32_t);
	const uint8_t *(__stdcall *mGetDescription)(uint32_t *);
	int32_t(__stdcall *mIsCompatible)(const uint8_t *);
	int32_t(__stdcall *mGetInterfaceVersion)();
	void(__stdcall *mRun)(EMUHANDLE, int32_t);
	void(__stdcall *mStep)(EMUHANDLE);
	int32_t(__stdcall *mIsRunning)(EMUHANDLE);

	// Running functions
	int32_t(__stdcall *mTick)(EMUHANDLE, uint32_t);
	void(__stdcall *mInput)(EMUHANDLE, int32_t, int32_t);
	void(__stdcall *mDraw)(EMUHANDLE, int32_t);
	void(__stdcall *mReshape)(EMUHANDLE, int32_t, int32_t, int32_t);

	// stopping functions
	int32_t(__stdcall *mSave)(EMUHANDLE, const uint8_t *);

	// Debugging functions
	uint8_t(__stdcall *mDisassemble)(EMUHANDLE, uint32_t, const uint8_t **, const uint8_t **);
	void(__stdcall *mAddBreakpoint)(EMUHANDLE, uint32_t);
	void(__stdcall *mRemoveBreakpoint)(EMUHANDLE, uint32_t);
	int32_t(__stdcall *mIsBreakpoint)(EMUHANDLE, uint32_t);
	uint8_t(__stdcall *mGetMemoryData)(EMUHANDLE, int32_t, uint32_t);
};

class Emulator
{
public:
	EmulatorInterface *emu;
	EMUHANDLE handle;

	bool operator==(const Emulator &st)
	{
		return emu == st.emu && handle == st.handle;
	}
};

class EmulatorList
{
public:
	EmulatorList(const char *folder);
	EmulatorList();
	~EmulatorList();

	std::list<EmulatorInterface *>::iterator begin();
	std::list<EmulatorInterface *>::iterator end();

	EmulatorInterface *GetCompatibleEmulator(const char *fileName) const;
	std::string GetFileFilters() const;
private:
	std::list<EmulatorInterface *> _emulators;
};

#endif