


#include "InputOptionsFrame.h"
#include "../util/memDbg.h"
#include "keyBindBox.h"

IMPLEMENT_CLASS(InputOptionsFrame, wxFrame)
BEGIN_EVENT_TABLE(InputOptionsFrame, wxFrame)
	EVT_CLOSE(InputOptionsFrame::OnClose)
	EVT_SHOW(InputOptionsFrame::OnShow)
	EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY,InputOptionsFrame::OnPageChanging)
END_EVENT_TABLE()


InputOptionsFrame::InputOptionsFrame(wxFrame *parent) :
	wxFrame(parent, wxID_ANY, _("Input options"))
{

	_notebook = new wxNotebook(this, wxID_ANY);
	_activeRebind = NULL;
}

InputOptionsFrame::~InputOptionsFrame()
{

}

std::string InputOptionsFrame::KeyToString(int key)
{
	if ((key > 32 && key < 193 && key != 127) || (key > 212 && key < 256))
	{
		// Normal ASCII keys
		return std::string(1, static_cast<char>(key));
	}

	switch (key)
	{
	case 8: return "Backspace";
	case 9: return "Tab";
	case 13: return "Return";
	case 27: return "Escape";
	case 32: return "Space";
	case 127: return "Delete";
	case 300: return "Start";
	case 301: return "Left button";
	case 302: return "Right button";
	case 303: return "Cancel";
	case 304: return "Middle button";
	case 305: return "Clear";
	case 306: return "Shift";
	case 307: return "Alt";
	case 308: return "Control";
	case 309: return "Menu";
	case 310: return "Break";
	case 311: return "Caps Lock";
	case 312: return "End";
	case 313: return "Home";
	case 314: return "Left";
	case 315: return "Up";
	case 316: return "Right";
	case 317: return "Down";
	case 318: return "Select";
	case 319: return "Print";
	case 320: return "Execute";
	case 321: return "Print screen";
	case 322: return "Insert";
	case 323: return "Help";
	case 324: return "NUM 0";
	case 325: return "NUM 1";
	case 326: return "NUM 2";
	case 327: return "NUM 3";
	case 328: return "NUM 4";
	case 329: return "NUM 5";
	case 330: return "NUM 6";
	case 331: return "NUM 7";
	case 332: return "NUM 8";
	case 333: return "NUM 9";
	case 334: return "Multiply";
	case 335: return "Add";
	case 336: return "Separator";
	case 337: return "subtract";
	case 338: return "Decimal";
	case 339: return "Divide"; 
	case 340: return "F1";
	case 341: return "F2";
	case 342: return "F3";
	case 343: return "F4";
	case 344: return "F5";
	case 345: return "F6";
	case 346: return "F7";
	case 347: return "F8";
	case 348: return "F9";
	case 349: return "F10";
	case 350: return "F11";
	case 351: return "F12";
	case 352: return "F13";
	case 353: return "F14";
	case 354: return "F15";
	case 355: return "F16";
	case 356: return "F17";
	case 357: return "F18";
	case 358: return "F19";
	case 359: return "F20";
	case 360: return "F21";
	case 361: return "F22";
	case 362: return "F23";
	case 363: return "F24";
	case 364: return "Num lock";
	case 365: return "Scroll lock";
	case 366: return "Page up";
	case 367: return "Page down";
	case 368: return "NUM Space";
	case 369: return "NUM Tab";
	case 370: return "NUM Enter";
	case 371: return "NUM F1";
	case 372: return "NUM F2";
	case 373: return "NUM F3";
	case 374: return "NUM F4";
	case 375: return "NUM Home";
	case 376: return "NUM Left";
	case 377: return "NUM Up";
	case 378: return "NUM Right";
	case 379: return "NUM Down";
	case 380: return "NUM Page up";
	case 381: return "NUM Page down";
	case 382: return "NUM End";
	case 383: return "NUM Begin";
	case 384: return "NUM Insert";
	case 385: return "NUM Delete";
	case 386: return "NUM =";
	case 387: return "NUM *";
	case 388: return "NUM +";
	case 389: return "NUM Separator";
	case 390: return "NUM -";
	case 391: return "NUM .";
	case 392: return "NUM /";
	case 393: return "Left Windows";
	case 394: return "Right Windows";
	case 395: return "Menu";
	case 193: return "Special 1";
	case 194: return "Special 2";
	case 195: return "Special 3";
	case 196: return "Special 4";
	case 197: return "Special 5";
	case 198: return "Special 6";
	case 199: return "Special 7";
	case 200: return "Special 8";
	case 201: return "Special 9";
	case 202: return "Special 10";
	case 203: return "Special 11";
	case 204: return "Special 12";
	case 205: return "Special 13";
	case 206: return "Special 14";
	case 207: return "Special 15";
	case 208: return "Special 16";
	case 209: return "Special 17";
	case 210: return "Special 18";
	case 211: return "Special 19";
	case 212: return "Special 20";
	}

	return "Unknown";
}

