#include "../util/memDbg.h"

#include <string>
#include "Emulator.h"
#include "../util/os_call.h"
#include "../util/dir.h"
#include "../util/log.h"
#include <stdint.h>
#include "../gui/debugger/DebuggerManager.h"
#include "../gui/debugger/DebuggerRoot.h"


EmulatorInterface::EmulatorInterface(const std::string &dllName) :
	_hDLL(NULL), _valid(false), _getInterfaceVersion(NULL), _createEmulator(NULL), _releaseEmulator(NULL),
	_init(NULL), _load(NULL), _getDescription(NULL), _isCompatible(NULL), _initGL(NULL), _destroyGL(NULL),
	_run(NULL), _isRunning(NULL), _tick(NULL), _input(NULL), _initPlugin(NULL),
	_draw(NULL), _reshape(NULL), _save(NULL), _step(NULL), _saveState(NULL), _loadState(NULL),
	_disassemble(NULL), _addBreakpoint(NULL), _removeBreakpoint(NULL), _isBreakpoint(NULL),
	_getMemoryData(NULL), _getValI(NULL), _getValU(NULL), _getString(NULL), _getFloat(NULL), _initAudio(nullptr), _getAudio(nullptr)
{
	_hDLL = LoadSharedLibrary(dllName.c_str());
	if (_hDLL == NULL)
	{
		Log(Error, "Could not load emulator %s: %d", dllName.c_str(), GetLastError());
		return;
	}
	_getInterfaceVersion = (int32_t(__stdcall*)())GetStdcallFunc(_hDLL, "GetInterfaceVersion");
	_getDescription = (const uint8_t *(__stdcall*)(uint32_t *))GetStdcallFunc<uint32_t *>(_hDLL, "GetDescription");
	_initPlugin = (void(__stdcall*)())GetStdcallFunc(_hDLL, "InitPlugin");
	if (_initPlugin) {
		_initPlugin();
	}
	if (!_getInterfaceVersion)
	{
		// Not valid emulator
		Log(Error, "Could not load emulator %s: Could not get the interface version", dllName.c_str());
		return;
	}
	if (!GetDescription(NULL))
	{
		Log(Error, "Could not load emulator %s: Could not get the description", dllName.c_str());
		return;
	}
	unsigned int xmlSize;
	const char *xml = GetDescription(&xmlSize);
	pugi::xml_parse_result result = _description.load_buffer(xml, xmlSize);
	if (!result)
	{
		Log(Error, "Could not load emulator %s: Parsing error \"%s\" in description at position %d ", dllName.c_str(), result.description(), result.offset);
		return;
	}
	_root = _description.child("emulator");
	if (!_root)
	{
		Log(Error, "Could not load emulator %s: description does not contain an 'emulator' root", dllName.c_str());
		return;
	}

	int version = _getInterfaceVersion();
	if (version >= 100)
	{
		_getValI				= (int32_t(__stdcall*)(EMUHANDLE, int32_t))					GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "GetValI");
		_getValU				= (uint32_t(__stdcall*)(EMUHANDLE, int32_t))				GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "GetValU");
		_getString				= (const uint8_t*(__stdcall*)(EMUHANDLE, int32_t))			GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "GetString");
		_getFloat				= (float (__stdcall*)(EMUHANDLE, int32_t))					GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "GetFloat");
		_setValI				= (void(__stdcall *)(EMUHANDLE, int32_t, int32_t))			GetStdcallFunc<EMUHANDLE, int32_t, int32_t>		(_hDLL, "SetValI");
		_setValU				= (void(__stdcall *)(EMUHANDLE, int32_t, uint32_t))			GetStdcallFunc<EMUHANDLE, int32_t, uint32_t>	(_hDLL, "SetValU");
		_createEmulator			= (EMUHANDLE(__stdcall*)())									GetStdcallFunc									(_hDLL, "CreateEmulator");
		_releaseEmulator		= (void(__stdcall*)(EMUHANDLE))								GetStdcallFunc<EMUHANDLE>						(_hDLL, "ReleaseEmulator");
		_init					= (int32_t(__stdcall*)(EMUHANDLE, callBackfunctions_t))		GetStdcallFunc<EMUHANDLE, callBackfunctions_t>	(_hDLL, "Init");
		_load					= (int32_t(__stdcall*)(EMUHANDLE, const SaveData_t *))		GetStdcallFunc<EMUHANDLE, const SaveData_t *>	(_hDLL, "Load");
		_loadState				= (int32_t(__stdcall*)(EMUHANDLE, const SaveData_t *))		GetStdcallFunc<EMUHANDLE, const SaveData_t *>	(_hDLL, "LoadState");
		_initGL					= (uint32_t(__stdcall*)(EMUHANDLE, int32_t))				GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "InitGL");
		_destroyGL				= (void(__stdcall*)(EMUHANDLE, int32_t))		 			GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "DestroyGL");
		_isCompatible			= (int32_t(__stdcall*)(const uint8_t *))					GetStdcallFunc<const uint8_t *>					(_hDLL, "IsCompatible");
		_releaseSaveData		= (void(__stdcall*)(EMUHANDLE, SaveData_t *))				GetStdcallFunc<EMUHANDLE, SaveData_t *>			(_hDLL, "ReleaseSaveData");
		_run					= (void(__stdcall*)(EMUHANDLE, int32_t))					GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "Run");
		_step					= (void(__stdcall*)(EMUHANDLE))								GetStdcallFunc<EMUHANDLE>						(_hDLL, "Step");
		_isRunning				= (int32_t(__stdcall*)(EMUHANDLE))							GetStdcallFunc<EMUHANDLE>						(_hDLL, "IsRunning");
		_tick					= (int32_t(__stdcall*)(EMUHANDLE, uint32_t))				GetStdcallFunc<EMUHANDLE, uint32_t>				(_hDLL, "Tick");
		_input					= (void(__stdcall*)(EMUHANDLE, int32_t, int32_t))			GetStdcallFunc<EMUHANDLE, int32_t, int32_t>		(_hDLL, "Input");
		_draw					= (void(__stdcall*)(EMUHANDLE, int32_t))					GetStdcallFunc<EMUHANDLE, int32_t>				(_hDLL, "Draw");
		_reshape				= (void(__stdcall*)(EMUHANDLE, int32_t, int32_t, int32_t, int32_t))	GetStdcallFunc<EMUHANDLE, int32_t, int32_t, int32_t, int32_t>(_hDLL, "Reshape");
		_save					= (int32_t(__stdcall*)(EMUHANDLE, SaveData_t *))			GetStdcallFunc<EMUHANDLE, SaveData_t *>			(_hDLL, "Save");
		_saveState				= (int32_t(__stdcall*)(EMUHANDLE, SaveData_t *))			GetStdcallFunc<EMUHANDLE, SaveData_t *>			(_hDLL, "SaveState");
		_disassemble			= (uint8_t(__stdcall*)(EMUHANDLE, uint32_t, const uint8_t **, const uint8_t **))GetStdcallFunc<EMUHANDLE, uint32_t, const uint8_t **, const uint8_t **>(_hDLL, "Disassemble");
		_addBreakpoint			= (void(__stdcall*)(EMUHANDLE, uint32_t))					GetStdcallFunc<EMUHANDLE, uint32_t>				(_hDLL, "AddBreakpoint");
		_removeBreakpoint		= (void(__stdcall*)(EMUHANDLE, uint32_t))					GetStdcallFunc<EMUHANDLE, uint32_t>				(_hDLL, "RemoveBreakpoint");
		_isBreakpoint			= (int32_t(__stdcall*)(EMUHANDLE, uint32_t))				GetStdcallFunc<EMUHANDLE, uint32_t>				(_hDLL, "IsBreakpoint");
		_getMemoryData			= (uint8_t(__stdcall*)(EMUHANDLE, int32_t, uint32_t))		GetStdcallFunc<EMUHANDLE, int32_t, uint32_t>	(_hDLL, "GetMemoryData");
		_initAudio				= (void (__stdcall *)(EMUHANDLE, int32_t, uint32_t, int32_t)) GetStdcallFunc<EMUHANDLE, int32_t, uint32_t, int32_t> (_hDLL, "InitAudio");
		_getAudio				= (void (__stdcall *)(EMUHANDLE, int32_t, int16_t *, uint32_t)) GetStdcallFunc<EMUHANDLE, int32_t, int16_t *, uint32_t> (_hDLL, "GetAudio");
	}
	_valid = true;
	Log(Debug, "Loaded functions from emulator");
}

