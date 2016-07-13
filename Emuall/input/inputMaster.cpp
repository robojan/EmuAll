

#include "inputMaster.h"
#include "../util/memDbg.h"
#include "../util/log.h"
#include "../util/Options.h"

InputMaster::InputMaster(wxWindow *parent) :
	_dinput(this, (HWND)parent->GetHWND()), _xinput(this)
{
	Bind(JOYSTICK_BUTTON_DOWN_EVENT, &InputMaster::OnJoystick, this, wxID_ANY, wxID_ANY, nullptr);
	Bind(JOYSTICK_BUTTON_UP_EVENT, &InputMaster::OnJoystick, this, wxID_ANY, wxID_ANY, nullptr);
	Bind(JOYSTICK_AXES_EVENT, &InputMaster::OnJoystick, this, wxID_ANY, wxID_ANY, nullptr);
	Bind(JOYSTICK_HAT_EVENT, &InputMaster::OnJoystick, this, wxID_ANY, wxID_ANY, nullptr);
}

InputMaster::~InputMaster()
{

}


void InputMaster::Tick(unsigned int deltaTime)
{
	_xinput.Tick(deltaTime);
	_dinput.Tick(deltaTime);
}

void InputMaster::ClearAllClients()
{
	_clients.clear();
}

void InputMaster::ClearAllClients(std::string name)
{
	_clients[name].clear();
}

void InputMaster::ClearClient(const Emulator &client)
{
	std::string name = client.emu->GetName();
	_clients[name].remove(client);
}

void InputMaster::RegisterInputs(std::list<EmulatorInput_t> inputs, std::string name)
{
	std::list<EmulatorInput_t>::iterator it;
	for (it = inputs.begin(); it != inputs.end(); ++it)
	{
		Options::GetSingleton().AddKeyBinding(name, *it);
	}
}

void InputMaster::RegisterClient(const Emulator &client)
{
	std::string name = client.emu->GetName();
	_clients[name].push_back(client);
}

bool InputMaster::SendKey(std::string name, int key, int pressed)
{
	bool sent = false;
	std::map<std::string, std::list<Emulator>>::iterator client = _clients.find(name);
	if(client == _clients.end())
		return sent;

	std::list<Emulator>::iterator it;
	for (it = client->second.begin(); it != client->second.end(); ++it)
	{
		it->emu->Input(it->handle, key, pressed);
		sent = true;
	}

	return sent;	
}

void InputMaster::OnKeyboard(wxKeyEvent &evt)
{
	bool skip = true;
	Log(Debug, "Key: %d, Scancode: %d, Modifiers: %d", 
		evt.GetKeyCode(), evt.GetRawKeyCode(), evt.GetModifiers());
	
	std::map<std::string, std::list<Emulator>>::iterator it;
	for (it = _clients.begin(); it != _clients.end(); ++it)
	{
		const EmulatorInput_t *keyInfo = Options::GetSingleton().GetKeyBindingInfo(it->first, evt.GetKeyCode());
		if (keyInfo != NULL)
		{
			if (SendKey(it->first, keyInfo->key, evt.GetEventType() == wxEVT_KEY_DOWN) &&
				!(keyInfo->flags & EMU_INPUT_PASS)) {
				skip = false;
			}
		}
	}

	evt.Skip(skip);
}

void InputMaster::OnJoystick(JoystickEvent &evt)
{
	std::string typeStr;
	wxEventType type = evt.GetEventType();
	if (type == JOYSTICK_BUTTON_DOWN_EVENT) {
		typeStr = "Button down";
	}
	else if (type == JOYSTICK_BUTTON_UP_EVENT) {
		typeStr = "Button up";
	}
	else if (type == JOYSTICK_AXES_EVENT) {
		typeStr = "Joystick move";
	}
	else if (type == JOYSTICK_HAT_EVENT) {
		typeStr = "Joystick hat event";
	}
	else {
		typeStr = "Unknown";
	}
	Log(Debug, "Joystick event '%s' 0x%08x, %g", typeStr.c_str(), evt.GetID(), evt.GetValue());
}
