#include "MainFrame.h"
#include "../util/memDbg.h"
#include "IdList.h"
#include "GLPane.h"
#include "../Emulator/SaveFile.h"

#include <errno.h>
#include <wx/ffile.h>
#include <wx/filename.h>

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
	mMenSaveState = NULL;
	mMenLoadState = NULL;
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
	UpdateRecentFiles();

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
	wxGLAttributes glAttr;
	glAttr.Reset();
	glAttr.PlatformDefaults();
	glAttr.DoubleBuffer();
	glAttr.RGBA();
	glAttr.Depth(24);
	glAttr.Stencil(8);
	glAttr.EndList();
	wxGLContextAttrs ctxAttr;
	ctxAttr.Reset();
	ctxAttr.CoreProfile();
	ctxAttr.OGLVersion(3, 2);
	ctxAttr.ForwardCompatible();
	ctxAttr.EndList();
	mDisplay = new GLPane(this, this, 0, ID_Main_display, wxDefaultPosition,
		wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, glAttr, ctxAttr);

	//mDisplay->Connect(ID_Main_display, wxEVT_KEY_UP, wxKeyEventHandler(InputMaster::OnKeyboard), (wxObject *) NULL, &mInputHandler);
	//mDisplay->Connect(ID_Main_display, wxEVT_KEY_DOWN, wxKeyEventHandler(InputMaster::OnKeyboard), (wxObject *) NULL, &mInputHandler);
	mDisplay->Bind(wxEVT_KEY_UP, &InputMaster::OnKeyboard, &mInputHandler);
	mDisplay->Bind(wxEVT_KEY_DOWN, &InputMaster::OnKeyboard, &mInputHandler);
	//Bind(wxEVT_CHAR_HOOK, &InputMaster::OnKeyboard, &mInputHandler);
	mDisplay->Connect(ID_Main_display, wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnResize), (wxObject *) NULL, this);
}

void MainFrame::CreateMenuBar()
{	
	// Create the save state menu
	mMenSaveState = new wxMenu;

	// Create the load state menu
	mMenLoadState = new wxMenu;

	for (int i = 0; i < 10; i++) {
		mMenSaveState->Append(ID_Main_File_SaveState + i, wxString::Format(_("Save State %d\tCtrl+Shift+F%d"), i + 1, i + 1));
		mMenLoadState->Append(ID_Main_File_LoadState + i, wxString::Format(_("Load State %d\tCtrl+F%d"), i + 1, i + 1));
	}
	UpdateSaveStateLabels();
	Bind(wxEVT_MENU, &MainFrame::OnSaveState, this, ID_Main_File_SaveState, ID_Main_File_SaveState + 9);
	Bind(wxEVT_MENU, &MainFrame::OnLoadState, this, ID_Main_File_LoadState, ID_Main_File_LoadState + 9);

	// Create the File menu
	mMenFile = new wxMenu;
	mMenFile->Append(ID_Main_File_open, _("&Open\tCtrl+O"));
	mMenFile->Append(ID_Main_File_run, _("&Run\tCtrl+R"));
	mMenFile->Append(ID_Main_File_reset, _("Reset"));
	mMenFile->AppendSeparator();
	mMenFile->AppendSubMenu(mMenSaveState, _("&Save state"));
	mMenFile->AppendSubMenu(mMenLoadState, _("&Load state"));
	mMenFile->AppendSeparator();
	for (int i = 0; i < 5; i++) {
		mMenFile->Append(ID_Main_File_RecentFile + i, wxString::Format(_("%d. ---"), i + 1));
	}
	Bind(wxEVT_MENU, &MainFrame::OnOpenRecentFile, this, ID_Main_File_RecentFile, ID_Main_File_RecentFile + 4);
	mMenFile->AppendSeparator();
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

	SaveData_t saveData;
	wxFFile romFile(fileName, "rb");
	if (!romFile.IsOpened()) {
		Log(Error, "Could not open the ROM file '%s'", fileName.c_str());
		emu->ReleaseEmulator(handle);
		return;
	}
	saveData.romDataLen = romFile.Length();
	saveData.romData = new uint8_t[saveData.romDataLen];
	int read = romFile.Read(saveData.romData, saveData.romDataLen);
	if (read != saveData.romDataLen) {
		if (romFile.Error()) {
			Log(Error, "Error occurred while reading ROM data");
			emu->ReleaseEmulator(handle);
			delete[] saveData.romData;
			saveData.romDataLen = 0;
			return;
		}
		if (romFile.Eof()) {
			Log(Warn, "End of ROM file reached before all the data was read");
			saveData.romDataLen = read;
		}
	}
	mSaveFilePath = fileName;
	mSaveFilePath.append(".sav");
	mSaveStateFilePath= fileName;
	mSaveStateFilePath.append(".ss");
	Log(Message, "Loading save data");
	SaveFile::ReadSaveFile(mSaveFilePath, saveData);

	if (!emu->Load(handle, &saveData))
	{
		Log(Error, "Could not load the ROM");
		emu->ReleaseEmulator(handle);
		return;
	}

	if (saveData.romData != NULL) delete[] saveData.romData;
	if (saveData.ramData != NULL) delete[] saveData.ramData;
	if (saveData.miscData != NULL) delete[] saveData.miscData;

	// store emulator
	mEmulator.emu = emu;
	mEmulator.handle = handle;

	// Register for keybindings
	mInputHandler.RegisterClient(mEmulator);

	// Load debuggers
	mCpuDebugger->SetEmulator(mEmulator);
	mMemDebugger->SetEmulator(mEmulator);
	mGpuDebugger->SetEmulator(mEmulator);

	// Update save state labels
	mOptions.SaveRecentFile(mFilePath.c_str());
	UpdateSaveStateLabels();
	UpdateRecentFiles();
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
		SaveData_t saveData;
		saveData.miscData = saveData.ramData = saveData.romData = NULL;
		saveData.miscDataLen = saveData.ramDataLen = saveData.romDataLen = 0;
		mEmulator.emu->Save(mEmulator.handle, &saveData);
		SaveFile::WriteSaveFile(mSaveFilePath, saveData);
		mEmulator.emu->ReleaseSaveData(mEmulator.handle, &saveData);
		mEmulator.emu->ReleaseEmulator(mEmulator.handle);

		mEmulator = emu;
	}
}

