


#include "InputOptionsFrame.h"
#include "../util/memDbg.h"
#include "keyBindBox.h"
#include <math.h>

IMPLEMENT_CLASS(InputOptionsFrame, wxFrame)
BEGIN_EVENT_TABLE(InputOptionsFrame, wxFrame)
	EVT_CLOSE(InputOptionsFrame::OnClose)
	EVT_SHOW(InputOptionsFrame::OnShow)
	EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY,InputOptionsFrame::OnPageChanging)
END_EVENT_TABLE()


InputOptionsFrame::InputOptionsFrame(wxFrame *parent, InputMaster *inputMaster) :
	wxFrame(parent, wxID_ANY, _("Input options")), _inputMaster(inputMaster)
{
	_notebook = new wxNotebook(this, wxID_ANY);
	_activeRebind = NULL;
	_inputMaster->Bind(JOYSTICK_AXES_EVENT, &InputOptionsFrame::OnJoystickInput, this, wxID_ANY, wxID_ANY, NULL);
	_inputMaster->Bind(JOYSTICK_BUTTON_DOWN_EVENT, &InputOptionsFrame::OnJoystickInput, this, wxID_ANY, wxID_ANY, NULL);
	_inputMaster->Bind(JOYSTICK_BUTTON_UP_EVENT, &InputOptionsFrame::OnJoystickInput, this, wxID_ANY, wxID_ANY, NULL);
	_inputMaster->Bind(JOYSTICK_HAT_EVENT, &InputOptionsFrame::OnJoystickInput, this, wxID_ANY, wxID_ANY, NULL);
}

InputOptionsFrame::~InputOptionsFrame()
{

}

