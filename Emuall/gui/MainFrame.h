#ifndef _MAINFRAME_H
#define _MAINFRAME_H

#include <wx/wx.h>
#include <stdint.h>
#include <string>

#include "EmulatorScreen.h"
#include "../audio.h"
#include "../Emulator/Emulator.h"
#include "../util/Options.h"
#include "../util/logWx.h"
#include "../keyhandler.h"
#include "InputOptionsFrame.h"
#include "cpuDebugger.h"
#include "memDebugger.h"
#include "gpuDebugger.h"

class MainFrame: public wxFrame, GLPaneI
{
	DECLARE_EVENT_TABLE()
public:
	MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	~MainFrame();
	void Update();

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
	Audio		*_audio;
	InputMaster _inputHandler;

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
};


#endif