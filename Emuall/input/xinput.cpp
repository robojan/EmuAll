
#include "../util/log.h"
#include "xinput.h"
#include "input.h"

#include <Windows.h>
#include <Xinput.h>
#include <math.h>
#include <climits>

XInput::~XInput()
{
	for (int i = 0; i < 4; i++) {
		Rumble(i, 0, 0);
	}
}

void XInput::RefreshDevices()
{
	for (int i = 0; i < XUSER_MAX_COUNT; i++) {
		bool connected = _controllers[i].IsConnected();
		if (connected != _controllerPresent[i]) {
			// Send event
			Log(Debug, "Controller %d %s", i, connected ? "connected" : "disconnected");
		}
		_controllerPresent[i] = connected;
	}
}

void XInput::Tick(unsigned int deltaTime)
{
	_refreshTimer += deltaTime;
	if (_refreshTimer > 3000) {
		_refreshTimer = 0;
		RefreshDevices();
	}

	for (int i = 0; i < XUSER_MAX_COUNT; i++) {
		if (_controllerPresent[i]) {
			if (!_controllers[i].Tick()) {
				_controllerPresent[i] = false;
				// TODO send event
				Log(Debug, "Controller %d disconnected", i);
			}
		}
	}
}

void XInput::Rumble(int id, float left, float right)
{
	if (id < 0 || id >= 4 || !_controllerPresent[id]) return;
	
	_controllers[id].Rumble(left, right);
}

wxString XInput::GetKeyName(int device, int object)
{
	return wxString::Format("XBOX %d %s", device, XInputController::GetButtonName((XInputController::ObjectId)object));
}

bool XInput::IsAxis(int device, int object)
{
	return object >= 16;
}

XInput::XInput(wxEvtHandler *parent) :
	_parent(parent), _refreshTimer(5000), _controllerPresent{false, false, false, false}, 
	_controllers{ {parent, 0 },{ parent, 1 },{ parent, 2 },{ parent, 3 } }
{

}


XInputController::XInputController(wxEvtHandler *parent, int id) :
	_parent(parent), _id(id), _lastPacket(0)
{
	ZeroMemory(&_lastState, sizeof(struct ControllerState));
}

XInputController::~XInputController()
{

}

bool XInputController::Tick()
{
	// get state
	DWORD dwResult;
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	dwResult = XInputGetState(_id, &state);

	if (dwResult != ERROR_SUCCESS) {
		return false;
	}

	if (state.dwPacketNumber != _lastPacket) {
		// New packet received
		struct XInputController::ControllerState newState;
		CalculateState(newState, state.Gamepad.wButtons, state.Gamepad.bLeftTrigger,
			state.Gamepad.bRightTrigger, state.Gamepad.sThumbLX, state.Gamepad.sThumbLY,
			state.Gamepad.sThumbRX, state.Gamepad.sThumbRY);

		DispatchButtons(newState.buttons);
		if (newState.leftTrigger != _lastState.leftTrigger) {
			DispatchEvent(JOYSTICK_AXES_EVENT, ObjectId::LeftTrigger, newState.leftTrigger);
		}
		if (newState.rightTrigger != _lastState.rightTrigger) {
			DispatchEvent(JOYSTICK_AXES_EVENT, ObjectId::RightTrigger, newState.rightTrigger);
		}
		if (newState.leftStickX != _lastState.leftStickX) {
			DispatchEvent(JOYSTICK_AXES_EVENT, ObjectId::LeftThumbX, newState.leftStickX);
		}
		if (newState.leftStickY != _lastState.leftStickY) {
			DispatchEvent(JOYSTICK_AXES_EVENT, ObjectId::LeftThumbY, newState.leftStickY);
		}
		if (newState.rightStickX != _lastState.rightStickX) {
			DispatchEvent(JOYSTICK_AXES_EVENT, ObjectId::RightThumbX, newState.rightStickX);
		}
		if (newState.rightStickY != _lastState.rightStickY) {
			DispatchEvent(JOYSTICK_AXES_EVENT, ObjectId::RightThumbY, newState.rightStickY);
		}

		_lastState = newState;
	}

	return true;
}