EmulatorInterface::~EmulatorInterface()
{
	if (_hDLL)
	{
		FreeSharedLibrary(_hDLL);
	}

}

EmulatorInfo_t EmulatorInterface::GetInfo() const
{
	EmulatorInfo_t info;
	info.name = _root.child_value("name");
	info.description = _root.child_value("description");
	info.fileFilterString = _root.child_value("fileFilter");
	info.aboutInfo = _root.child_value("about");
	info.screenAutoRefresh = _root.child("automaticScreenRefresh").text().as_bool(true);
	for (pugi::xml_node screen = _root.child("screen"); screen; screen = screen.next_sibling("screen")) {
		EmulatorScreen_t screenInfo;
		screenInfo.id = screen.attribute("id").as_int(-1);		
		screenInfo.width = screen.child("width").text().as_int(-1);
		screenInfo.height = screen.child("height").text().as_int(-1);
		screenInfo.mouseXvarID = screen.child("mousex").text().as_int(-1);
		screenInfo.mouseYvarID = screen.child("mousey").text().as_int(-1);
		info.screens.push_back(screenInfo);
	}
	info.numAudioStreams = _root.child("audio").child("sources").text().as_int(0);
	return info;
}

std::string EmulatorInterface::GetFileFilterEntry() const
{
	std::string entry = GetName();
	entry.append("(");
	const char *filter = _root.child_value("fileFilter");
	if (*filter == '\0')
	{
		static const char *allFiles = "*.*";
		Log(Warn, "Emulator \"%s\" has no filters", GetName().c_str());
		filter = allFiles;
	}
	entry.append(filter);
	entry.append(")|");
	entry.append(filter);
	return entry;
}