void InputOptionsFrame::OnShow(wxShowEvent &evt)
{
	if (!evt.IsShown())
		return;
	if (_notebook->GetPageCount() >0 && _notebook->GetPageText(0) == _("No emulators"))
		_notebook->DeletePage(0);
	Options &options = Options::GetInstance();
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
		std::map<std::string, std::map<int, EmulatorInput_t >>::iterator it;
		for (it = options._keyBindings.begin(); it != options._keyBindings.end(); ++it)
		{
			bool pageExists = false;
			for (size_t i = 0; i < _notebook->GetPageCount(); i++)
			{
				if (_notebook->GetPageText(i) == wxString(it->first))
				{
					pageExists = true;
					break;
				}
			}

			if (pageExists)
				continue;
			// Add page
			wxPanel *emuPage = new wxPanel(_notebook);
			wxBoxSizer *sizerV1 = new wxBoxSizer(wxVERTICAL);
			wxStaticText *labelTitle = new wxStaticText(emuPage, wxID_ANY, wxString(it->first).Append(" Input options"));
			wxFlexGridSizer *fgsizer1 = new  wxFlexGridSizer(4,5,10);
			fgsizer1->SetFlexibleDirection(wxHORIZONTAL);
			std::map<int, EmulatorInput_t>::iterator it2;
			for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
			{
				wxStaticText *labelKey = new wxStaticText(emuPage, wxID_ANY, wxString(it2->second.name));
				KeyBindBox *textBoxKey = new KeyBindBox(it->first, it2->second.key, emuPage, wxID_ANY, wxDefaultPosition, wxDefaultSize);
				textBoxKey->SetLabelText(KeyToString(options.GetInputKeyBinding(it->first, it2->second.key, 0)));
				textBoxKey->Bind(wxEVT_LEFT_UP, &InputOptionsFrame::OnBindingClick, this, wxID_ANY, wxID_ANY, NULL);
				_bindingBoxes[it->first][it2->second.key] = textBoxKey;
				fgsizer1->Add(labelKey);
				fgsizer1->Add(textBoxKey);
			}
			sizerV1->Add(labelTitle, 0, wxALIGN_CENTER_HORIZONTAL, 20);
			sizerV1->Add(fgsizer1, 0, wxALIGN_CENTER_HORIZONTAL, 0);
			emuPage->SetSizerAndFit(sizerV1);
			_notebook->InsertPage(0, emuPage, wxString(it->first),true);
		}
	}
}

void InputOptionsFrame::RestoreBinding()
{
	_activeRebind->SetLabelText(KeyToString(Options::GetInstance().GetInputKeyBinding(
		_activeRebind->GetName(), _activeRebind->GetKey(), 0)));
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
		return; // How the hell did we come here?

	Options &options = Options::GetInstance();
	options.RebindKey(_activeRebind->GetName(), _activeRebind->GetKey(), evt.GetKeyCode());
	_activeRebind->Unbind(wxEVT_KEY_UP, &InputOptionsFrame::OnKeyboardInput,this, wxID_ANY, wxID_ANY, NULL);
	// Update all boxes
	std::map<int, KeyBindBox *>::iterator it;
	for (it = _bindingBoxes[_activeRebind->GetName()].begin(); it != _bindingBoxes[_activeRebind->GetName()].end(); ++it)
	{
		it->second->SetLabelText(KeyToString(options.GetInputKeyBinding(it->second->GetName(), it->second->GetKey(), 0)));
	}
	_activeRebind = NULL;
}

void InputOptionsFrame::OnPageChanging(wxNotebookEvent &evt)
{
	if (_activeRebind != NULL)
	{
		_activeRebind = NULL;
		// Update all boxes
		std::map<int, KeyBindBox *>::iterator it;
		for (it = _bindingBoxes[_activeRebind->GetName()].begin(); it != _bindingBoxes[_activeRebind->GetName()].end(); ++it)
		{
			it->second->SetLabelText(KeyToString(Options::GetInstance().GetInputKeyBinding(
				it->second->GetName(), it->second->GetKey(), 0)));
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