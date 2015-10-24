#include "MainFrame.h"
#include "../util/memDbg.h"
#include "IdList.h"
#include "GLPane.h"

#include <errno.h>

BEGIN_EVENT_TABLE(MainFrame, wxFrame)
	EVT_MENU(ID_Main_File_quit, MainFrame::OnQuit)
	EVT_MENU(ID_Main_File_open, MainFrame::OnOpen)
	EVT_MENU(ID_Main_File_reset, MainFrame::OnReset)
	EVT_MENU(ID_Main_File_run, MainFrame::OnRun)
	EVT_MENU(ID_Main_Debug_window, MainFrame::OnLogWindow)
	// Debug levels
	EVT_MENU(ID_Main_Debug_level_fat, MainFrame::OnLogLevel)
	EVT_MENU(ID_Main_Debug_level_err, MainFrame::OnLogLevel)
	EVT_MENU(ID_Main_Debug_level_war, MainFrame::OnLogLevel)
	EVT_MENU(ID_Main_Debug_level_mes, MainFrame::OnLogLevel)
	EVT_MENU(ID_Main_Debug_level_deb, MainFrame::OnLogLevel)
	// Debug menu
	EVT_MENU(ID_Main_Debug_cpu, MainFrame::OnDebugCPU)
	EVT_MENU(ID_Main_Debug_mem, MainFrame::OnDebugMEM)
	EVT_MENU(ID_Main_Debug_gpu, MainFrame::OnDebugGPU)
	// Options
	EVT_MENU(ID_Main_Options_KeepAspect, MainFrame::OnOptions)
	EVT_MENU(ID_Main_Options_input, MainFrame::OnOptions)

	EVT_TIMER(ID_Timer, MainFrame::OnTimer)

	EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size) :
	wxFrame(NULL, ID_MainFrame, title, pos, size),
	mInputHandler(&mOptions)
{	
	mBar = NULL;
	mMenFile = NULL;
	mMenOptions = NULL;
	mMenDebug = NULL;
	mMenDebugLevel = NULL;
	mDisplay = NULL;
	mLogger = NULL;
	mTimer = NULL;
	mEmulator.emu = NULL;
	mEmulator.handle = NULL;
	mEmulators = NULL;
	mAudio = NULL;
	mInputOptionsFrame = NULL;

	CreateLayout();

	// Constructor
	mLogger = new wxLogWindow(this, _("Debug Log"), false, false);
	mLogger->SetVerbose(true);
	mLogger->SetLogLevel(wxLOG_Message);
	InitLog(&mLogDst);

	// Load options
	mOptions.LoadOptions();
	mInputOptionsFrame = new InputOptionsFrame(this, &mOptions);

	mEmulators = new EmulatorList("plugins");
	std::list<EmulatorInterface *>::iterator it;
	for (it = mEmulators->begin(); it != mEmulators->end(); ++it)
	{
		mInputHandler.RegisterInputs((*it)->GetEmulatorInputs(), (*it)->GetName());
	}

	// Load debuggers
	mCpuDebugger = new CPUDebugger(this, wxID_ANY, _("CPU debugger"));
	mMemDebugger = new MemDebugger(this, wxID_ANY, _("Memory debugger"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER|wxMAXIMIZE_BOX));
	mGpuDebugger = new GPUDebugger(this, wxID_ANY, _("GPU debugger"));

	mFilePath.clear();
	mTimer = new wxTimer(this, ID_Timer);
	mTimer->Start(FPS, false);
	Log(Message,"EmuAll started");
	mAudio = new Audio();
	mAudio->Init();
}

MainFrame::~MainFrame()
{
	// Destructor
	if (mEmulators)
		delete mEmulators;
	Log(Message, "EmuAll stopped");
}

void MainFrame::CreateLayout()
{
	// Create the layout
	CreateMenuBar();

	// Create the opengl screen
	int args[] = {WX_GL_RGBA, WX_GL_DOUBLEBUFFER, WX_GL_DEPTH_SIZE, 32, 0 };
	mDisplay = new GLPane(this, this, 0, ID_Main_display, wxDefaultPosition,
		wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, args);

	mDisplay->Connect(ID_Main_display, wxEVT_KEY_UP, wxKeyEventHandler(InputMaster::OnKeyboard), (wxObject *) NULL, &mInputHandler);
	mDisplay->Connect(ID_Main_display, wxEVT_KEY_DOWN, wxKeyEventHandler(InputMaster::OnKeyboard), (wxObject *) NULL, &mInputHandler);
	mDisplay->Connect(ID_Main_display, wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnResize), (wxObject *) NULL, this);
}

