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
	mhDLL(NULL), mValid(false), mGetInterfaceVersion(NULL), mCreateEmulator(NULL), mReleaseEmulator(NULL),
	mInit(NULL), mLoad(NULL), mGetDescription(NULL), mIsCompatible(NULL), mInitGL(NULL), mDestroyGL(NULL),
	mRun(NULL), mIsRunning(NULL), mTick(NULL), mInput(NULL), 
	mDraw(NULL), mReshape(NULL), mSave(NULL), mStep(NULL),
	mDisassemble(NULL), mAddBreakpoint(NULL), mRemoveBreakpoint(NULL), mIsBreakpoint(NULL),
	mGetMemoryData(NULL), mGetValI(NULL), mGetValU(NULL), mGetString(NULL)
{
	mhDLL = LoadSharedLibrary(dllName.c_str());
	if (mhDLL == NULL)
	{
		Log(Error, "Could not load emulator %s: %d", dllName.c_str(), GetLastError());
		return;
	}
	mGetInterfaceVersion = (int32_t(__stdcall*)())GetStdcallFunc(mhDLL, "GetInterfaceVersion");
	mGetDescription = (const uint8_t *(__stdcall*)(uint32_t *))GetStdcallFunc<uint32_t *>(mhDLL, "GetDescription");
	if (!mGetInterfaceVersion)
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
	pugi::xml_parse_result result = mDescription.load_buffer(xml, xmlSize);
	if (!result)
	{
		Log(Error, "Could not load emulator %s: Parsing error \"%s\" in description at position %d ", dllName.c_str(), result.description(), result.offset);
		return;
	}
	mRoot = mDescription.child("emulator");
	if (!mRoot)
	{
		Log(Error, "Could not load emulator %s: description does not contain an 'emulator' root", dllName.c_str());
		return;
	}

	int version = mGetInterfaceVersion();
	if (version >= 100)
	{
		mGetValI				= (int32_t(__stdcall*)(EMUHANDLE, int32_t))					GetStdcallFunc<EMUHANDLE, int32_t>				(mhDLL, "GetValI");
		mGetValU				= (uint32_t(__stdcall*)(EMUHANDLE, int32_t))				GetStdcallFunc<EMUHANDLE, int32_t>				(mhDLL, "GetValU");
		mGetString				= (const uint8_t*(__stdcall*)(EMUHANDLE, int32_t))			GetStdcallFunc<EMUHANDLE, int32_t>				(mhDLL, "GetString");
		mSetValI				= (void(__stdcall *)(EMUHANDLE, int32_t, int32_t))			GetStdcallFunc<EMUHANDLE, int32_t, int32_t>		(mhDLL, "SetValI");
		mSetValU				= (void(__stdcall *)(EMUHANDLE, int32_t, uint32_t))			GetStdcallFunc<EMUHANDLE, int32_t, uint32_t>	(mhDLL, "SetValU");
		mCreateEmulator			= (EMUHANDLE(__stdcall*)())									GetStdcallFunc									(mhDLL, "CreateEmulator");
		mReleaseEmulator		= (void(__stdcall*)(EMUHANDLE))								GetStdcallFunc<EMUHANDLE>						(mhDLL, "ReleaseEmulator");
		mInit					= (int32_t(__stdcall*)(EMUHANDLE, callBackfunctions_t))		GetStdcallFunc<EMUHANDLE, callBackfunctions_t>	(mhDLL, "Init");
		mLoad					= (int32_t(__stdcall*)(EMUHANDLE, const uint8_t *))			GetStdcallFunc<EMUHANDLE, const uint8_t *>		(mhDLL, "Load");
		mInitGL					= (uint32_t(__stdcall*)(EMUHANDLE, int32_t))				GetStdcallFunc<EMUHANDLE, int32_t>				(mhDLL, "InitGL");
		mDestroyGL				= (void(__stdcall*)(EMUHANDLE, int32_t))		 			GetStdcallFunc<EMUHANDLE, int32_t>				(mhDLL, "DestroyGL");
		mIsCompatible			= (int32_t(__stdcall*)(const uint8_t *))					GetStdcallFunc<const uint8_t *>					(mhDLL, "IsCompatible");
		mRun					= (void(__stdcall*)(EMUHANDLE, int32_t))					GetStdcallFunc<EMUHANDLE, int32_t>				(mhDLL, "Run");
		mStep					= (void(__stdcall*)(EMUHANDLE))								GetStdcallFunc<EMUHANDLE>						(mhDLL, "Step");
		mIsRunning				= (int32_t(__stdcall*)(EMUHANDLE))							GetStdcallFunc<EMUHANDLE>						(mhDLL, "IsRunning");
		mTick					= (int32_t(__stdcall*)(EMUHANDLE, uint32_t))				GetStdcallFunc<EMUHANDLE, uint32_t>				(mhDLL, "Tick");
		mInput					= (void(__stdcall*)(EMUHANDLE, int32_t, int32_t))			GetStdcallFunc<EMUHANDLE, int32_t, int32_t>		(mhDLL, "Input");
		mDraw					= (void(__stdcall*)(EMUHANDLE, int32_t))					GetStdcallFunc<EMUHANDLE, int32_t>				(mhDLL, "Draw");
		mReshape				= (void(__stdcall*)(EMUHANDLE, int32_t, int32_t, int32_t))	GetStdcallFunc<EMUHANDLE, int32_t, int32_t, int32_t>(mhDLL, "Reshape");
		mSave					= (int32_t(__stdcall*)(EMUHANDLE, const uint8_t *))			GetStdcallFunc<EMUHANDLE, const uint8_t *>		(mhDLL, "Save");
		mDisassemble			= (uint8_t(__stdcall*)(EMUHANDLE, uint32_t, const uint8_t **, const uint8_t **))GetStdcallFunc<EMUHANDLE, uint32_t, const uint8_t **, const uint8_t **>(mhDLL, "Disassemble");
		mAddBreakpoint			= (void(__stdcall*)(EMUHANDLE, uint32_t))					GetStdcallFunc<EMUHANDLE, uint32_t>				(mhDLL, "AddBreakpoint");
		mRemoveBreakpoint		= (void(__stdcall*)(EMUHANDLE, uint32_t))					GetStdcallFunc<EMUHANDLE, uint32_t>				(mhDLL, "RemoveBreakpoint");
		mIsBreakpoint			= (int32_t(__stdcall*)(EMUHANDLE, uint32_t))				GetStdcallFunc<EMUHANDLE, uint32_t>				(mhDLL, "IsBreakpoint");
		mGetMemoryData			= (uint8_t(__stdcall*)(EMUHANDLE, int32_t, uint32_t))		GetStdcallFunc<EMUHANDLE, int32_t, uint32_t>	(mhDLL, "GetMemoryData");
	}
	mValid = true;
	Log(Debug, "Loaded functions from emulator");
}

