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
	_inputHandler(&_options)
{	
	_bar = NULL;
	_menFile = NULL;
	_menOptions = NULL;
	_menDebug = NULL;
	_menDebugLevel = NULL;
	_menSaveState = NULL;
	_menLoadState = NULL;
	_display = NULL;
	_logger = NULL;
	_timer = NULL;
	_emulator.emu = NULL;
	_emulator.handle = NULL;
	_emulators = NULL;
	_audio = NULL;
	_inputOptionsFrame = NULL;

	// Constructor
	_logger = new wxLogWindow(this, _("Debug Log"), false, false);
	_logger->SetVerbose(true);
	_logger->SetLogLevel(wxLOG_Message);
	InitLog(&_logDst);

	CreateLayout();

	// Load options
	_options.LoadOptions();
	_inputOptionsFrame = new InputOptionsFrame(this, &_options);
	UpdateRecentFiles();

	_emulators = new EmulatorList("plugins");
	std::list<EmulatorInterface *>::iterator it;
	for (it = _emulators->begin(); it != _emulators->end(); ++it)
	{
		_inputHandler.RegisterInputs((*it)->GetEmulatorInputs(), (*it)->GetName());
	}

	// Load debuggers
	_cpuDebugger = new CPUDebugger(this, wxID_ANY, _("CPU debugger"));
	_memDebugger = new MemDebugger(this, wxID_ANY, _("Memory debugger"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE & ~(wxRESIZE_BORDER|wxMAXIMIZE_BOX));
	_gpuDebugger = new GPUDebugger(this, wxID_ANY, _("GPU debugger"));

	_filePath.clear();
	_timer = new wxTimer(this, ID_Timer);
	_timer->Start(FPS, false);
	Log(Message,"EmuAll started");
	_audio = new Audio();
	_audio->Init();
}

MainFrame::~MainFrame()
{
	// Destructor
	if (_emulators)
		delete _emulators;
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
	//ctxAttr.OGLVersion(3, 2);
	ctxAttr.ForwardCompatible();
	ctxAttr.EndList();
	_display = new GLPane(this, this, 0, ID_Main_display, wxDefaultPosition,
		wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, glAttr, ctxAttr);

	//mDisplay->Connect(ID_Main_display, wxEVT_KEY_UP, wxKeyEventHandler(InputMaster::OnKeyboard), (wxObject *) NULL, &mInputHandler);
	//mDisplay->Connect(ID_Main_display, wxEVT_KEY_DOWN, wxKeyEventHandler(InputMaster::OnKeyboard), (wxObject *) NULL, &mInputHandler);
	_display->Bind(wxEVT_KEY_UP, &InputMaster::OnKeyboard, &_inputHandler);
	_display->Bind(wxEVT_KEY_DOWN, &InputMaster::OnKeyboard, &_inputHandler);
	//Bind(wxEVT_CHAR_HOOK, &InputMaster::OnKeyboard, &mInputHandler);
	_display->Connect(ID_Main_display, wxEVT_SIZE, wxSizeEventHandler(MainFrame::OnResize), (wxObject *) NULL, this);
}

void MainFrame::CreateMenuBar()
{	
	// Create the save state menu
	_menSaveState = new wxMenu;

	// Create the load state menu
	_menLoadState = new wxMenu;

	for (int i = 0; i < 10; i++) {
		_menSaveState->Append(ID_Main_File_SaveState + i, wxString::Format(_("Save State %d\tCtrl+Shift+F%d"), i + 1, i + 1));
		_menLoadState->Append(ID_Main_File_LoadState + i, wxString::Format(_("Load State %d\tCtrl+F%d"), i + 1, i + 1));
	}
	UpdateSaveStateLabels();
	Bind(wxEVT_MENU, &MainFrame::OnSaveState, this, ID_Main_File_SaveState, ID_Main_File_SaveState + 9);
	Bind(wxEVT_MENU, &MainFrame::OnLoadState, this, ID_Main_File_LoadState, ID_Main_File_LoadState + 9);

	// Create the File menu
	_menFile = new wxMenu;
	_menFile->Append(ID_Main_File_open, _("&Open\tCtrl+O"));
	_menFile->Append(ID_Main_File_run, _("&Run\tCtrl+R"));
	_menFile->Append(ID_Main_File_reset, _("Reset"));
	_menFile->AppendSeparator();
	_menFile->AppendSubMenu(_menSaveState, _("&Save state"));
	_menFile->AppendSubMenu(_menLoadState, _("&Load state"));
	_menFile->AppendSeparator();
	for (int i = 0; i < 5; i++) {
		_menFile->Append(ID_Main_File_RecentFile + i, wxString::Format(_("%d. ---"), i + 1));
	}
	Bind(wxEVT_MENU, &MainFrame::OnOpenRecentFile, this, ID_Main_File_RecentFile, ID_Main_File_RecentFile + 4);
	_menFile->AppendSeparator();
	_menFile->Append(ID_Main_File_quit, _("&Quit"));

	// Create the options menu
	_menOptions = new wxMenu;
	_menOptions->AppendCheckItem(ID_Main_Options_KeepAspect, _("Keep aspect ratio"));
	_menOptions->Check(ID_Main_Options_KeepAspect, _options.videoOptions.keepAspect);
	_menOptions->Append(ID_Main_Options_input, _("&Input"));

	// Create the debug level menu
	_menDebugLevel = new wxMenu;
	_menDebugLevel->AppendRadioItem(ID_Main_Debug_level_fat, _("&Fatal"));
	_menDebugLevel->AppendRadioItem(ID_Main_Debug_level_err, _("&Error"));
	_menDebugLevel->AppendRadioItem(ID_Main_Debug_level_war, _("&Warning"));
	_menDebugLevel->AppendRadioItem(ID_Main_Debug_level_mes, _("&Message"))->Check(true);
	_menDebugLevel->AppendRadioItem(ID_Main_Debug_level_deb, _("&Debug"));

	// Create the debug menu
	_menDebug = new wxMenu;
	_menDebug->Append(ID_Main_Debug_cpu, _("&CPU"));
	_menDebug->Append(ID_Main_Debug_mem, _("&Memory"));
	_menDebug->Append(ID_Main_Debug_gpu, _("&GPU"));
	_menDebug->Append(ID_Main_Debug_window, _("&Debug"));
	_menDebug->AppendSubMenu(_menDebugLevel, _("Debug &Level"));

	// create the menubar
	_bar = new wxMenuBar;
	_bar->Append(_menFile, _("&File"));
	_bar->Append(_menOptions, _("&Options"));
	_bar->Append(_menDebug, _("&Debug"));

	SetMenuBar(_bar);
}

void MainFrame::DrawGL(int user)
{
	if (_emulator.emu != NULL)
	{
		_emulator.emu->Draw(_emulator.handle, user);
	}
	else {
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void MainFrame::LoadEmulator(std::string &fileName)
{
	// Load the emulator
	// Detect and create the emulator
	EmulatorInterface *emu = _emulators->GetCompatibleEmulator(fileName.c_str());
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
	_display->GetSize(&w, &h);
	emu->Reshape(handle, w, h, _options.videoOptions.keepAspect);

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
	_saveFilePath = fileName;
	_saveFilePath.append(".sav");
	_saveStateFilePath= fileName;
	_saveStateFilePath.append(".ss");
	Log(Message, "Loading save data");
	SaveFile::ReadSaveFile(_saveFilePath, saveData);

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
	_emulator.emu = emu;
	_emulator.handle = handle;

	// Register for keybindings
	_inputHandler.RegisterClient(_emulator);

	// Load debuggers
	_cpuDebugger->SetEmulator(_emulator);
	_memDebugger->SetEmulator(_emulator);
	_gpuDebugger->SetEmulator(_emulator);

	// Update save state labels
	_options.SaveRecentFile(_filePath.c_str());
	UpdateSaveStateLabels();
	UpdateRecentFiles();
	return;
}

void MainFrame::CloseEmulator()
{
	// is a emulator running, yes then close it
	if (_emulator.emu != NULL)
	{
		Log(Message, "Emulator closed");
		Emulator emu;
		emu.emu = NULL;
		emu.handle = NULL;

		_cpuDebugger->SetEmulator(emu);
		_memDebugger->SetEmulator(emu);
		_gpuDebugger->SetEmulator(emu);

		_display->DestroyGL();

		_inputHandler.ClearClient(_emulator);
		SaveData_t saveData;
		saveData.miscData = saveData.ramData = saveData.romData = NULL;
		saveData.miscDataLen = saveData.ramDataLen = saveData.romDataLen = 0;
		_emulator.emu->Save(_emulator.handle, &saveData);
		SaveFile::WriteSaveFile(_saveFilePath, saveData);
		_emulator.emu->ReleaseSaveData(_emulator.handle, &saveData);
		_emulator.emu->ReleaseEmulator(_emulator.handle);

		_emulator = emu;
	}
}

void MainFrame::UpdateSaveStateLabels()
{
	for (int i = 0; i < 10; i++) {
		wxString filePath = _saveStateFilePath;
		filePath.Append(std::to_string(i));
		wxFileName file(filePath);
		wxString name;
		if (file.FileExists()) {
			wxDateTime date = file.GetModificationTime();
			name = wxString::Format(_("(%s)"),date.Format());
		}
		_menSaveState->SetLabel(ID_Main_File_SaveState + i, wxString::Format(_("Save State %d %s\tCtrl+Shift+F%d"), i + 1, name, i + 1));
		_menLoadState->SetLabel(ID_Main_File_LoadState + i, wxString::Format(_("Load State %d %s\tCtrl+F%d"), i + 1, name, i + 1));
	}
}

void MainFrame::UpdateRecentFiles()
{
	const int maxLength = 35;
	for (int i = 0; i < 5; i++) {
		wxString name("---");
		if (!_options.recentFiles[i].empty()) {
			wxFileName fileName(_options.recentFiles[i]);
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
		_menFile->SetLabel(ID_Main_File_RecentFile + i, wxString::Format(_("%d. %s"), i+1, name));
	}
}

void MainFrame::Update()
{
	if (_cpuDebugger != NULL)
		_cpuDebugger->Update();
	if (_memDebugger != NULL)
		_memDebugger->Update();
	if (_gpuDebugger != NULL)
		_gpuDebugger->Update();
}

void MainFrame::OnClose(wxCloseEvent &evt)
{
	// Event when the frame closes
	CloseEmulator();
	if(_timer != NULL)
	{
		_timer->Stop();
		delete _timer;
		_timer = NULL;
	}
	if(_audio != NULL)
	{
		_audio->Close();
		delete _audio;
		_audio = NULL;
	}

	_options.SaveOptions();

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
	assert(_emulators != NULL);
	// Event when clicked on the file open button

	// Filter
	std::string filter = _emulators->GetFileFilters();
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
	_filePath = openFileDialog.GetPath();

	// Load the file
	LoadEmulator(_filePath);
}

void MainFrame::OnOpenRecentFile(wxCommandEvent &evt)
{
	int idx = evt.GetId() - ID_Main_File_RecentFile;
	if (idx < 0 || idx >= 5) return;
	_filePath = _options.recentFiles[idx];

	CloseEmulator();
	LoadEmulator(_filePath);
}

void MainFrame::OnRun(wxCommandEvent &evt)
{
	if (_emulator.emu != NULL)
	{
		_emulator.emu->Run(_emulator.handle, true);
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
		_emulator.emu->SaveState(_emulator.handle, &saveData);

		// Write it to an file
		wxString fileName = _saveStateFilePath;
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
		wxString fileName = _saveStateFilePath;
		fileName.Append(std::to_string(state));
		SaveFile::ReadStateFile(fileName, saveData);

		_emulator.emu->LoadState(_emulator.handle, &saveData);
		Update();
	}
}

void MainFrame::OnLogWindow(wxCommandEvent &evt)
{
	_logger->Show(true);
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
	if(_emulator.emu != NULL)
	{
		if (_emulator.emu->Tick(_emulator.handle, time)) // GUI update necessary
		{
			Update();
		}
	}
	_display->Refresh();
	
}

void MainFrame::OnReset(wxCommandEvent &evt)
{
	if(_filePath.empty())
	{
		OnOpen(wxCommandEvent());
		return;
	}
	// Close the emulator
	CloseEmulator();
	
	// Load the file
	LoadEmulator(_filePath);
}

void MainFrame::OnOptions(wxCommandEvent &evt)
{
	switch (evt.GetId())
	{
	case ID_Main_Options_KeepAspect:
		_options.videoOptions.keepAspect = evt.IsChecked();
		if (_emulator.emu != NULL)
		{
			int width, height;
			_display->GetSize(&width, &height);
			_emulator.emu->Reshape(_emulator.handle, width, height, evt.IsChecked());
		}
		break;
	case ID_Main_Options_input:
		_inputOptionsFrame->Show();
		break;
	}
}

void MainFrame::OnDebugCPU(wxCommandEvent &evt)
{
	_cpuDebugger->Show();
}

void MainFrame::OnDebugMEM(wxCommandEvent &evt)
{
	_memDebugger->Show();
}

void MainFrame::OnDebugGPU(wxCommandEvent &evt)
{
	_gpuDebugger->Show();
}

void MainFrame::OnResize(wxSizeEvent &evt)
{
	if (_emulator.emu != NULL && _display != NULL)
	{
		int width, height;
		_display->GetSize(&width, &height);
		_emulator.emu->Reshape(_emulator.handle, width, height, _options.videoOptions.keepAspect);
	}
	if (_display != NULL && _emulator.emu == NULL)
	{
		_display->Refresh();
	}
}

bool MainFrame::InitGL(int user)
{
	if (_emulator.emu != NULL)
	{
		return _emulator.emu->InitGL(_emulator.handle, user);
	}
	else {
		return false;
	}
}

void MainFrame::DestroyGL(int user)
{
	if (_emulator.emu != NULL)
	{
		_emulator.emu->DestroyGL(_emulator.handle, user);
	}
}