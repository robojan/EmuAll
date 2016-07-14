#ifndef _INPUTOPTIONSFRAME_H_
#define _INPUTOPTIONSFRAME_H_

#include <wx/wx.h>
#include <wx/notebook.h>
#include "../input/inputMaster.h"
#include "../input/input.h"
#include "keyBindBox.h"

class InputOptionsFrame : public wxFrame
{
	DECLARE_CLASS(InputOptionsFrame);
	DECLARE_EVENT_TABLE();

public:
	InputOptionsFrame(wxFrame *parent, InputMaster *inputMaster);
	~InputOptionsFrame();

private:
	void UpdateBindingBoxes(const std::string &name);
	void OnClose(wxCloseEvent &evt);
	void OnShow(wxShowEvent &evt);
	void OnPageChanging(wxNotebookEvent &evt);
	void OnBindingClick(wxMouseEvent &evt);
	void OnKeyboardInput(wxKeyEvent &evt);
	void OnJoystickInput(JoystickEvent &evt);
	void OnFocusLost(wxFocusEvent &evt);

	void RestoreBinding();

	std::map<std::string, std::map<int, KeyBindBox *[2]>> _bindingBoxes;
	wxNotebook *_notebook;
	KeyBindBox *_activeRebind;
	InputMaster *_inputMaster;
};

#endif