#ifndef XINPUT_H_
#define XINPUT_H_

#include <wx/wx.h>

class XInputController {
public:
	enum class ObjectId {
		DPadUp = 0,
		DPadDown,
		DPadLeft,
		DPadRight,
		Start,
		Back,
		LeftThumb,
		RightThumb,
		LeftShoulder,
		RightShoulder,
		A = 12,
		B,
		X,
		Y,
		LeftTrigger,
		RightTrigger,
		LeftThumbX,
		LeftThumbY,
		RightThumbX,
		RightThumbY
	};
	XInputController(wxEvtHandler *parent, int id);
	~XInputController();

	bool Tick();
	bool IsConnected();

	void Rumble(float left, float right);

	static const char *GetButtonName(ObjectId button);
private:
	struct ControllerState {
		unsigned short buttons;
		float leftTrigger;
		float rightTrigger;
		float leftStickX;
		float leftStickY;
		float rightStickX;
		float rightStickY;
	};

	void DispatchEvent(wxEventType type, ObjectId object, float value);
	void DispatchButtons(unsigned short newButtons);
	void CalculateState(struct ControllerState &out, unsigned short buttons, unsigned char leftTrigger,
		unsigned char rightTrigger, short leftStickX, short leftStickY, short rightStickX, short rightStickY);
	static float CalculateTriggerValue(unsigned char value, unsigned char deadzone);
	static void CalculateStickValue(float &outX, float &outY, short inX, short inY, short deadZone);

	wxEvtHandler *_parent;
	int _id;

	int _lastPacket;
	struct ControllerState _lastState;

};

class XInput {
public:
	XInput(wxEvtHandler *parent);
	~XInput();

	void RefreshDevices();
	void Tick(unsigned int deltaTime);

	void Rumble(int id, float left, float right);
private:
	wxEvtHandler *_parent;
	unsigned int _refreshTimer;
	bool _controllerPresent[4];
	XInputController _controllers[4];
};

#endif