void MainFrame::CreateMenuBar()
{	
	// Create the File menu
	mMenFile = new wxMenu;
	mMenFile->Append(ID_Main_File_open, _("&Open"));
	mMenFile->Append(ID_Main_File_run, _("&Run"));
	mMenFile->Append(ID_Main_File_reset, _("&Reset"));
	mMenFile->Append(ID_Main_File_quit, _("&Quit"));

	// Create the options menu
	mMenOptions = new wxMenu;
	mMenOptions->AppendCheckItem(ID_Main_Options_KeepAspect, _("Keep aspect ratio"));
	mMenOptions->Check(ID_Main_Options_KeepAspect, mOptions.videoOptions.keepAspect);
	mMenOptions->Append(ID_Main_Options_input, _("&Input"));

	// Create the debug level menu
	mMenDebugLevel = new wxMenu;
	mMenDebugLevel->AppendRadioItem(ID_Main_Debug_level_fat, _("&Fatal"));
	mMenDebugLevel->AppendRadioItem(ID_Main_Debug_level_err, _("&Error"));
	mMenDebugLevel->AppendRadioItem(ID_Main_Debug_level_war, _("&Warning"));
	mMenDebugLevel->AppendRadioItem(ID_Main_Debug_level_mes, _("&Message"))->Check(true);
	mMenDebugLevel->AppendRadioItem(ID_Main_Debug_level_deb, _("&Debug"));

	// Create the debug menu
	mMenDebug = new wxMenu;
	mMenDebug->Append(ID_Main_Debug_cpu, _("&CPU"));
	mMenDebug->Append(ID_Main_Debug_mem, _("&Memory"));
	mMenDebug->Append(ID_Main_Debug_gpu, _("&GPU"));
	mMenDebug->Append(ID_Main_Debug_window, _("&Debug"));
	mMenDebug->AppendSubMenu(mMenDebugLevel, _("Debug &Level"));

	// create the menubar
	mBar = new wxMenuBar;
	mBar->Append(mMenFile, _("&File"));
	mBar->Append(mMenOptions, _("&Options"));
	mBar->Append(mMenDebug, _("&Debug"));

	SetMenuBar(mBar);
}

