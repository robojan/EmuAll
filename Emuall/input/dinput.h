#ifndef DINPUT_H_
#define DINPUT_H_

#define DIRECTINPUT_VERSION 0x0800
#include <initguid.h>
#include <Windows.h>
#include <dinput.h>
#include <vector>
#include <string>
#include <wx/event.h>

class DInput;

class DInputDevice {
public:
	struct ObjectInfo {
		int id;
		std::wstring name;
		int min;
		int max;
		int type;
	};
	struct DeviceObjectInfo{
		std::vector<ObjectInfo> buttons;
		std::vector<ObjectInfo> axes;
		std::vector<ObjectInfo> povs;
	};
	DInputDevice(DInput *dinput, LPDIRECTINPUT8 di, DIDEVICEINSTANCE &device, HWND hwnd);
	~DInputDevice();

	void Tick();

	void Acquire();
	void Unacquire();
	void SetCooperativeLevel(bool foreground, bool exclusive);
	void SetAxisRange(int id, int min, int max);
	void GetAxisRange(int id, int &min, int &max);

	int GetDeviceId() const;

	const DeviceObjectInfo &GetObjectInfoList();
private:
	struct DeviceState {
		std::vector<bool> buttons;
		std::vector<float> axes;
		std::vector<float> povs;
	};

	void GetState(struct DeviceState &state);
	void Poll();
	void GetCapabilities();
	void SetDataFormat();
	void SetBufferSize(int size);
	void EnumObjects();

	LPDIRECTINPUTDEVICE8 _did;
	GUID _instanceGuid;
	GUID _productGuid;
	std::wstring _name;
	std::wstring _instanceName;
	bool _acquired;
	HWND _hwnd;

	bool _attached;
	int _numAxes;
	int _numButtons;
	int _numPOVs;
	DeviceObjectInfo _infoList;
	int _dataPacketSize;
	unsigned char *_dataPacket;

	struct DeviceState _lastState;
	DInput *_dinput;
	int _deviceId;
};

class DInput {
public:
	~DInput();
	DInput(wxEvtHandler *handler, HWND hwnd);

	void Tick(unsigned int time);
	void EnumDevices();
	void DispatchEvent(wxEventType type, int deviceId, int id, float value);
	int GetGUIDId(const GUID &guidInstance);
private:
	LPDIRECTINPUT8 _di;

	std::vector<DIDEVICEINSTANCE> _devicesInfo;
	std::vector<DInputDevice> _devices;
	HWND _hwnd;
	wxEvtHandler *_evtHandler;
};

#endif