std::string EmulatorInterface::GetName() const
{
	std::string name = _root.child_value("name");
	if (name.empty())
	{
		name = "Error!";
	}
	return name;
}

std::vector<EmulatorInput_t> EmulatorInterface::GetEmulatorInputs() const
{
	std::vector<EmulatorInput_t> ret;
	pugi::xml_node inputs = _root.child("input");
	if (!inputs)
	{
		Log(Warn, "Emulator \"%s\" has no input", GetName().c_str());
		return ret;
	}
	for (pugi::xml_node_iterator it = inputs.begin(); it != inputs.end(); ++it)
	{
		if (strcmp(it->name(), "key") == 0)
		{
			EmulatorInput_t input;
			input.id = it->attribute("id").as_int(-1);
			input.primaryKey = it->child("defaultKey").text().as_int(-1);
			input.secondaryKey = it->child("defaultSecondaryKey").text().as_int(-1);
			input.name = it->child_value("name");
			input.flags = 0;
			if (input.id >= 0) {
				ret.push_back(input);
			}
		}
	}
	return ret;
}


CpuDebuggerInfo_t EmulatorInterface::GetCpuDebuggerInfo() const
{
	CpuDebuggerInfo_t ret;
	ret.breakpointSupport = false;
	ret.disassembler = false;
	ret.disassemblerSize = 0;
	ret.step = false;
	ret.stepOut = false;
	ret.stepOver = false;
	pugi::xml_node cpu = _root.child("debugging").child("cpu");
	if (!cpu)
	{
		Log(Warn, "Emulator \"%s\" has no \"debugging/cpu\" section", GetName().c_str());
		return ret;
	}
	if (strcmp(cpu.child("disassembler").child_value("enabled"), "true") == 0)
	{
		ret.disassembler = true;
		ret.breakpointSupport = strcmp(cpu.child("disassembler").child_value("breakpoint"), "true") == 0;
		ret.disassemblerSize = strtol(cpu.child("disassembler").child_value("size"), NULL, 0);
		ret.curLineId = strtol(cpu.child("disassembler").child_value("curLineId"), NULL, 0);
	}
	ret.step = strcmp(cpu.child_value("step"), "true") == 0;
	ret.stepOver = strcmp(cpu.child_value("stepOver"), "true") == 0;
	ret.stepOut = strcmp(cpu.child_value("stepOut"), "true") == 0;
	pugi::xml_node node = cpu.child("registers");
	if (node)
	{
		for (pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
		{
			if (strcmp(it->name(), "register") == 0)
			{
				EmulatorRegister_t reg;
				reg.id = it->attribute("id").as_int();
				reg.name = it->child_value("name");
				reg.size = strtol(it->child_value("size"), NULL, 0);
				reg.readonly = it->child("readOnly") ? true : false; // stupid ms warnings
				ret.registers.push_back(reg);
			}
			else if (strcmp(it->name(), "flag") == 0)
			{
				EmulatorRegister_t reg;
				reg.id = it->attribute("id").as_int();
				reg.name = it->child_value("name");
				reg.size = 1;
				reg.readonly = it->child("readOnly") ? true : false; // stupid ms warnings
				ret.flags.push_back(reg);
			}
		}
	}
	return ret;
}

MemDebuggerInfo_t EmulatorInterface::GetMemDebuggerInfo(EMUHANDLE handle) const
{
	MemDebuggerInfo_t ret; 
	pugi::xml_node mem = _root.child("debugging").child("mem");
	if (!mem)
	{
		Log(Warn, "Emulator \"%s\" has no \"debugging/mem\" section", GetName().c_str());
		return ret;
	}
	else
	{
		for (pugi::xml_node_iterator it = mem.begin(); it != mem.end(); ++it)
		{
			if (strcmp(it->name(), "memory") == 0)
			{
				EmulatorMemory_t mem;
				mem.id = it->attribute("id").as_int();
				mem.name = it->child_value("name");
				mem.size = GetXMLVal(handle, *it, "size");
				ret.memories.push_back(mem);
			}
			else if (strcmp(it->name(), "info") == 0)
			{
				EmulatorValue_t val;
				val.id = it->attribute("id").as_int();
				val.name = it->child("name").text().as_string("Error!");
				val.size = it->child("size").text().as_int();
				const char *mode = it->child("mode").text().as_string("dec");
				if (strcmp(mode, "dec") == 0)
				{
					val.mode = EmulatorValue_t::dec;
				}
				else if (strcmp(mode, "hex") == 0)
				{
					val.mode = EmulatorValue_t::hex;
				}
				else if (strcmp(mode, "oct") == 0)
				{
					val.mode = EmulatorValue_t::oct;
				}
				else if (strcmp(mode, "string") == 0)
				{
					val.mode = EmulatorValue_t::string;
				}
				else {
					Log(Warn, "Unknown info mode \"%s\" found", mode);
					val.mode = EmulatorValue_t::dec;
				}
				ret.infos.push_back(val);
			}
			else if (strcmp(it->name(), "readOnly") == 0)
			{
				ret.readOnly = strcmp(it->text().as_string(), "true") == 0;
			}
		}
	}
	return ret;
}

Debugger::DebuggerRoot *EmulatorInterface::GetGpuDebuggerInfo(Emulator *emu) const
{
	pugi::xml_node gpu = _root.child("debugging").child("gpu");
	if (!gpu)
	{
		Log(Warn, "Emulator \"%s\" has no \"debugging/gpu\" section", GetName().c_str());
		return NULL;
	}
	else
	{
		return new Debugger::DebuggerRoot(emu, gpu);
	}
}

uint32_t EmulatorInterface::GetValU(EMUHANDLE handle, int id) const
{
	if (_getValU == NULL)
	{
		return 0;
	}
	return _getValU(handle, id);
}

int32_t EmulatorInterface::GetValI(EMUHANDLE handle, int id) const
{
	if (_getValI == NULL)
	{
		return 0;
	}
	return _getValI(handle, id);
}

const char *EmulatorInterface::GetString(EMUHANDLE handle, int id) const
{
	if (_getString == NULL)
	{
		return NULL;
	}
	return (const char *)_getString(handle, id);
}

void EmulatorInterface::SetValI(EMUHANDLE handle, int id, int32_t val) const
{
	if (_setValI == NULL)
	{
		return;
	}
	return _setValI(handle, id, val);
}

void EmulatorInterface::SetValU(EMUHANDLE handle, int id, uint32_t val) const
{
	if (_setValU == NULL)
	{
		return;
	}
	return _setValU(handle, id, val);
}

EMUHANDLE EmulatorInterface::CreateEmulator()
{
	if (_createEmulator == NULL)
		return NULL;
	return _createEmulator();
}

void EmulatorInterface::ReleaseEmulator(EMUHANDLE handle)
{
	if (_releaseEmulator == NULL)
		return;
	_releaseEmulator(handle);
}

int EmulatorInterface::Init(EMUHANDLE handle, callBackfunctions_t funcs)
{
	if (_init == NULL)
		return false;
	return _init(handle, funcs);
}

int EmulatorInterface::Load(EMUHANDLE handle, const SaveData_t *data)
{
	if (_load == NULL)
		return false;
	return _load(handle, data);
}

int EmulatorInterface::LoadState(EMUHANDLE handle, const SaveData_t *data)
{
	if (_loadState == NULL)
		return false;
	return _loadState(handle, data);
}

bool EmulatorInterface::InitGL(EMUHANDLE handle, int id)
{
	if (_initGL == NULL)
		return false;
	Log(Debug, "InitGL %d", id);
	return _initGL(handle, id) != 0;
}

void EmulatorInterface::DestroyGL(EMUHANDLE handle, int id)
{
	if (_destroyGL == NULL)
		return;
	Log(Debug, "DestoyGL %d", id);
	_destroyGL(handle, id);
}

void EmulatorInterface::ReleaseSaveData(EMUHANDLE handle, SaveData_t *data)
{
	if (_releaseSaveData == NULL)
		return;
	_releaseSaveData(handle, data);
}

const char *EmulatorInterface::GetDescription(unsigned int *size)
{
	if (_getDescription == NULL)
		return NULL;
	return (const char *) _getDescription(size);
}

int EmulatorInterface::IsCompatible(const char *filename)
{
	if (_isCompatible == NULL)
		return false;
	return _isCompatible((const uint8_t *) filename);
}

void EmulatorInterface::Run(EMUHANDLE handle, int run)
{
	if (_run == NULL)
		return;
	_run(handle, run);
}

void EmulatorInterface::Step(EMUHANDLE handle)
{
	if (_step == NULL)
		return;
	_step(handle);
}

int EmulatorInterface::IsRunning(EMUHANDLE handle)
{
	if (_isRunning == NULL)
		return -1;
	return _isRunning(handle);
}

int EmulatorInterface::Tick(EMUHANDLE handle, uint32_t time)
{
	if (_tick == NULL)
		return false;
	return _tick(handle, time);
}

void EmulatorInterface::Input(EMUHANDLE handle, int key, int pressed)
{
	if (_input == NULL)
		return;
	return _input(handle, key, pressed);
}

void EmulatorInterface::Draw(EMUHANDLE handle, int id)
{
	if (_draw == NULL)
		return;
	_draw(handle, id);
}

void EmulatorInterface::Reshape(EMUHANDLE handle,int id, int width, int height, bool keepAspect)
{
	if (_reshape == NULL)
		return;
	_reshape(handle, id, width, height, keepAspect);
}

int EmulatorInterface::Save(EMUHANDLE handle, SaveData_t *data)
{
	if (_save == NULL)
		return false;
	return _save(handle, data);
}

int EmulatorInterface::SaveState(EMUHANDLE handle, SaveData_t *data)
{
	if (_saveState == NULL)
		return false;
	return _saveState(handle, data);
}

void EmulatorInterface::InitAudio(EMUHANDLE handle, int source, unsigned int sampleRate, int channels)
{
	if (_initAudio == nullptr) {
		return;
	}
	_initAudio(handle, source, sampleRate, channels);
}

void EmulatorInterface::GetAudio(EMUHANDLE handle, int source, short *buffer, unsigned int samples)
{
	if (_getAudio == nullptr) {
		return;
	}
	_getAudio(handle, source, buffer, samples);
}

char EmulatorInterface::Disassemble(EMUHANDLE handle, unsigned int pos, const char **raw, const char **instr)
{
	if (_disassemble == NULL)
		return -1;
	return _disassemble(handle, pos, (const uint8_t **) raw, (const uint8_t **) instr);
}

void EmulatorInterface::AddBreakpoint(EMUHANDLE handle, unsigned int pos)
{
	if (_addBreakpoint == NULL)
		return;
	return _addBreakpoint(handle, pos);
}

void EmulatorInterface::RemoveBreakpoint(EMUHANDLE handle, unsigned int pos)
{
	if (_removeBreakpoint == NULL)
		return;
	return _removeBreakpoint(handle, pos);
}

int EmulatorInterface::IsBreakpoint(EMUHANDLE handle, unsigned int pos)
{
	if (_isBreakpoint == NULL)
		return 1 == 0;
	return _isBreakpoint(handle, pos);
}

char EmulatorInterface::GetMemoryData(EMUHANDLE handle, int memory, unsigned int address)
{
	if (_getMemoryData == NULL)
		return 0;
	return _getMemoryData(handle, memory, address);
}

unsigned int EmulatorInterface::GetXMLVal(EMUHANDLE handle, pugi::xml_node &node, const char *element) const 
{
	if (!node.child(element))
	{
		Log(Warn, "node %s doesn't contain child %s", node.name(), element);
		return 0;
	}
	if (!node.child(element).attribute("var").as_bool(false))
	{
		return strtol(node.child_value(element), NULL, 0);
	}
	else {
		return GetValU(handle, strtol(node.child_value(element), NULL, 0));
	}
}

float EmulatorInterface::GetFloat(EMUHANDLE handle, int id) const
{
	if (_getFloat == NULL)
	{
		return NAN;
	}
	return _getFloat(handle, id);
}

EmulatorList::EmulatorList(const char *folder)
{
	// Load all emulators
	std::vector<std::string> files = GetDirFiles(folder,"emu.dll");
	std::vector<std::string>::iterator it;
	for (it = files.begin(); it != files.end(); ++it)
	{
		EmulatorInterface *emu = new EmulatorInterface(it->substr(0, it->rfind('.')));
		if (emu->IsValid())
		{
			_emulators.push_back(emu);
		}
		else {
			delete emu;
		}
	}
}

EmulatorList::EmulatorList()
{
	// Do nothing
}

EmulatorList::~EmulatorList()
{
	std::list<EmulatorInterface *>::iterator it;
	for (it = _emulators.begin(); it != _emulators.end(); ++it)
	{
		delete *it;
	}
	_emulators.clear();
}

std::list<EmulatorInterface *>::iterator EmulatorList::begin()
{
	return _emulators.begin();
}

std::list<EmulatorInterface *>::iterator EmulatorList::end()
{
	return _emulators.end();
}

EmulatorInterface *EmulatorList::GetCompatibleEmulator(const char *fileName) const
{
	std::list<EmulatorInterface *>::const_iterator it;
	for (it = _emulators.cbegin(); it != _emulators.cend(); ++it)
	{
		if ((*it)->IsCompatible(fileName))
		{
			return *it;
		}
	}
	return NULL;
}

std::string EmulatorList::GetFileFilters() const
{
	std::list<EmulatorInterface *>::const_iterator it = _emulators.cbegin();
	std::string filter;
	while (it != _emulators.cend())
	{
		filter.append((*it)->GetFileFilterEntry());
		++it;
		if (it != _emulators.cend())
		{
			filter.append("|");
		}
	}
	return filter;
}