bool XInputController::IsConnected()
{
	DWORD dwResult;
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	dwResult = XInputGetState(_id, &state);
	return dwResult == ERROR_SUCCESS;
}

void XInputController::Rumble(float left, float right)
{
	Log(Debug, "Controller %d rumble %g, %g", _id, left, right);
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	if (left < 0) left = 0;
	if (right < 0) right = 0;
	if (left > 1) left = 1;
	if (right > 1) right = 1;
	vibration.wLeftMotorSpeed = WORD(left * USHRT_MAX);
	vibration.wLeftMotorSpeed = WORD(right * USHRT_MAX);
	XInputSetState(_id, &vibration);
}

const char * XInputController::GetButtonName(ObjectId button)
{
	switch (button) {
	case ObjectId::DPadUp: return "D-Pad up";
	case ObjectId::DPadDown: return "D-Pad down";
	case ObjectId::DPadLeft: return "D-Pad left";
	case ObjectId::DPadRight: return "D-Pad right";
	case ObjectId::Start: return "Start";
	case ObjectId::Back: return "Back";
	case ObjectId::LeftThumb: return "Left thumb";
	case ObjectId::RightThumb: return "Right thumb";
	case ObjectId::LeftShoulder: return "Left shoulder";
	case ObjectId::RightShoulder: return "Right shoulder";
	case ObjectId::A: return "A";
	case ObjectId::B: return "B";
	case ObjectId::X: return "X";
	case ObjectId::Y: return "Y";
	case ObjectId::LeftTrigger: return "Left trigger";
	case ObjectId::RightTrigger: return "Right trigger";
	case ObjectId::LeftThumbX: return "Left thumb X";
	case ObjectId::LeftThumbY: return "Left thumb Y";
	case ObjectId::RightThumbX: return "Right thumb X";
	case ObjectId::RightThumbY: return "Right thumb Y";
	default: return "Unknown";
	}
}

void XInputController::DispatchEvent(wxEventType type, ObjectId object, float value)
{
	int id = MAKE_JOYSTICK_ID((int)InputSource::XInput, _id, (int)object);
	JoystickEvent event(type, id, value);
	_parent->ProcessEvent(event);
}

void XInputController::DispatchButtons(unsigned short newButtons)
{
	WORD buttonsChanged = newButtons ^ _lastState.buttons;
	if (buttonsChanged == 0) {
		return;
	}
	for (int i = 0; i < 16; i++) {
		if (buttonsChanged & (1 << i)) {
			bool pressed = (newButtons & (1 << i)) != 0;
			if (pressed) {
				DispatchEvent(JOYSTICK_BUTTON_DOWN_EVENT, (ObjectId)i, 1.0);
			}
			else {
				DispatchEvent(JOYSTICK_BUTTON_UP_EVENT, (ObjectId)i, 1.0);
			}
		}
	}
}

void XInputController::CalculateState(struct ControllerState &out, unsigned short buttons,
	unsigned char leftTrigger, unsigned char rightTrigger, short leftStickX, 
	short leftStickY, short rightStickX, short rightStickY)
{
	out.buttons = buttons;
	out.leftTrigger = CalculateTriggerValue(leftTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	out.rightTrigger = CalculateTriggerValue(rightTrigger, XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
	CalculateStickValue(out.leftStickX, out.leftStickY, leftStickX, leftStickY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
	CalculateStickValue(out.rightStickX, out.rightStickY, rightStickX, rightStickY, XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE);
}

float XInputController::CalculateTriggerValue(unsigned char value, unsigned char deadzone)
{
	if (value < deadzone) {
		return 0.0f;
	}
	float result = float(value - deadzone);
	return result / (255 - deadzone);
}

void XInputController::CalculateStickValue(float &outX, float &outY, short inX, short inY, short deadZone)
{
	float x = float(inX);
	float y = float(inY);
	float magnitude = sqrtf(x*x + y*y);
	float angle = atan2f(y, x);
	if (magnitude < deadZone) {
		outX = 0.0;
		outY = 0.0;
		return;
	}
	magnitude = (magnitude - deadZone) / (SHRT_MAX - deadZone);
	if (magnitude > 1.0) magnitude = 1.0;
	outX = magnitude * cosf(angle);
	outY = magnitude * sinf(angle);
}
