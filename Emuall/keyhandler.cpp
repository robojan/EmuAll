

#include "keyhandler.h"
#include "util/memDbg.h"
#include "util/log.h"

InputMaster::InputMaster(Options *options) :
	mOptions(options)
{
}

InputMaster::~InputMaster()
{

}

void InputMaster::ClearAllClients()
{
	mClients.clear();
}

void InputMaster::ClearAllClients(std::string name)
{
	mClients[name].clear();
}

void InputMaster::ClearClient(const Emulator &client)
{
	std::string name = client.emu->GetName();
	mClients[name].remove(client);
}

void InputMaster::RegisterInputs(std::list<EmulatorInput_t> inputs, std::string name)
{
	std::list<EmulatorInput_t>::iterator it;
	for (it = inputs.begin(); it != inputs.end(); ++it)
	{
		mOptions->AddKeyBinding(name, *it);
	}
}

void InputMaster::RegisterClient(const Emulator &client)
{
	std::string name = client.emu->GetName();
	mClients[name].push_back(client);
}

bool InputMaster::SendKey(std::string name, int key, int pressed)
{
	bool sent = false;
	std::map<std::string, std::list<Emulator>>::iterator client = mClients.find(name);
	if(client == mClients.end())
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
	for (it = mClients.begin(); it != mClients.end(); ++it)
	{
		const EmulatorInput_t *keyInfo = mOptions->GetKeyBindingInfo(it->first, evt.GetKeyCode());
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