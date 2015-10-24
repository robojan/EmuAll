#ifndef _MAINFRAME_H
#define _MAINFRAME_H

#include <wx/wx.h>
#include <stdint.h>
#include <string>

#include "GLPane.h"
#include "../audio.h"
#include "../Emulator/Emulator.h"
#include "../util/Options.h"
#include "../util/logWx.h"
#include "../keyhandler.h"
#include "InputOptionsFrame.h"
#include "cpuDebugger.h"
#include "memDebugger.h"
#include "gpuDebugger.h"

#define FPS		1000/30

class MainFrame: public wxFrame, GLPaneI
{
	DECLARE_EVENT_TABLE()
public:
	MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
	~MainFrame();
	void Update();

private:
	// wxWidgets members
	wxMenuBar	*mBar;
	wxMenu		*mMenFile;
	wxMenu		*mMenOptions;
	wxMenu		*mMenDebug;
	wxMenu		*mMenDebugLevel;
	GLPane		*mDisplay;
	wxLogWindow *mLogger;
	wxTimer		*mTimer;
	LogWx		mLogDst;
	InputOptionsFrame *mInputOptionsFrame;
	CPUDebugger *mCpuDebugger;
	MemDebugger *mMemDebugger;
	GPUDebugger *mGpuDebugger;

	// other members
	Emulator	mEmulator;
	EmulatorList *mEmulators;
	std::string	mFilePath;
	Audio		*mAudio;
	Options		mOptions;
	InputMaster mInputHandler;

	// private functions
	void CreateLayout();
	void CreateMenuBar();
	void LoadEmulator(std::string &fileName);
	void CloseEmulator();

	void OnClose(wxCloseEvent &evt);
	void OnQuit(wxCommandEvent &evt);
	void OnOpen(wxCommandEvent &evt);
	void OnReset(wxCommandEvent &evt);
	void OnRun(wxCommandEvent &evt);
	void OnLogWindow(wxCommandEvent &evt);
	void OnLogLevel(wxCommandEvent &evt);
	void OnOptions(wxCommandEvent &evt);
	void OnTimer(wxTimerEvent &evt);
	void OnResize(wxSizeEvent &evt);
	void OnDebugCPU(wxCommandEvent &evt);
	void OnDebugMEM(wxCommandEvent &evt);
	void OnDebugGPU(wxCommandEvent &evt);
	void DrawGL(int user);
	bool InitGL(int user);
	void DestroyGL(int user);
};


#endif