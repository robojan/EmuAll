

#include "inputMaster.h"
#include "../util/memDbg.h"
#include "../util/log.h"
#include "../util/Options.h"

InputMaster::InputMaster(wxWindow *parent) :
	_dinput(this, (HWND)parent->GetHWND()), _xinput(this)
{
	_bindings.clear();
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


void InputMaster::RegisterClient(Emulator *client)
{
	_emu = client;
	_bindings.clear();
	if (client != nullptr && client->emu != nullptr) {
		auto &bindings = Options::GetSingleton()._keyBindings[client->emu->GetName()];
		for (auto &binding : bindings) {
			if (binding.primaryKey != -1) {
				_bindings[binding.primaryKey] = binding;
			}
			if (binding.secondaryKey != -1) {
				_bindings[binding.secondaryKey] = binding;
			}
		}
	}
	
}

bool InputMaster::SendKey(int id, int pressed)
{
	if (_emu == nullptr) {
		return false;
	}

	_emu->emu->Input(_emu->handle, id, pressed);

	return true;	
}


wxString InputMaster::GetKeyboardKeyName(int id)
{
	if ((id > 32 && id < 193 && id != 127) || (id > 212 && id < 256))
	{
		// Normal ASCII keys
		return wxString(char(id));
	}

	switch (id)
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

void InputMaster::OnKeyboard(wxKeyEvent &evt)
{
	bool skip = true;
	Log(Debug, "Key: %d, Scancode: %d, Modifiers: %d", 
		evt.GetKeyCode(), evt.GetRawKeyCode(), evt.GetModifiers());

	int id = evt.GetKeyCode();
	bool pressed = evt.GetEventType() == wxEVT_KEY_DOWN;
	auto &binding = _bindings.find(id);
	if (binding != _bindings.end()) {
		if ((binding->second.flags & EMU_INPUT_ANALOG) != 0) {
			Log(Warn, "TODO: send analog inputs");
			skip = true;
		}
		else {
			if (SendKey(binding->second.id, pressed) &&
				(binding->second.flags & EMU_INPUT_PASS) == 0) {
				skip = false;
			}
		}
	}

	evt.Skip(skip);
}

void InputMaster::OnJoystick(JoystickEvent &evt)
{
	wxEventType type = evt.GetEventType();
	if (type == JOYSTICK_BUTTON_DOWN_EVENT) {
		int id = evt.GetID();
		auto &binding = _bindings.find(id);
		if (binding != _bindings.end()) {
			if ((binding->second.flags & EMU_INPUT_ANALOG) != 0) {
				Log(Warn, "TODO: send analog inputs");
			}
			else {
				SendKey(binding->second.id, true);
			}
		}
	}
	else if (type == JOYSTICK_BUTTON_UP_EVENT) {
		int id = evt.GetID();
		auto &binding = _bindings.find(id);
		if (binding != _bindings.end()) {
			if ((binding->second.flags & EMU_INPUT_ANALOG) != 0) {
				Log(Warn, "TODO: send analog inputs");
			}
			else {
				SendKey(binding->second.id, false);
			}
		}
	}
	else if (type == JOYSTICK_AXES_EVENT) {
		float value = evt.GetValue();
		int id = evt.GetID();
		auto &bindingAnalog = _bindings.find(id);
		auto &bindingDigital = _bindings.find(id | (JOYSTICK_FLAG_NEGATIVE << 28));
		if (bindingAnalog != _bindings.end()) {
			if ((bindingAnalog->second.flags & EMU_INPUT_ANALOG) != 0) {
				Log(Warn, "TODO: send analog inputs");
			}
			else {
				SendKey(bindingAnalog->second.id, value > 0.5f);
			}
		}
		if (bindingDigital != _bindings.end()) {
			if ((bindingDigital->second.flags & EMU_INPUT_ANALOG) != 0) {
				Log(Warn, "TODO: send analog inputs");
			}
			else {
				SendKey(bindingDigital->second.id, value < -0.5f);
			}
		}
	}
	else if (type == JOYSTICK_HAT_EVENT) {
	}
	evt.Skip();
}

wxString InputMaster::GetKeyName(int id)
{
	if (id == -1) return "Unassigned";
	InputSource source = (InputSource)JOYSTICK_SOURCE(id);
	int device = JOYSTICK_DEVICE(id);
	int object = JOYSTICK_ELEMENT(id);
	int flags = JOYSTICK_FLAGS(id);
	bool negativeAxis = (flags & JOYSTICK_FLAG_NEGATIVE) != 0;
	switch (source) {
	case InputSource::Keyboard: return GetKeyboardKeyName(object);
	case InputSource::Mouse: return "Unknown Mouse Event";
	case InputSource::XInput: {
		bool axis = _xinput.IsAxis(device, object);
		if (axis) {
			return _xinput.GetKeyName(device, object).Append(negativeAxis ? "-" : "+");
		}
		else {
			return _xinput.GetKeyName(device, object);
		}
	}
	case InputSource::DInput: {
		bool axis = _dinput.IsAxis(device, object);
		if (axis) {
			return _dinput.GetKeyName(device, object).Append(negativeAxis ? "-" : "+");
		}
		else {
			return _dinput.GetKeyName(device, object);
		}
	}
	default: return "Unknown";
	}
}
