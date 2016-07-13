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

	void ClearAllClients();
	void ClearAllClients(std::string name);
	void ClearClient(const Emulator &client);

	void RegisterInputs(std::list<EmulatorInput_t> inputs, std::string name);
	void RegisterClient(const Emulator &client);

	void OnKeyboard(wxKeyEvent &evt);
	void OnJoystick(JoystickEvent &evt);

private:
	bool SendKey(std::string name, int key, int pressed);

	std::map<std::string, std::list<Emulator>> _clients;
	DInput _dinput;
	XInput _xinput;
};

#endif