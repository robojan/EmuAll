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
	videoOptions.filter = DEFAULT_VIDEOFILTER;
	videoOptions.messageFontSize = 16;
	audioOptions.sampleRate = DEFAULT_SAMPLERATE;
	audioOptions.bufferSize = DEFAULT_BUFFERSIZE;
	audioOptions.numBuffers = DEFAULT_NUMAUDIOBUFFERS;

}

Options Options::_instance;

Options::~Options()
{

}

Options & Options::GetSingleton()
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
	audioOptions.numBuffers = mFileConfig.ReadLong("Audio/NumBuffers", DEFAULT_NUMAUDIOBUFFERS);

	// Read Video
	videoOptions.keepAspect = mFileConfig.ReadBool("Video/KeepAspectRatio", DEFAULT_KEEPASPECT);
	videoOptions.filter = mFileConfig.ReadLong("Video/Filter", DEFAULT_VIDEOFILTER);

	wxString defaultMessageFont;
#ifdef _WIN32
	wxGetEnv("WINDIR", &defaultMessageFont);
	defaultMessageFont.append(wxFileName::GetPathSeparator());
	defaultMessageFont.append("Fonts");
	defaultMessageFont.append(wxFileName::GetPathSeparator());
	defaultMessageFont.append("georgia.ttf");
#elif defined(__APPLE__)
	defaultMessageFont = "/System/Library/Fonts/georgia.ttf";
#elif defined(__linux__) || defined(__unix__)
#endif
	videoOptions.messageFont = mFileConfig.Read("Video/MessageFont", defaultMessageFont);
	videoOptions.messageFontIdx = mFileConfig.ReadLong("Video/MessageFontIdx", 0);
	videoOptions.messageFontSize = mFileConfig.ReadLong("Video/MessageFontSize", 16);

	// Recent files
	recentFiles[0] = mFileConfig.Read("RecentFiles/file0", "");
	recentFiles[1] = mFileConfig.Read("RecentFiles/file1", "");
	recentFiles[2] = mFileConfig.Read("RecentFiles/file2", "");
	recentFiles[3] = mFileConfig.Read("RecentFiles/file3", "");
	recentFiles[4] = mFileConfig.Read("RecentFiles/file4", "");

	// Key bindings are loaded when the keys are added

	// DInput bindings
	dinputMap.clear();
	for (int i = 0; i < 255; i++) {
		wxString elementId = wxString::Format(_("DInput/device%d"), i);
		wxString guidString;
		if (mFileConfig.Read(elementId, &guidString)) {
			dinputMap[guidString.ToStdString()] = i;
		}
	}
}

void Options::SaveOptions()
{
	wxString appName = wxApp::GetInstance()->GetAppDisplayName();
	wxFileConfig mFileConfig(appName, wxEmptyString, GetConfigFileName(), wxEmptyString,
		wxCONFIG_USE_SUBDIR | wxCONFIG_USE_LOCAL_FILE);

	// Read Audio
	mFileConfig.Write("Audio/SampleRate", audioOptions.sampleRate);
	mFileConfig.Write("Audio/BufferSize", audioOptions.bufferSize);
	mFileConfig.Write("Audio/NumBuffers", audioOptions.numBuffers);

	// Read Video
	mFileConfig.Write("Video/KeepAspectRatio", videoOptions.keepAspect);
	mFileConfig.Write("Video/Filter", videoOptions.filter);
	mFileConfig.Write("Video/MessageFont", wxString(videoOptions.messageFont));
	mFileConfig.Write("Video/MessageFontIdx", videoOptions.messageFontIdx);
	mFileConfig.Write("Video/MessageFontSize", videoOptions.messageFontSize);

	// Recent files
	mFileConfig.Write("RecentFiles/file0", wxString(recentFiles[0]));
	mFileConfig.Write("RecentFiles/file1", wxString(recentFiles[1]));
	mFileConfig.Write("RecentFiles/file2", wxString(recentFiles[2]));
	mFileConfig.Write("RecentFiles/file3", wxString(recentFiles[3]));
	mFileConfig.Write("RecentFiles/file4", wxString(recentFiles[4]));

	// Key bindings
	for (auto &emu : _keyBindings) {
		wxString groupBase = wxString::Format(_("KeyBindings.%s"), emu.first);
		for (auto &keybinding : emu.second) {
			wxString group = groupBase;
			mFileConfig.Write(wxString::Format("%s/%d.primary", groupBase, keybinding.id), keybinding.primaryKey);
			mFileConfig.Write(wxString::Format("%s/%d.secondary", groupBase, keybinding.id), keybinding.secondaryKey);
		}
	}

	// DInput bindings
	for (auto &map : dinputMap) {
		wxString elementId = wxString::Format(_("DInput/device%d"), map.second);
		mFileConfig.Write(elementId, wxString(map.first));
	}

	// Create directory
	wxFileName configFileName(GetConfigFileName());
	configFileName.Mkdir(wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
	mFileConfig.Flush();
}


void Options::LoadKeyBindings(const std::string &name, const std::vector<EmulatorInput_t> &bindings)
{
	wxString appName = wxApp::GetInstance()->GetAppDisplayName();
	wxFileConfig mFileConfig(appName, wxEmptyString, GetConfigFileName(), wxEmptyString,
		wxCONFIG_USE_SUBDIR | wxCONFIG_USE_LOCAL_FILE);

	_keyBindings[name].clear();
	wxString groupBase = "KeyBindings.";
	groupBase.Append(name);
	for (auto &binding : bindings) {
		EmulatorInput_t input = binding;
		input.primaryKey = mFileConfig.ReadLong(wxString::Format("%s/%d.primary", groupBase, binding.id), binding.primaryKey);
		input.secondaryKey = mFileConfig.ReadLong(wxString::Format("%s/%d.secondary", groupBase, binding.id), binding.secondaryKey);
		_keyBindings[name].push_back(input);
	}
}

std::string Options::GetConfigFileName() const
{
	wxString appName = wxApp::GetInstance()->GetAppDisplayName();
	return wxFileConfig::GetLocalFileName(appName, wxCONFIG_USE_SUBDIR).ToStdString();
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

void Options::RebindKey(std::string name, int id, int keycode, int idx)
{
	auto &bindings = _keyBindings.find(name);
	if (bindings == _keyBindings.end()) {
		return;
	}

	for (auto &binding : bindings->second) {
		if (binding.id == id) {
			switch (idx) {
			case 0:
				binding.primaryKey = keycode;
				if (binding.secondaryKey == keycode) {
					binding.secondaryKey = -1;
				}
				break;
			case 1:
				binding.secondaryKey = keycode;
				if (binding.primaryKey == keycode) {
					binding.primaryKey = -1;
				}
				break;
			}
		}
		else {
			if (binding.primaryKey == keycode) {
				binding.primaryKey = -1;
			}
			if (binding.secondaryKey == keycode) {
				binding.secondaryKey = -1;
			}
		}
	}
}