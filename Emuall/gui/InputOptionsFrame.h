#ifndef _INPUTOPTIONSFRAME_H_
#define _INPUTOPTIONSFRAME_H_

#include <wx/wx.h>
#include <wx/notebook.h>
#include "../util/Options.h"
#include "../keyhandler.h"
#include "keyBindBox.h"

class InputOptionsFrame : public wxFrame
{
	DECLARE_CLASS(InputOptionsFrame);
	DECLARE_EVENT_TABLE();

public:
	InputOptionsFrame(wxFrame *parent, Options *options);
	~InputOptionsFrame();

	std::string KeyToString(int key);

private:
	void OnClose(wxCloseEvent &evt);
	void OnShow(wxShowEvent &evt);
	void OnPageChanging(wxNotebookEvent &evt);
	void OnBindingClick(wxMouseEvent &evt);
	void InputOptionsFrame::OnKeyboardInput(wxKeyEvent &evt);

	void RestoreBinding();

	std::map<std::string, std::map<int, KeyBindBox *>> _bindingBoxes;
	Options *_options;
	wxNotebook *_notebook;
	KeyBindBox *_activeRebind;
};

#endif