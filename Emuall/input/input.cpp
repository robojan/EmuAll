
#include "input.h"
#include <wx/joystick.h>

wxDEFINE_EVENT(JOYSTICK_EVENT, JoystickEvent);
wxDEFINE_EVENT(JOYSTICK_BUTTON_DOWN_EVENT, JoystickEvent);
wxDEFINE_EVENT(JOYSTICK_BUTTON_UP_EVENT, JoystickEvent);
wxDEFINE_EVENT(JOYSTICK_AXES_EVENT, JoystickEvent);
wxDEFINE_EVENT(JOYSTICK_HAT_EVENT, JoystickEvent);

JoystickEvent::JoystickEvent(wxEventType type, int id, float value) :
	wxEvent(0, type), _id(id), _value(value)
{

}

JoystickEvent::JoystickEvent(const JoystickEvent &event) :
	wxEvent(event)
{

}

wxEvent * JoystickEvent::Clone() const
{
	return new JoystickEvent(*this);
}

int JoystickEvent::GetID() const
{
	return _id;
}

float JoystickEvent::GetValue() const
{
	return  _value;
}