EmulatorInterface::~EmulatorInterface()
{
	if (mhDLL)
	{
		FreeSharedLibrary(mhDLL);
	}

}

std::string EmulatorInterface::GetFileFilterEntry() const
{
	std::string entry = GetName();
	entry.append("(");
	const char *filter = mRoot.child_value("fileFilter");
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
	std::string name = mRoot.child_value("name");
	if (name.empty())
	{
		name = "Error!";
	}
	return name;
}

std::list<EmulatorInput_t> EmulatorInterface::GetEmulatorInputs() const
{
	std::list<EmulatorInput_t> ret;
	pugi::xml_node inputs = mRoot.child("input");
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
			input.key = it->attribute("key").as_int(-1);
			input.defaultKey = strtol(it->child_value("defaultKey"), NULL, 0);
			input.name = it->child_value("name");
			input.flags = 0;
			ret.push_back(input);
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
	pugi::xml_node cpu = mRoot.child("debugging").child("cpu");
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
	pugi::xml_node mem = mRoot.child("debugging").child("mem");
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
	pugi::xml_node gpu = mRoot.child("debugging").child("gpu");
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

EmulatorSettings_t EmulatorInterface::GetEmulatorSettings() const
{
	EmulatorSettings_t ret;
	ret.audioSources = mRoot.child("audio").child("sources").text().as_int(0);
	return ret;
}

uint32_t EmulatorInterface::GetValU(EMUHANDLE handle, int id) const
{
	if (mGetValU == NULL)
	{
		return 0;
	}
	return mGetValU(handle, id);
}

int32_t EmulatorInterface::GetValI(EMUHANDLE handle, int id) const
{
	if (mGetValI == NULL)
	{
		return 0;
	}
	return mGetValI(handle, id);
}

const char *EmulatorInterface::GetString(EMUHANDLE handle, int id) const
{
	if (mGetString == NULL)
	{
		return NULL;
	}
	return (const char *)mGetString(handle, id);
}

void EmulatorInterface::SetValI(EMUHANDLE handle, int id, int32_t val) const
{
	if (mSetValI == NULL)
	{
		return;
	}
	return mSetValI(handle, id, val);
}

void EmulatorInterface::SetValU(EMUHANDLE handle, int id, uint32_t val) const
{
	if (mSetValU == NULL)
	{
		return;
	}
	return mSetValU(handle, id, val);
}

EMUHANDLE EmulatorInterface::CreateEmulator()
{
	if (mCreateEmulator == NULL)
		return NULL;
	return mCreateEmulator();
}

void EmulatorInterface::ReleaseEmulator(EMUHANDLE handle)
{
	if (mReleaseEmulator == NULL)
		return;
	mReleaseEmulator(handle);
}

int EmulatorInterface::Init(EMUHANDLE handle, callBackfunctions_t funcs)
{
	if (mInit == NULL)
		return false;
	return mInit(handle, funcs);
}
int EmulatorInterface::Load(EMUHANDLE handle, const char *filename)
{
	if (mLoad == NULL)
		return false;
	return mLoad(handle, (const uint8_t *)filename);
}

bool EmulatorInterface::InitGL(EMUHANDLE handle, int id)
{
	if (mInitGL == NULL)
		return false;
	return mInitGL(handle, id) != 0;
}

void EmulatorInterface::DestroyGL(EMUHANDLE handle, int id)
{
	if (mDestroyGL == NULL)
		return;
	mDestroyGL(handle, id);
}

const char *EmulatorInterface::GetDescription(unsigned int *size)
{
	if (mGetDescription == NULL)
		return NULL;
	return (const char *) mGetDescription(size);
}

int EmulatorInterface::IsCompatible(const char *filename)
{
	if (mIsCompatible == NULL)
		return false;
	return mIsCompatible((const uint8_t *) filename);
}

void EmulatorInterface::Run(EMUHANDLE handle, int run)
{
	if (mRun == NULL)
		return;
	mRun(handle, run);
}

void EmulatorInterface::Step(EMUHANDLE handle)
{
	if (mStep == NULL)
		return;
	mStep(handle);
}

int EmulatorInterface::IsRunning(EMUHANDLE handle)
{
	if (mIsRunning == NULL)
		return -1;
	return mIsRunning(handle);
}

int EmulatorInterface::Tick(EMUHANDLE handle, uint32_t time)
{
	if (mTick == NULL)
		return false;
	return mTick(handle, time);
}

void EmulatorInterface::Input(EMUHANDLE handle, int key, int pressed)
{
	if (mInput == NULL)
		return;
	return mInput(handle, key, pressed);
}

void EmulatorInterface::Draw(EMUHANDLE handle, int id)
{
	if (mDraw == NULL)
		return;
	mDraw(handle, id);
}

void EmulatorInterface::Reshape(EMUHANDLE handle, int width, int height, bool keepAspect)
{
	if (mReshape == NULL)
		return;
	mReshape(handle, width, height, keepAspect);
}

int EmulatorInterface::Save(EMUHANDLE handle, const char *filename)
{
	if (mSave == NULL)
		return false;
	return mSave(handle, (const uint8_t *) filename);
}

char EmulatorInterface::Disassemble(EMUHANDLE handle, unsigned int pos, const char **raw, const char **instr)
{
	if (mDisassemble == NULL)
		return -1;
	return mDisassemble(handle, pos, (const uint8_t **) raw, (const uint8_t **) instr);
}

void EmulatorInterface::AddBreakpoint(EMUHANDLE handle, unsigned int pos)
{
	if (mAddBreakpoint == NULL)
		return;
	return mAddBreakpoint(handle, pos);
}

void EmulatorInterface::RemoveBreakpoint(EMUHANDLE handle, unsigned int pos)
{
	if (mRemoveBreakpoint == NULL)
		return;
	return mRemoveBreakpoint(handle, pos);
}

int EmulatorInterface::IsBreakpoint(EMUHANDLE handle, unsigned int pos)
{
	if (mIsBreakpoint == NULL)
		return 1 == 0;
	return mIsBreakpoint(handle, pos);
}

char EmulatorInterface::GetMemoryData(EMUHANDLE handle, int memory, unsigned int address)
{
	if (mGetMemoryData == NULL)
		return 0;
	return mGetMemoryData(handle, memory, address);
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