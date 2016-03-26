#include "memDbg.h"

#include <string>
#include <cassert>
#include <wx/fileconf.h>
#include <wx/app.h>
#include "Options.h"
#include "Log.h"

Options::Options()
{
	videoOptions.keepAspect = DEFAULT_KEEPASPECT;
	audioOptions.sampleRate = DEFAULT_SAMPLERATE;
	audioOptions.bufferSize = DEFAULT_BUFFERSIZE;

}

Options Options::_instance;

Options::~Options()
{

}

Options & Options::GetInstance()
{
	return _instance;
}

void Options::LoadOptions()
{
	wxString appName = wxApp::GetInstance()->GetAppDisplayName();
	wxFileConfig mFileConfig(appName, wxEmptyString, GetConfigFileName(), wxEmptyString,
		wxCONFIG_USE_SUBDIR | wxCONFIG_USE_LOCAL_FILE);


	Log(Message, "Config file: '%s'", GetConfigFileName().c_str());

	// Read Audio
	audioOptions.sampleRate = mFileConfig.ReadLong("Audio/SampleRate", DEFAULT_SAMPLERATE);
	audioOptions.bufferSize = mFileConfig.ReadLong("Audio/BufferSize", DEFAULT_BUFFERSIZE);

	// Read Video
	videoOptions.keepAspect = mFileConfig.ReadBool("Video/KeepAspectRatio", DEFAULT_KEEPASPECT);

	// Recent files
	recentFiles[0] = mFileConfig.Read("RecentFiles/file0", "");
	recentFiles[1] = mFileConfig.Read("RecentFiles/file1", "");
	recentFiles[2] = mFileConfig.Read("RecentFiles/file2", "");
	recentFiles[3] = mFileConfig.Read("RecentFiles/file3", "");
	recentFiles[4] = mFileConfig.Read("RecentFiles/file4", "");

	// Key bindings are loaded when the keys are added
}

void Options::SaveOptions()
{
	wxString appName = wxApp::GetInstance()->GetAppDisplayName();
	wxFileConfig mFileConfig(appName, wxEmptyString, GetConfigFileName(), wxEmptyString,
		wxCONFIG_USE_SUBDIR | wxCONFIG_USE_LOCAL_FILE);

	// Read Audio
	mFileConfig.Write("Audio/SampleRate", audioOptions.sampleRate);
	mFileConfig.Write("Audio/BufferSize", audioOptions.bufferSize);

	// Read Video
	mFileConfig.Write("Video/KeepAspectRatio", videoOptions.keepAspect);

	// Recent files
	mFileConfig.Write("RecentFiles/file0", wxString(recentFiles[0]));
	mFileConfig.Write("RecentFiles/file1", wxString(recentFiles[1]));
	mFileConfig.Write("RecentFiles/file2", wxString(recentFiles[2]));
	mFileConfig.Write("RecentFiles/file3", wxString(recentFiles[3]));
	mFileConfig.Write("RecentFiles/file4", wxString(recentFiles[4]));

	// Key bindings
	for (auto &emu : _keyBindings) {
		wxString groupBase = wxString::Format(_("KeyBindings.%s"), emu.first);
		for (auto &keybindings : emu.second) {
			wxString group = groupBase;
			group.Append("/");
			group.Append(keybindings.second.name);
			mFileConfig.Write(group, keybindings.first);
		}
	}


	// Create directory
	wxFileName configFileName(GetConfigFileName());
	configFileName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	mFileConfig.Flush();
}

std::string Options::GetConfigFileName() const
{
	wxString appName = wxApp::GetInstance()->GetAppDisplayName();
	return wxFileConfig::GetLocalFileName(appName, wxCONFIG_USE_SUBDIR).ToStdString();
}

void Options::AddKeyBinding(std::string name, const EmulatorInput_t &keyInfo)
{
	wxString appName = wxApp::GetInstance()->GetAppDisplayName();
	wxFileConfig mFileConfig(appName, wxEmptyString, GetConfigFileName(), wxEmptyString,
		wxCONFIG_USE_SUBDIR | wxCONFIG_USE_LOCAL_FILE);
	wxString entry = wxString::Format("KeyBindings.%s/%s", name, keyInfo.name);
	int key = mFileConfig.ReadLong(entry, keyInfo.defaultKey);
	_keyBindings[name][key] = keyInfo;
}

void Options::RebindKey(std::string name, int emuKey, int inputKey)
{
	assert(_keyBindings.find(name) != _keyBindings.end());
	std::map<int, EmulatorInput_t>::iterator it;
	EmulatorInput_t reboundEmuInput;
	int oldKey = 0;
	// Search for the keybinding
	for (it = _keyBindings[name].begin(); it != _keyBindings[name].end(); ++it)
	{
		if (it->second.key == emuKey)
		{
			reboundEmuInput = it->second;
			oldKey = it->first;
			// Remove binding from the list
			_keyBindings[name].erase(it);
			break;
		}
	}

	if ((it = _keyBindings[name].find(inputKey)) != _keyBindings[name].end())
	{
		// Key already exists in rebindings
		EmulatorInput_t overWriteInput = it->second;
		it->second = reboundEmuInput;
		_keyBindings[name][oldKey] = overWriteInput;
	}
	else {
		_keyBindings[name][inputKey] = reboundEmuInput;
	}
}

void Options::SaveRecentFile(std::string file)
{
	int exists = 4;
	for (int i = 0; i < 4; i++) {
		if (recentFiles[i] == file) {
			exists = i;
			break;
		}
	}
	if (exists == 0) {
		return;
	}
	for (int i = exists; i > 0; --i) {
		recentFiles[i] = recentFiles[i - 1];
	}
	recentFiles[0] = file;	
}

int Options::GetKeyBinding(std::string name, int key, int defaultKey)
{
	if (_keyBindings.find(name) == _keyBindings.end() || _keyBindings[name].find(key) == _keyBindings[name].end())
	{
		return defaultKey;
	}
	return _keyBindings[name][key].key;
}

int Options::GetInputKeyBinding(std::string name, int key, int defaultKey)
{
	if (_keyBindings.find(name) == _keyBindings.end())
		return defaultKey;

	std::map<int, EmulatorInput_t>::iterator it;
	// Search for the keybinding
	for (it = _keyBindings[name].begin(); it != _keyBindings[name].end(); ++it)
	{
		if (it->second.key == key)
		{
			return it->first;
		}
	}
	return defaultKey;
}

const EmulatorInput_t *Options::GetKeyBindingInfo(std::string name, int key)
{
	if (_keyBindings.find(name) == _keyBindings.end() || _keyBindings[name].find(key) == _keyBindings[name].end())
	{
		return NULL;
	}
	return &_keyBindings[name][key];
}