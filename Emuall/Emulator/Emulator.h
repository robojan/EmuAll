#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <list>
#include <vector>
#include <string>
#include <stdint.h>
#include <map>
#include "../util/xml/pugixml.hpp"
#include "../gui/debugger/DebuggerRoot.h"
#include <emu.h>

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
	int id;
	int width;
	int height;
	int mouseXvarID;
	int mouseYvarID;
} EmulatorScreen_t;

typedef struct {
	std::string name;
	std::string fileFilterString;
	std::string description;
	std::string aboutInfo;
	std::vector <EmulatorScreen_t> screens;
	int numAudioStreams;
	bool screenAutoRefresh;
} EmulatorInfo_t;

class EmulatorInterface
{
public:
	EmulatorInterface(const std::string &dllName);
	~EmulatorInterface();

	bool IsValid() {return _valid;}

	EmulatorInfo_t GetInfo() const;
	std::string GetFileFilterEntry() const;
	std::string GetName() const;
	std::list<EmulatorInput_t> GetEmulatorInputs() const;
	CpuDebuggerInfo_t GetCpuDebuggerInfo() const;
	MemDebuggerInfo_t GetMemDebuggerInfo(EMUHANDLE handle) const;
	Debugger::DebuggerRoot *GetGpuDebuggerInfo(Emulator *emu) const;

	uint32_t GetValU(EMUHANDLE handle, int id) const;
	int32_t GetValI(EMUHANDLE handle, int id) const;
	const char *GetString(EMUHANDLE handle, int id) const;
	void SetValU(EMUHANDLE handle, int id, uint32_t val) const;
	void SetValI(EMUHANDLE handle, int id, int32_t val) const;
	float GetFloat(EMUHANDLE handle, int id) const;

	EMUHANDLE CreateEmulator();
	void ReleaseEmulator(EMUHANDLE handle);
	int Init(EMUHANDLE handle, callBackfunctions_t funcs);
	int Load(EMUHANDLE handle, const SaveData_t *data);
	int LoadState(EMUHANDLE handle, const SaveData_t *state);
	bool InitGL(EMUHANDLE handle, int id);
	void DestroyGL(EMUHANDLE handle, int id);
	void ReleaseSaveData(EMUHANDLE handle, SaveData_t *data);

	const char *GetDescription(unsigned int *size);
	int IsCompatible(const char *filename);
	void Run(EMUHANDLE handle, int run);
	void Step(EMUHANDLE handle);
	int IsRunning(EMUHANDLE handle);

	int Tick(EMUHANDLE handle, uint32_t time);
	void Input(EMUHANDLE handle, int key, int pressed);
	void Draw(EMUHANDLE handle, int id);
	void Reshape(EMUHANDLE handle, int id, int width, int height, bool keepAspect);

	int Save(EMUHANDLE handle, SaveData_t* data);
	int SaveState(EMUHANDLE handle, SaveData_t *state);

	void InitAudio(EMUHANDLE handle, int source, unsigned int sampleRate, int channels);
	void GetAudio(EMUHANDLE handle, int source, short *buffer, unsigned int samples);

	// Debugging
	char Disassemble(EMUHANDLE handle, unsigned int pos, const char **raw, const char **instr);
	void AddBreakpoint(EMUHANDLE handle, unsigned int pos);
	void RemoveBreakpoint(EMUHANDLE handle, unsigned int pos);
	int IsBreakpoint(EMUHANDLE handle, unsigned int pos);
	char GetMemoryData(EMUHANDLE handle, int memory, unsigned int address);
private:
	unsigned int GetXMLVal(EMUHANDLE handle, pugi::xml_node &node, const char *element) const;

	void *_hDLL;
	bool _valid;
	pugi::xml_document _description;
	pugi::xml_node _root;



	// DLL functions
	void(__stdcall *_initPlugin)();

	// Initialization functions
	EMUHANDLE( __stdcall *_createEmulator)();
	void(__stdcall *_releaseEmulator)(EMUHANDLE);
	int32_t(__stdcall *_init)(EMUHANDLE, callBackfunctions_t);
	int32_t(__stdcall *_load)(EMUHANDLE, const SaveData_t *);
	int32_t(__stdcall *_loadState)(EMUHANDLE, const SaveData_t *);
	uint32_t(__stdcall *_initGL)(EMUHANDLE, int32_t);
	void(__stdcall *_destroyGL)(EMUHANDLE, int32_t);


	// Misc functions
	int32_t(__stdcall *_getValI)(EMUHANDLE, int32_t);
	uint32_t(__stdcall *_getValU)(EMUHANDLE, int32_t);
	const uint8_t *(__stdcall *_getString)(EMUHANDLE, int32_t);
	float (__stdcall *_getFloat)(EMUHANDLE handle, int id);
	void(__stdcall *_setValI)(EMUHANDLE, int32_t, int32_t);
	void(__stdcall *_setValU)(EMUHANDLE, int32_t, uint32_t);
	const uint8_t *(__stdcall *_getDescription)(uint32_t *);
	int32_t(__stdcall *_isCompatible)(const uint8_t *);
	int32_t(__stdcall *_getInterfaceVersion)();
	void(__stdcall *_run)(EMUHANDLE, int32_t);
	void(__stdcall *_step)(EMUHANDLE);
	int32_t(__stdcall *_isRunning)(EMUHANDLE);
	void(__stdcall *_releaseSaveData)(EMUHANDLE, SaveData_t *);

	// Running functions
	int32_t(__stdcall *_tick)(EMUHANDLE, uint32_t);
	void(__stdcall *_input)(EMUHANDLE, int32_t, int32_t);
	void(__stdcall *_draw)(EMUHANDLE, int32_t);
	void(__stdcall *_reshape)(EMUHANDLE, int32_t, int32_t, int32_t, int32_t);

	// stopping functions
	int32_t(__stdcall *_save)(EMUHANDLE, SaveData_t *);
	int32_t(__stdcall *_saveState)(EMUHANDLE, SaveData_t *);

	// Audio functions
	void(__stdcall *_initAudio)(EMUHANDLE, int32_t, uint32_t, int32_t);
	void(__stdcall *_getAudio)(EMUHANDLE, int32_t, int16_t *, uint32_t);

	// Debugging functions
	uint8_t(__stdcall *_disassemble)(EMUHANDLE, uint32_t, const uint8_t **, const uint8_t **);
	void(__stdcall *_addBreakpoint)(EMUHANDLE, uint32_t);
	void(__stdcall *_removeBreakpoint)(EMUHANDLE, uint32_t);
	int32_t(__stdcall *_isBreakpoint)(EMUHANDLE, uint32_t);
	uint8_t(__stdcall *_getMemoryData)(EMUHANDLE, int32_t, uint32_t);
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