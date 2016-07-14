#ifndef INPUTMASTER_H_
#define INPUTMASTER_H_

#include "../Emulator/Emulator.h"
#include "dinput.h"
#include "xinput.h"
#include "input.h"
#include <map>
#include <list>
#include <vector>
#include <string>
#include <wx/wx.h>

class InputMaster : public wxEvtHandler
{
public:
	InputMaster(wxWindow *parent);
	~InputMaster();

	void Tick(unsigned int deltaTime);

	void RegisterClient(Emulator *client);

	void OnKeyboard(wxKeyEvent &evt);
	void OnJoystick(JoystickEvent &evt);

	wxString GetKeyName(int id);

private:
	bool SendKey(int key, int pressed);
	wxString GetKeyboardKeyName(int id);

	Emulator *_emu;
	std::map<int, EmulatorInput_t> _bindings;
	DInput _dinput;
	XInput _xinput;
};

#endif