void InputOptionsFrame::OnShow(wxShowEvent &evt)
{
	if (!evt.IsShown())
		return;
	if (_notebook->GetPageCount() >0 && _notebook->GetPageText(0) == _("No emulators"))
		_notebook->DeletePage(0);
	Options &options = Options::GetSingleton();
	if (options._keyBindings.size() == 0)
	{
		// Add empty page
		wxPanel *emptyPage = new wxPanel(_notebook);
		wxBoxSizer *sizer1 = new wxBoxSizer(wxVERTICAL);
		sizer1->AddStretchSpacer();
		wxStaticText *label1 = new wxStaticText(emptyPage, wxID_ANY, _("There are no emulators loaded!"));
		sizer1->Add(label1, 1, wxEXPAND|wxALIGN_CENTER,0);
		sizer1->AddStretchSpacer();
		emptyPage->SetSizerAndFit(sizer1);
		_notebook->InsertPage(0, emptyPage,_("No emulators"), true);
	}
	else {
		// Add a new page for each emulator
		for (auto &it : options._keyBindings)
		{
			bool pageExists = false;
			for (size_t i = 0; i < _notebook->GetPageCount(); i++)
			{
				if (_notebook->GetPageText(i) == wxString(it.first))
				{
					pageExists = true;
					break;
				}
			}

			if (pageExists)
				continue;
			// Add page
			wxScrolledWindow *emuPage = new wxScrolledWindow(_notebook);
			wxBoxSizer *sizerV1 = new wxBoxSizer(wxVERTICAL);
			wxStaticText *labelTitle = new wxStaticText(emuPage, wxID_ANY, wxString(it.first).Append(" Input options"));
			wxStaticText *cancelText = new wxStaticText(emuPage, wxID_ANY, _("Press escape to cancel."));
			wxFlexGridSizer *fgsizer1 = new  wxFlexGridSizer(3,5,10);
			fgsizer1->SetFlexibleDirection(wxHORIZONTAL);
			wxStaticText *primaryKeyLabel = new wxStaticText(emuPage, wxID_ANY, _("Primary"));
			wxStaticText *secondaryKeyLabel = new wxStaticText(emuPage, wxID_ANY, _("Secondary"));
			fgsizer1->AddStretchSpacer();
			fgsizer1->Add(primaryKeyLabel);
			fgsizer1->Add(secondaryKeyLabel);
				
			for (auto &binding : it.second)
			{
				wxStaticText *labelKey = new wxStaticText(emuPage, wxID_ANY, wxString(binding.name));
				KeyBindBox *textBoxKeyPrimary = new KeyBindBox(it.first, binding.id, 0, emuPage, wxID_ANY, wxDefaultPosition, wxDefaultSize);
				textBoxKeyPrimary->SetLabelText(_inputMaster->GetKeyName(binding.primaryKey));
				textBoxKeyPrimary->Bind(wxEVT_LEFT_UP, &InputOptionsFrame::OnBindingClick, this, wxID_ANY, wxID_ANY, NULL);
				textBoxKeyPrimary->Bind(wxEVT_KILL_FOCUS, &InputOptionsFrame::OnFocusLost, this, wxID_ANY, wxID_ANY, NULL);
				KeyBindBox *textBoxKeySecondary = new KeyBindBox(it.first, binding.id, 1, emuPage, wxID_ANY, wxDefaultPosition, wxDefaultSize);
				textBoxKeySecondary->SetLabelText(_inputMaster->GetKeyName(binding.secondaryKey));
				textBoxKeySecondary->Bind(wxEVT_LEFT_UP, &InputOptionsFrame::OnBindingClick, this, wxID_ANY, wxID_ANY, NULL);
				textBoxKeySecondary->Bind(wxEVT_KILL_FOCUS, &InputOptionsFrame::OnFocusLost, this, wxID_ANY, wxID_ANY, NULL);

				_bindingBoxes[it.first][binding.id][0] = textBoxKeyPrimary;
				_bindingBoxes[it.first][binding.id][1] = textBoxKeySecondary;

				fgsizer1->Add(labelKey);
				fgsizer1->Add(textBoxKeyPrimary);
				fgsizer1->Add(textBoxKeySecondary);
			}
			sizerV1->Add(labelTitle, 0, wxALIGN_CENTER_HORIZONTAL, 20);
			sizerV1->Add(cancelText, 0, wxALIGN_CENTER_HORIZONTAL, 2);
			sizerV1->Add(fgsizer1, 0, wxALIGN_CENTER_HORIZONTAL, 0);
			emuPage->SetSizerAndFit(sizerV1);
			emuPage->SetScrollRate(0, 10);
			emuPage->ShowScrollbars(wxSHOW_SB_DEFAULT, wxSHOW_SB_ALWAYS);
			_notebook->InsertPage(0, emuPage, wxString(it.first),true);
		}
	}
}

void InputOptionsFrame::RestoreBinding()
{
	if (_activeRebind == nullptr) {
		return;
	}
	Options &options = Options::GetSingleton();
	auto &bindings = options._keyBindings.find(_activeRebind->GetName());
	if (bindings == options._keyBindings.end()) {
		return;
	}
	int keyCode = -1;
	int id = _activeRebind->GetKey();
	for (auto &binding : bindings->second) {
		if (binding.id == id) {
			switch (_activeRebind->GetIdx()) {
			case 0:
				keyCode = binding.primaryKey;
				break;
			case 1:
				keyCode = binding.secondaryKey;
				break;
			}
			break;
		}
	}
	_activeRebind->SetLabelText(_inputMaster->GetKeyName(keyCode));
	_activeRebind = NULL;
}

void InputOptionsFrame::OnBindingClick(wxMouseEvent &evt)
{
	if (_activeRebind != NULL)
	{
		RestoreBinding();
	}

	_activeRebind = (KeyBindBox *) evt.GetEventObject();

	_activeRebind->SetLabelText(_("-Press any key-"));
	_activeRebind->Bind(wxEVT_KEY_UP, &InputOptionsFrame::OnKeyboardInput, this, wxID_ANY, wxID_ANY, NULL);

	evt.Skip();
}