void MainFrame::DrawGL(int user)
{
	if (mEmulator.emu != NULL)
	{
		mEmulator.emu->Draw(mEmulator.handle, user);
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void MainFrame::LoadEmulator(std::string &fileName)
{
	// Load the emulator
	// Detect and create the emulator
	EmulatorInterface *emu = mEmulators->GetCompatibleEmulator(fileName.c_str());
	if (!emu)
	{
		Log(Error, "Could not find compatible emulator for %s", fileName.c_str());
		return;
	}
	Log(Message, "Loading the \"%s\" emulator", emu->GetName().c_str());
	
	EmulatorSettings_t configuration = emu->GetEmulatorSettings();

	EMUHANDLE handle = emu->CreateEmulator();
	if (!handle)
	{
		Log(Error, "Could not create the emulator");
		return;
	}
	Log(Message, "Initializing the emulator");

	// Loading the callback functions
	callBackfunctions_t callback;
	callback.Log = Log;
	
	// Initializing the emulator
	if (!emu->Init(handle, callback))
	{
		Log(Error, "Could not initialize the emulator");
		emu->ReleaseEmulator(handle);
		return;
	}

	int w, h;
	mDisplay->GetSize(&w, &h);
	emu->Reshape(handle, w, h, mOptions.videoOptions.keepAspect);

	// Loading the rom in memory
	Log(Message, "Loading the ROM in memory");
	if (!emu->Load(handle, fileName.c_str()))
	{
		Log(Error, "Could not load the ROM");
		emu->ReleaseEmulator(handle);
		return;
	}

	// store emulator
	mEmulator.emu = emu;
	mEmulator.handle = handle;

	// Register for keybindings
	mInputHandler.RegisterClient(mEmulator);

	// Load debuggers
	mCpuDebugger->SetEmulator(mEmulator);
	mMemDebugger->SetEmulator(mEmulator);
	mGpuDebugger->SetEmulator(mEmulator);
	return;
}

void MainFrame::CloseEmulator()
{
	// is a emulator running, yes then close it
	if (mEmulator.emu != NULL)
	{
		Log(Message, "Emulator closed");
		Emulator emu;
		emu.emu = NULL;
		emu.handle = NULL;

		mCpuDebugger->SetEmulator(emu);
		mMemDebugger->SetEmulator(emu);
		mGpuDebugger->SetEmulator(emu);

		mDisplay->DestroyGL();

		mInputHandler.ClearClient(mEmulator);

		mEmulator.emu->Save(mEmulator.handle, mFilePath.c_str());
		mEmulator.emu->ReleaseEmulator(mEmulator.handle);

		mEmulator = emu;
	}
}

void MainFrame::Update()
{
	if (mCpuDebugger != NULL)
		mCpuDebugger->Update();
	if (mMemDebugger != NULL)
		mMemDebugger->Update();
	if (mGpuDebugger != NULL)
		mGpuDebugger->Update();
}

void MainFrame::OnClose(wxCloseEvent &evt)
{
	// Event when the frame closes
	CloseEmulator();
	if(mTimer != NULL)
	{
		mTimer->Stop();
		delete mTimer;
		mTimer = NULL;
	}
	if(mAudio != NULL)
	{
		mAudio->Close();
		delete mAudio;
		mAudio = NULL;
	}

	mOptions.SaveOptions();

	// Destroy the window
	Destroy();
}

void MainFrame::OnQuit(wxCommandEvent &evt)
{
	// Event when clicked on quit button
	Close(true);
}

void MainFrame::OnOpen(wxCommandEvent &evt)
{
	assert(mEmulators != NULL);
	// Event when clicked on the file open button

	// Filter
	std::string filter = mEmulators->GetFileFilters();
	if (!filter.empty())
	{
		filter.append("|");
	}
	filter.append("All files(*.*)|*.*");
	// Open the file dialog
	wxFileDialog openFileDialog(this, _("Open rom file"), wxEmptyString,
								wxEmptyString, filter,
								wxFD_OPEN|wxFD_FILE_MUST_EXIST);

	// Check if a file was choosen
	if(openFileDialog.ShowModal() == wxID_CANCEL)
	{
		Log(Debug, "Loading cancelled");
		return;
	}
	CloseEmulator();

	// Check if the file exists
	mFilePath = openFileDialog.GetPath();

	// Load the file
	LoadEmulator(mFilePath);
}

void MainFrame::OnRun(wxCommandEvent &evt)
{
	if (mEmulator.emu != NULL)
	{
		mEmulator.emu->Run(mEmulator.handle, true);
		Update();
	}
}

void MainFrame::OnLogWindow(wxCommandEvent &evt)
{
	mLogger->Show(true);
}

void MainFrame::OnLogLevel(wxCommandEvent &evt)
{
	switch(evt.GetId())
	{
	case ID_Main_Debug_level_fat:
		SetLogLevel(Fatal);
		break;
	case ID_Main_Debug_level_err:
		SetLogLevel(Error);
		break;
	case ID_Main_Debug_level_war:
		SetLogLevel(Warn);
		break;
	case ID_Main_Debug_level_mes:
		SetLogLevel(Message);
		break;
	case ID_Main_Debug_level_deb:
		SetLogLevel(Debug);
		break;
	default:
		SetLogLevel(Message);
		break;
	}
}

void MainFrame::OnTimer(wxTimerEvent &evt)
{
	static wxStopWatch sw;
	uint32_t time = sw.TimeInMicro().GetLo(); // Calculate the actual simulation time needed
	if (time > FPS * 2000) 
		time = FPS*2000;
	sw.Start(0);
	if(mEmulator.emu != NULL)
	{
		if (mEmulator.emu->Tick(mEmulator.handle, time)) // GUI update necessary
		{
			Update();
		}
	}
	mDisplay->Refresh();
	
}

void MainFrame::OnReset(wxCommandEvent &evt)
{
	if(mFilePath.empty())
	{
		OnOpen(wxCommandEvent());
		return;
	}
	// Close the emulator
	CloseEmulator();
	
	// Load the file
	LoadEmulator(mFilePath);
}

void MainFrame::OnOptions(wxCommandEvent &evt)
{
	switch (evt.GetId())
	{
	case ID_Main_Options_KeepAspect:
		mOptions.videoOptions.keepAspect = evt.IsChecked();
		if (mEmulator.emu != NULL)
		{
			int width, height;
			mDisplay->GetSize(&width, &height);
			mEmulator.emu->Reshape(mEmulator.handle, width, height, evt.IsChecked());
		}
		break;
	case ID_Main_Options_input:
		mInputOptionsFrame->Show();
		break;
	}
}

void MainFrame::OnDebugCPU(wxCommandEvent &evt)
{
	mCpuDebugger->Show();
}

void MainFrame::OnDebugMEM(wxCommandEvent &evt)
{
	mMemDebugger->Show();
}

void MainFrame::OnDebugGPU(wxCommandEvent &evt)
{
	mGpuDebugger->Show();
}

void MainFrame::OnResize(wxSizeEvent &evt)
{
	if (mEmulator.emu != NULL && mDisplay != NULL)
	{
		int width, height;
		mDisplay->GetSize(&width, &height);
		mEmulator.emu->Reshape(mEmulator.handle, width, height, mOptions.videoOptions.keepAspect);
	}
	if (mDisplay != NULL && mEmulator.emu == NULL)
	{
		mDisplay->Refresh();
	}
}

bool MainFrame::InitGL(int user)
{
	if (mEmulator.emu != NULL)
	{
		return mEmulator.emu->InitGL(mEmulator.handle, user);
	}
	else {
		return false;
	}
}

void MainFrame::DestroyGL(int user)
{
	if (mEmulator.emu != NULL)
	{
		mEmulator.emu->DestroyGL(mEmulator.handle, user);
	}
}