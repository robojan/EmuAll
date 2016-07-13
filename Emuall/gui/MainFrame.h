#ifndef _MAINFRAME_H
#define _MAINFRAME_H

#include <wx/wx.h>
#include <stdint.h>
#include <string>

#include "EmulatorScreen.h"
#include "../Emulator/Emulator.h"
#include "../Emulator/Sound/AudioManager.h"
#include "../Emulator/Sound/AudioStream.h"
#include "../Emulator/Sound/AudioBuffer.h"
#include "../util/Options.h"
#include "../util/logWx.h"
#include "../input/inputMaster.h"
#include "InputOptionsFrame.h"
#include "cpuDebugger.h"
#include "memDebugger.h"
#include "gpuDebugger.h"



class MainFrame: public wxFrame, GLPaneI
{
	DECLARE_EVENT_TABLE()
public:
	class AudioCallbackData {
	public:
		AudioCallbackData(Emulator *emu, int id) : emulator(emu), id(id) { }
		Emulator *emulator;
		int id;
	};

	MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	~MainFrame();
	void Update();
	void DrawNow(int id);

private:

	// wxWidgets members
	wxMenuBar	*_bar;
	wxMenu		*_menFile;
	wxMenu		*_menOptions;
	wxMenu		*_menDebug;
	wxMenu		*_menDebugLevel;
	wxMenu		*_menSaveState;
	wxMenu		*_menLoadState;
	EmulatorScreen *_display;
	wxLogWindow *_logger;
	wxTimer		*_timer;
	LogWx		_logDst;
	InputOptionsFrame *_inputOptionsFrame;
	CPUDebugger *_cpuDebugger;
	MemDebugger *_memDebugger;
	GPUDebugger *_gpuDebugger;
	wxStopWatch _deltaTimeTimer;

	// other members
	Emulator	_emulator;
	EmulatorList *_emulators;
	std::string	_filePath;
	std::string _saveFilePath;
	std::string _saveStateFilePath;
	AudioManager _audio;
	InputMaster *_inputHandler;
	bool _screenAutoRefresh;

	std::vector<AudioCallbackData> _audioCBData;

	// private functions
	void CreateLayout();
	void CreateMenuBar();
	void LoadEmulator(std::string &fileName);
	void CloseEmulator();

	void UpdateSaveStateLabels();
	void UpdateRecentFiles();
	void RunEmulator(uint32_t deltaTime);

	void OnIdle(wxIdleEvent &evt);
	void OnClose(wxCloseEvent &evt);
	void OnQuit(wxCommandEvent &evt);
	void OnOpen(wxCommandEvent &evt);
	void OnOpenRecentFile(wxCommandEvent &evt);
	void OnReset(wxCommandEvent &evt);
	void OnRun(wxCommandEvent &evt);
	void OnSaveState(wxCommandEvent &evt);
	void OnLoadState(wxCommandEvent &evt);
	void OnLogWindow(wxCommandEvent &evt);
	void OnLogLevel(wxCommandEvent &evt);
	void OnOptions(wxCommandEvent &evt);
	void OnTimer(wxTimerEvent &evt);
	void OnResize(wxSizeEvent &evt);
	void OnDebugCPU(wxCommandEvent &evt);
	void OnDebugMEM(wxCommandEvent &evt);
	void OnDebugGPU(wxCommandEvent &evt);
	void OnOptionVideoFilter(wxCommandEvent &evt);
	void DrawGL(int user);
	bool InitGL(int user);
	void DestroyGL(int user);
	static void AudioStreamCB(AudioBuffer::Format format, int freq, int elements, void *data, void *user);
};


#endif