void MainFrame::UpdateSaveStateLabels()
{
	for (int i = 0; i < 10; i++) {
		wxString filePath = mSaveStateFilePath;
		filePath.Append(std::to_string(i));
		wxFileName file(filePath);
		wxString name;
		if (file.FileExists()) {
			wxDateTime date = file.GetModificationTime();
			name = wxString::Format(_("(%s)"),date.Format());
		}
		mMenSaveState->SetLabel(ID_Main_File_SaveState + i, wxString::Format(_("Save State %d %s\tCtrl+Shift+F%d"), i + 1, name, i + 1));
		mMenLoadState->SetLabel(ID_Main_File_LoadState + i, wxString::Format(_("Load State %d %s\tCtrl+F%d"), i + 1, name, i + 1));
	}
}

void MainFrame::UpdateRecentFiles()
{
	const int maxLength = 35;
	for (int i = 0; i < 5; i++) {
		wxString name("---");
		if (!mOptions.recentFiles[i].empty()) {
			wxFileName fileName(mOptions.recentFiles[i]);
			wxString path = fileName.GetFullPath();
			if (path.length() > maxLength) {
				wxString base = fileName.GetVolume() + wxFileName::GetVolumeSeparator() + wxFileName::GetPathSeparator() + 
					".." + wxFileName::GetPathSeparator();
				const wxArrayString &dirs = fileName.GetDirs();
				unsigned numDirs = 0;
				int len = base.length() + fileName.GetFullName().length();
				while(len < maxLength && numDirs < dirs.size()) {
					len += 1 + dirs[dirs.size() - 1 - numDirs].length();
					numDirs++;
				}
				name = base;
				for (unsigned j = dirs.size() - numDirs + 1; j < dirs.size(); j++) {
					name += dirs[j] + wxFileName::GetPathSeparator();
				}
				name += fileName.GetFullName();
			}
		}
		mMenFile->SetLabel(ID_Main_File_RecentFile + i, wxString::Format(_("%d. %s"), i+1, name));
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
		Log(Debug, "Loading canceled");
		return;
	}
	CloseEmulator();

	// Check if the file exists
	mFilePath = openFileDialog.GetPath();

	// Load the file
	LoadEmulator(mFilePath);
}

void MainFrame::OnOpenRecentFile(wxCommandEvent &evt)
{
	int idx = evt.GetId() - ID_Main_File_RecentFile;
	if (idx < 0 || idx >= 5) return;
	mFilePath = mOptions.recentFiles[idx];
	
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

void MainFrame::OnSaveState(wxCommandEvent &evt)
{
	if (evt.GetEventType() == wxEVT_MENU) {
		int state = evt.GetId() - ID_Main_File_SaveState;
		if (state < 0 || state >= 10) {
			Log(Warn, "Invalid save state event received");
			return;
		}

		// Save the state
		SaveData_t saveData;
		saveData.romData = saveData.ramData = saveData.miscData = NULL;
		saveData.romDataLen = saveData.ramDataLen = saveData.miscDataLen = 0;
		mEmulator.emu->SaveState(mEmulator.handle, &saveData);

		// Write it to an file
		wxString fileName = mSaveStateFilePath;
		fileName.Append(std::to_string(state));
		SaveFile::WriteStateFile(fileName, saveData);
	}
	UpdateSaveStateLabels();
}

void MainFrame::OnLoadState(wxCommandEvent &evt)
{
	if (evt.GetEventType() == wxEVT_MENU) {
		int state = evt.GetId() - ID_Main_File_LoadState;
		if (state < 0 || state >= 10) {
			Log(Warn, "Invalid save state event received");
			return;
		}

		// Save the state
		SaveData_t saveData;

		// Read state from an file
		wxString fileName = mSaveStateFilePath;
		fileName.Append(std::to_string(state));
		SaveFile::ReadStateFile(fileName, saveData);

		mEmulator.emu->LoadState(mEmulator.handle, &saveData);
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