
#include <GL/glew.h>

#include <errno.h>
#include <wx/ffile.h>
#include <wx/filename.h>

#include <emuall/support.h>
#include <emuall/exception.h>

#include "icon.h"
#include "MainFrame.h"
#include "../util/memDbg.h"
#include "IdList.h"
#include "GLPane.h"
#include "../Emulator/SaveFile.h"

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
	EVT_MENU_RANGE(ID_Main_Options_Filter, ID_Main_Options_Filter + 5, MainFrame::OnOptionVideoFilter)
	
	// General events
	//EVT_TIMER(ID_Timer, MainFrame::OnTimer)
	EVT_IDLE(MainFrame::OnIdle)
	EVT_CLOSE(MainFrame::OnClose)
END_EVENT_TABLE()

MainFrame::MainFrame(const wxString &title, const wxPoint &pos, const wxSize &size) :
	wxFrame(NULL, ID_MainFrame, title, pos, size)
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
	_inputOptionsFrame = NULL;

	// Constructor
	_logger = new wxLogWindow(this, _("Debug Log"), false, false);
	_logger->SetVerbose(true);
	_logger->SetLogLevel(wxLOG_Message);
	InitLog(&_logDst);

	// Load options
	Options::GetSingleton().LoadOptions();

	// Create layout
	CreateLayout();

	// create Options frames
	_inputOptionsFrame = new InputOptionsFrame(this);

	// Set the current context
	_display->SetCurrentContext();

	// Initialize support library
	emuallSupportInit();
	GLenum result;
	glewExperimental = true;
	if ((result = glewInit()) != GLEW_OK) {
		Log(Error, "glewInit failed: %s", glewGetErrorString(result));
	}

	// load plugins
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
	//_timer = new wxTimer(this, ID_Timer);
	//_timer->Start(16, false);
	Log(Message,"EmuAll started");
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
	// Set the window icon
	SetIcons(getIconBundle());

	// Create the layout
	CreateMenuBar();

	// Create the opengl screen
	wxGLAttributes glAttr;
	glAttr.PlatformDefaults();
	glAttr.DoubleBuffer();
	//glAttr.RGBA();
	glAttr.Depth(24);
	glAttr.Stencil(8);
	glAttr.EndList();
	wxGLContextAttrs ctxAttr;
	ctxAttr.CoreProfile();
	//ctxAttr.OGLVersion(3, 2);
	//ctxAttr.ForwardCompatible();
	ctxAttr.EndList();
	_display = new EmulatorScreen(this, this, 0, ID_Main_display, wxDefaultPosition,
		wxDefaultSize, wxFULL_REPAINT_ON_RESIZE, glAttr, ctxAttr);

	_display->Bind(wxEVT_KEY_UP, &InputMaster::OnKeyboard, &_inputHandler);
	_display->Bind(wxEVT_KEY_DOWN, &InputMaster::OnKeyboard, &_inputHandler);
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
	UpdateRecentFiles();
	_menFile->AppendSeparator();
	_menFile->Append(ID_Main_File_quit, _("&Quit"));

	// Create video filter menu
	wxMenu *videoFilterMenu = new wxMenu;
	videoFilterMenu->AppendRadioItem(ID_Main_Options_Filter + 0, _("Nearest"));
	videoFilterMenu->AppendRadioItem(ID_Main_Options_Filter + 1, _("Bi-Linear"));
	videoFilterMenu->AppendRadioItem(ID_Main_Options_Filter + 2, _("Bi-Cubic triangular"));
	videoFilterMenu->AppendRadioItem(ID_Main_Options_Filter + 3, _("Bi-Cubic Bell"));
	videoFilterMenu->AppendRadioItem(ID_Main_Options_Filter + 4, _("Bi-Cubic B-Spline"));
	videoFilterMenu->AppendRadioItem(ID_Main_Options_Filter + 5, _("Bi-Cubic CatMull-Rom"));
	int filterID = Options::GetSingleton().videoOptions.filter;
	if (filterID >= 5 || filterID < 0) {
		filterID = 0;
	}
	videoFilterMenu->Check(ID_Main_Options_Filter + filterID, true);

	// Create the options menu
	_menOptions = new wxMenu;
	_menOptions->AppendCheckItem(ID_Main_Options_KeepAspect, _("Keep aspect ratio"));
	_menOptions->Check(ID_Main_Options_KeepAspect, Options::GetSingleton().videoOptions.keepAspect);
	_menOptions->AppendSubMenu(videoFilterMenu, _("Video &filter"));
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
	Options &options = Options::GetSingleton();
	// Load the emulator
	// Detect and create the emulator
	EmulatorInterface *emu = _emulators->GetCompatibleEmulator(fileName.c_str());
	if (!emu)
	{
		Log(Error, "Could not find compatible emulator for %s", fileName.c_str());
		return;
	}
	Log(Message, "Loading the \"%s\" emulator", emu->GetName().c_str());

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
	
	// set current context
	_display->SetCurrentContext();

	// Initializing the emulator
	if (!emu->Init(handle, callback))
	{
		Log(Error, "Could not initialize the emulator");
		emu->ReleaseEmulator(handle);
		return;
	}

	EmulatorInfo_t info = emu->GetInfo();

	try {
		// Init the display
		assert(info.screens.size() <= 1);
		if (info.screens.size() != 0 && info.screens[0].width > 0 && info.screens[0].height > 0) {
			_display->SetFrameBufferSize(info.screens[0].width, info.screens[0].height);
		}
		else {
			int w, h;
			_display->GetSize(&w, &h);
			_display->SetFrameBufferSize(w, h);
			emu->Reshape(handle, _display->GetUserData(), w, h, options.videoOptions.keepAspect);
		}

		// Init the audio
		assert(info.numAudioStreams > 0);
		_audioCBData.clear();
		_audioCBData.reserve(info.numAudioStreams);
		for (int i = 0; i < info.numAudioStreams; ++i) {
			_audioCBData.emplace_back(&_emulator, i);
			AudioStream stream(AudioBuffer::Stereo16, options.audioOptions.sampleRate,
				options.audioOptions.bufferSize, options.audioOptions.numBuffers, 
				&MainFrame::AudioStreamCB, &_audioCBData.back());
			_audio.AddAudioStream(stream);
			emu->InitAudio(handle, i, options.audioOptions.sampleRate, 2);
		}
		_audio.Play();
	}
	catch (BaseException &e) {
		Log(Error, "%s\nStacktrace:\n%s", e.GetMsg(), e.GetStacktrace());
		throw;
	}
	

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
	_emulator.handle = handle;
	_emulator.emu = emu;

	// Register for keybindings
	_inputHandler.RegisterClient(_emulator);

	// Load debuggers
	_cpuDebugger->SetEmulator(_emulator);
	_memDebugger->SetEmulator(_emulator);
	_gpuDebugger->SetEmulator(_emulator);

	// Update save state labels
	Options::GetSingleton().SaveRecentFile(_filePath.c_str());
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

		try {
			_audio.Pause();
			_audio.ClearAudioStreams();
			_audioCBData.clear();
			_display->DestroyGL();
		}
		catch (BaseException &e) {
			Log(Error, "%s\nStacktrace:\n", e.GetMsg(), e.GetStacktrace());
			throw;
		}

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
	Options &options = Options::GetSingleton();
	for (int i = 0; i < 5; i++) {
		wxString name("---");
		if (!options.recentFiles[i].empty()) {
			wxFileName fileName(options.recentFiles[i]);
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

void MainFrame::RunEmulator(uint32_t deltaTime)
{
	static wxStopWatch sw;
	sw.Start(0);
	if (_emulator.emu != NULL)
	{
		if (_emulator.emu->Tick(_emulator.handle, deltaTime)) // GUI update necessary
		{
			Update();
		}
	}
	try {
		_audio.Tick();
		_display->Refresh();
	}
	catch (BaseException &e) {
		Log(Error, "BaseException caught: %s\nStacktrace:\n%s", e.GetMsg(), e.GetStacktrace());
	}
}

void MainFrame::OnIdle(wxIdleEvent &evt)
{
	uint32_t deltaTime = _deltaTimeTimer.TimeInMicro().GetLo();
	// Limit the amount of time simulated
	if (deltaTime > 30000)
		deltaTime = 30000;

	_deltaTimeTimer.Start(0);
	RunEmulator(deltaTime);
	uint32_t emuTime = _deltaTimeTimer.TimeInMicro().GetLo();
	if (emuTime < 16000) {
		// Yield to reduce CPU usage
		Sleep((16000-emuTime)/1000);
	}
	evt.RequestMore();
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

	Options::GetSingleton().SaveOptions();

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
	_filePath = Options::GetSingleton().recentFiles[idx];

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
	uint32_t deltaTime = _deltaTimeTimer.TimeInMicro().GetLo();
	// Limit the amount of time simulated
	if (deltaTime > 30000)
		deltaTime = 30000;

	_deltaTimeTimer.Start(0);
	RunEmulator(deltaTime);
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
		Options::GetSingleton().videoOptions.keepAspect = evt.IsChecked();
		if (_emulator.emu != NULL)
		{
			int width, height;
			_display->GetSize(&width, &height);
			_emulator.emu->Reshape(_emulator.handle, _display->GetUserData(), width, height, evt.IsChecked());
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

void MainFrame::OnOptionVideoFilter(wxCommandEvent &evt)
{
	EmulatorScreen::Filter filter;
	switch (evt.GetId()) {
	default:
	case ID_Main_Options_Filter + 0: // nearest
		filter = EmulatorScreen::Nearest;
		break;
	case ID_Main_Options_Filter + 1: // Bilinear
		filter = EmulatorScreen::BiLinear;
		break;
	case ID_Main_Options_Filter + 2: // bicubic triangular
		filter = EmulatorScreen::BicubicTriangular;
		break;
	case ID_Main_Options_Filter + 3: // bicubic bell
		filter = EmulatorScreen::BicubicBell;
		break;
	case ID_Main_Options_Filter + 4: // bicubic bspline
		filter = EmulatorScreen::BicubicBSpline;
		break;
	case ID_Main_Options_Filter + 5: // bicubic catmull-rom
		filter = EmulatorScreen::BicubicCatMullRom;
		break;
	}
	_display->SetPostProcessingFilter(filter);
}

void MainFrame::OnResize(wxSizeEvent &evt)
{
	if (_emulator.emu != NULL && _display != NULL)
	{
		int width, height;
		_display->GetSize(&width, &height);
		_emulator.emu->Reshape(_emulator.handle, _display->GetUserData(), width, height, Options::GetSingleton().videoOptions.keepAspect);
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

void MainFrame::AudioStreamCB(AudioBuffer::Format format, int freq, int elements, void *data, void *user)
{
	short *dataPtr = reinterpret_cast<short *>(data);
	MainFrame::AudioCallbackData *cbData = reinterpret_cast<MainFrame::AudioCallbackData *>(user);
	(void)freq;

	if (cbData != nullptr && cbData->emulator != nullptr && cbData->emulator->emu != nullptr) {
		cbData->emulator->emu->GetAudio(cbData->emulator->handle, cbData->id, dataPtr, elements);
	}
	else {
		memset(dataPtr, 0, elements * sizeof(short));
	}
}
