#ifndef _KEYHANDLER_H
#define _KEYHANDLER_H

#include "Emulator\Emulator.h"
#include <map>
#include <list>
#include <vector>
#include <string>
#include <wx/wx.h>

class InputMaster : public wxEvtHandler
{
public:
	InputMaster();
	~InputMaster();

	void ClearAllClients();
	void ClearAllClients(std::string name);
	void ClearClient(const Emulator &client);

	void RegisterInputs(std::list<EmulatorInput_t> inputs, std::string name);
	void RegisterClient(const Emulator &client);

	void OnKeyboard(wxKeyEvent &evt);
private:
	bool SendKey(std::string name, int key, int pressed);

	std::map<std::string, std::list<Emulator>> _clients;
};

#endif