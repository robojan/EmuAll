#ifndef INPUT_H_
#define INPUT_H_

#include <wx/event.h>

enum class InputSource {
	Keyboard = 0,
	Mouse,
	XInput,
	DInput,
};

#define MAKE_JOYSTICK_ID(source, device, input) (((source) << 24) | ((device) << 16) | (input))
#define JOYSTICK_ELEMENT(id) ((id) & 0xFFFF)
#define JOYSTICK_DEVICE(id) (((id) >> 16) & 0xFF)
#define JOYSTICK_SOURCE(id) (((id) >> 24) & 0xFF)

class JoystickEvent;

wxDECLARE_EVENT(JOYSTICK_BUTTON_DOWN_EVENT, JoystickEvent);
wxDECLARE_EVENT(JOYSTICK_BUTTON_UP_EVENT, JoystickEvent);
wxDECLARE_EVENT(JOYSTICK_AXES_EVENT, JoystickEvent);
wxDECLARE_EVENT(JOYSTICK_HAT_EVENT, JoystickEvent);

typedef void (wxEvtHandler::*JoystickEventHandler)(JoystickEvent &evt);
#define JoystickEventHandler(func) wxEVENT_HANDLER_CAST(JoystickEventHandler, func)

class JoystickEvent : public wxEvent {
public:
	JoystickEvent(wxEventType type, int id, float value);
	JoystickEvent(const JoystickEvent &event);

	wxEvent *Clone() const;

	int GetID() const;
	float GetValue() const;

private: 
	int _id;
	float _value;
};

#endif