void InputOptionsFrame::OnKeyboardInput(wxKeyEvent &evt)
{
	int keyCode = evt.GetKeyCode();
	if (_activeRebind == NULL || keyCode == 0)
		return; // How the hell did we get here?

	Options &options = Options::GetSingleton();
	int keycode = evt.GetKeyCode();
	if (keycode == 27) { // escape
		keycode = -1;
	}
	options.RebindKey(_activeRebind->GetName(), _activeRebind->GetKey(), keycode, _activeRebind->GetIdx());
	_activeRebind->Unbind(wxEVT_KEY_UP, &InputOptionsFrame::OnKeyboardInput,this, wxID_ANY, wxID_ANY, NULL);
	UpdateBindingBoxes(_activeRebind->GetName());
	_activeRebind = NULL;
}


void InputOptionsFrame::OnJoystickInput(JoystickEvent &evt)
{
	if (_activeRebind == NULL) {
		evt.Skip();
		return;
	}
	Options &options = Options::GetSingleton();
	wxEventType type = evt.GetEventType();
	if (type == JOYSTICK_AXES_EVENT) {
		float value = evt.GetValue();
		if (value < -0.5) {
			// Negative axis
			int flags = JOYSTICK_FLAG_NEGATIVE;
			int id = evt.GetID() | (flags << 28);
			options.RebindKey(_activeRebind->GetName(), _activeRebind->GetKey(), id, _activeRebind->GetIdx());
			UpdateBindingBoxes(_activeRebind->GetName());
			_activeRebind = NULL;
		}
		else if (value > 0.5) {
			// Positive axis
			int flags = 0;
			int id = evt.GetID() | (flags << 28);
			options.RebindKey(_activeRebind->GetName(), _activeRebind->GetKey(), id, _activeRebind->GetIdx());
			UpdateBindingBoxes(_activeRebind->GetName());
			_activeRebind = NULL;
		}
	}
	else if (type == JOYSTICK_BUTTON_DOWN_EVENT || type == JOYSTICK_BUTTON_UP_EVENT) {
		options.RebindKey(_activeRebind->GetName(), _activeRebind->GetKey(), evt.GetID(), _activeRebind->GetIdx());
		UpdateBindingBoxes(_activeRebind->GetName());
		_activeRebind = NULL;
	}
	else if (type == JOYSTICK_HAT_EVENT) {

	}
	evt.Skip();
}

void InputOptionsFrame::OnFocusLost(wxFocusEvent &evt)
{
	if (_activeRebind != NULL)
	{
		RestoreBinding();
	}
	evt.Skip();
}

void InputOptionsFrame::OnPageChanging(wxNotebookEvent &evt)
{
	if (_activeRebind != NULL)
	{
		RestoreBinding();
	}
}

void InputOptionsFrame::UpdateBindingBoxes(const std::string &name)
{
	Options &options = Options::GetSingleton();
	auto &keybindBoxesArray = _bindingBoxes.find(name);
	if (keybindBoxesArray == _bindingBoxes.end()) {
		return;
	}
	auto &bindings = options._keyBindings.find(name);
	if (bindings != options._keyBindings.end()) {
		for (auto &binding : bindings->second) {
			auto &keybindBoxes = keybindBoxesArray->second.find(binding.id);
			if (keybindBoxes == keybindBoxesArray->second.end()) {
				continue;
			}
			keybindBoxes->second[0]->SetLabelText(_inputMaster->GetKeyName(binding.primaryKey));
			keybindBoxes->second[1]->SetLabelText(_inputMaster->GetKeyName(binding.secondaryKey));
		}
	}
}

void InputOptionsFrame::OnClose(wxCloseEvent &evt)
{
	if (evt.CanVeto())
	{
		evt.Veto(true);
		Show(false);
		return;
	}
	Destroy();
}