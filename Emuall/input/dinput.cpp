
#include "dinput.h"
#include "../util/log.h"
#include "../util/Options.h"
#include "input.h"

#include <wbemidl.h>
#include <oleauto.h>
#include <sstream>
#include <cassert>
#include <crtdbg.h>

//-----------------------------------------------------------------------------
// Enum each PNP device using WMI and check each device ID to see if it contains 
// "IG_" (ex. "VID_045E&PID_028E&IG_00").  If it does, then it's an XInput device
// Unfortunately this information can not be found by just using DirectInput 
//-----------------------------------------------------------------------------
BOOL IsXInputDevice(const GUID* pGuidProductFromDirectInput)
{
	IWbemLocator*           pIWbemLocator = NULL;
	IEnumWbemClassObject*   pEnumDevices = NULL;
	IWbemClassObject*       pDevices[20] = { 0 };
	IWbemServices*          pIWbemServices = NULL;
	BSTR                    bstrNamespace = NULL;
	BSTR                    bstrDeviceID = NULL;
	BSTR                    bstrClassName = NULL;
	DWORD                   uReturned = 0;
	bool                    bIsXinputDevice = false;
	UINT                    iDevice = 0;
	VARIANT                 var;
	HRESULT                 hr;

	// CoInit if needed
	hr = CoInitialize(NULL);
	bool bCleanupCOM = SUCCEEDED(hr);

	// Create WMI
	hr = CoCreateInstance(__uuidof(WbemLocator),
		NULL,
		CLSCTX_INPROC_SERVER,
		__uuidof(IWbemLocator),
		(LPVOID*)&pIWbemLocator);
	if (FAILED(hr) || pIWbemLocator == NULL)
		goto LCleanup;

	bstrNamespace = SysAllocString(L"\\\\.\\root\\cimv2");if (bstrNamespace == NULL) goto LCleanup;
	bstrClassName = SysAllocString(L"Win32_PNPEntity");   if (bstrClassName == NULL) goto LCleanup;
	bstrDeviceID = SysAllocString(L"DeviceID");          if (bstrDeviceID == NULL)  goto LCleanup;

	// Connect to WMI 
	hr = pIWbemLocator->ConnectServer(bstrNamespace, NULL, NULL, 0L,
		0L, NULL, NULL, &pIWbemServices);
	if (FAILED(hr) || pIWbemServices == NULL)
		goto LCleanup;

	// Switch security level to IMPERSONATE. 
	CoSetProxyBlanket(pIWbemServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
		RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);

	hr = pIWbemServices->CreateInstanceEnum(bstrClassName, 0, NULL, &pEnumDevices);
	if (FAILED(hr) || pEnumDevices == NULL)
		goto LCleanup;

	// Loop over all devices
	for (;; )
	{
		// Get 20 at a time
		hr = pEnumDevices->Next(10000, 20, pDevices, &uReturned);
		if (FAILED(hr))
			goto LCleanup;
		if (uReturned == 0)
			break;

		for (iDevice = 0; iDevice < uReturned; iDevice++)
		{
			// For each device, get its device ID
			hr = pDevices[iDevice]->Get(bstrDeviceID, 0L, &var, NULL, NULL);
			if (SUCCEEDED(hr) && var.vt == VT_BSTR && var.bstrVal != NULL)
			{
				// Check if the device ID contains "IG_".  If it does, then it's an XInput device
				// This information can not be found from DirectInput 
				if (wcsstr(var.bstrVal, L"IG_"))
				{
					// If it does, then get the VID/PID from var.bstrVal
					DWORD dwPid = 0, dwVid = 0;
					WCHAR* strVid = wcsstr(var.bstrVal, L"VID_");
					if (strVid && swscanf(strVid, L"VID_%4X", &dwVid) != 1)
						dwVid = 0;
					WCHAR* strPid = wcsstr(var.bstrVal, L"PID_");
					if (strPid && swscanf(strPid, L"PID_%4X", &dwPid) != 1)
						dwPid = 0;

					// Compare the VID/PID to the DInput device
					DWORD dwVidPid = MAKELONG(dwVid, dwPid);
					if (dwVidPid == pGuidProductFromDirectInput->Data1)
					{
						bIsXinputDevice = true;
						goto LCleanup;
					}
				}
			}
			if (pDevices[iDevice]) { pDevices[iDevice]->Release(); pDevices[iDevice] = NULL; }
		}
	}

LCleanup:
	if (bstrNamespace)
		SysFreeString(bstrNamespace);
	if (bstrDeviceID)
		SysFreeString(bstrDeviceID);
	if (bstrClassName)
		SysFreeString(bstrClassName);
	for (iDevice = 0; iDevice < 20; iDevice++)
		if (pDevices[iDevice]) { pDevices[iDevice]->Release(); pDevices[iDevice] = NULL; }
	if (pEnumDevices) { pEnumDevices->Release(); pEnumDevices = NULL; }
	if (pIWbemLocator) { pIWbemLocator->Release(); pIWbemLocator = NULL; }
	if (pIWbemServices) { pIWbemServices->Release(); pIWbemServices = NULL; }

	if (bCleanupCOM)
		CoUninitialize();

	return bIsXinputDevice;
}

DInput::DInput(wxEvtHandler *handler, HWND hwnd) :
	_di(nullptr), _hwnd(hwnd), _evtHandler(handler)
{
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	HINSTANCE hinst = GetModuleHandle(nullptr);
	HRESULT result;
	result = DirectInput8Create(hinst, DIRECTINPUT_VERSION, IID_IDirectInput8, (LPVOID *)&_di, nullptr);
	if (result != DI_OK) {
		Log(Error, "Could not create direct input 8 interface: 0x%08x", result);
		return;
	}

	EnumDevices();

	_devices.clear();
	for (auto &it = _devicesInfo.begin(); it != _devicesInfo.end(); ++it) {
		_devices.emplace_back(this, _di, *it, _hwnd);
	}

}

void DInput::Tick(unsigned int time)
{
	for (auto &it = _devices.begin(); it != _devices.end(); ++it) {
		it->Tick();
	}
}

DInput::~DInput()
{
	CoUninitialize();

	if (_di != nullptr) {
		_di->Release();
		_di = nullptr;
	}
}

BOOL FAR PASCAL DInputEnumDevicesCallback(LPCDIDEVICEINSTANCE lpddi, LPVOID pvRef) {
	std::vector<DIDEVICEINSTANCE> *list = (std::vector<DIDEVICEINSTANCE> *)pvRef;

	if (IsXInputDevice(&lpddi->guidProduct)) {
		Log(Debug, "XInput device found: %S", lpddi->tszProductName);
	}
	else {
		Log(Debug, "Direct input device found: %S", lpddi->tszProductName);
		list->push_back(*lpddi);
	}

	return DIENUM_CONTINUE;
}

void DInput::EnumDevices()
{
	HRESULT result;
	if (_di == nullptr) return;

	_devicesInfo.clear();
	result = _di->EnumDevices(DI8DEVCLASS_GAMECTRL, DInputEnumDevicesCallback, &_devicesInfo, DIEDFL_ATTACHEDONLY);
	if (result != DI_OK) {
		Log(Error, "Could not enumerate direct input 8 devices: 0x%08x", result);
		return;
	}

}

void DInput::DispatchEvent(wxEventType type, int deviceId, int id, float value)
{
	int idValue = MAKE_JOYSTICK_ID((int)InputSource::DInput, deviceId, (int)id);
	JoystickEvent event(type, idValue, value);
	_evtHandler->ProcessEvent(event);
}

int DInput::GetGUIDId(const GUID &guid)
{
	wxString guidString = wxString::Format("{%08lX-%04hX-%04hX-%02hhX%02hhX-%02hhX%02hhX%02hhX%02hhX%02hhX%02hhX}",
		guid.Data1, guid.Data2, guid.Data3, guid.Data4[0], guid.Data4[1], guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5], guid.Data4[6], guid.Data4[7]);
	Options &options = Options::GetSingleton();
	auto &map = options.dinputMap;
	std::string guidStringStd(guidString.ToStdString());
	auto &mapping = map.find(guidStringStd);
	if (mapping == map.end()) {
		// find first unused key
		int maxId = 0;
		for (auto &it = map.begin(); it != map.end(); ++it) {
			if (it->second >= maxId) {
				maxId = it->second + 1;
			}
		}
		if (maxId > 255) {
			Log(Warn, "DInput mapping overflow. Consider deleting the configuration file or emptying the dinput section.");
			maxId = 0;
		}
		map[guidStringStd] = maxId;
		return maxId;
	}
	else {
		return mapping->second;
	}
}

DInputDevice::DInputDevice(DInput *dinput, LPDIRECTINPUT8 di, DIDEVICEINSTANCE &device, HWND hwnd) :
	_did(nullptr), _instanceGuid(device.guidInstance), _productGuid(device.guidProduct), 
	_name(device.tszProductName), _instanceName(device.tszInstanceName), _dataPacket(nullptr),
	_dataPacketSize(0), _acquired(false), _hwnd(hwnd), _numAxes(0), _numButtons(0), _numPOVs(0),
	_dinput(dinput), _deviceId(dinput->GetGUIDId(device.guidInstance))
{
	HRESULT result;
	result = di->CreateDevice(device.guidInstance, &_did, nullptr);
	if (result != DI_OK) {
		Log(Error, "Could not create direct input 8 device interface: 0x%08x", result);
		return;
	}
	SetCooperativeLevel(false, true);
	GetCapabilities();
	EnumObjects();
	SetDataFormat();
	SetBufferSize(sizeof(DIJOYSTATE));

	Acquire();
	GetState(_lastState);
}

DInputDevice::~DInputDevice()
{
	if (_did) {
		Unacquire();
		_did->Release();
		_did = nullptr;
	}
	if (_dataPacket) {
		delete[] _dataPacket;
		_dataPacket = nullptr;
	}
}

void DInputDevice::Tick()
{
	DeviceState newState;

	GetState(newState);

	for (int i = 0; i < _numButtons; i++) {
		if (newState.buttons[i] != _lastState.buttons[i]) {
			if (newState.buttons[i]) {
				_dinput->DispatchEvent(JOYSTICK_BUTTON_DOWN_EVENT, _deviceId, i, 1.0f);
			}
			else {
				_dinput->DispatchEvent(JOYSTICK_BUTTON_UP_EVENT, _deviceId, i, 0.0f);
			}
		}
	}
	for (int i = 0; i < _numAxes;i++) {
		if (newState.axes[i] != _lastState.axes[i]) {
			_dinput->DispatchEvent(JOYSTICK_AXES_EVENT, _deviceId, _numButtons + i, newState.axes[i]);
		}
	}
	for (int i = 0; i < _numPOVs;i++) {
		if (newState.povs[i] != _lastState.povs[i]) {
			_dinput->DispatchEvent(JOYSTICK_HAT_EVENT, _deviceId, _numButtons + _numPOVs + i, newState.povs[i]);
		}
	}
	_lastState.buttons.swap(newState.buttons);
	_lastState.axes.swap(newState.axes);
	_lastState.povs.swap(newState.povs);
}

void DInputDevice::Acquire()
{
	HRESULT result = _did->Acquire();
	if (result != DI_OK && result != S_FALSE) {
		Log(Error, "Could not acquire input device: 0x%08x", result);
	}
	_acquired = true;
}

void DInputDevice::Unacquire()
{
	_did->Unacquire();
	_acquired = false;
}

void DInputDevice::SetCooperativeLevel(bool foreground, bool exclusive)
{
	HRESULT result;
	bool wasAcquired = _acquired;
	if (_acquired) {
		Unacquire();
	}

	DWORD flags = 0;
	flags |= foreground ? DISCL_FOREGROUND : DISCL_BACKGROUND;
	flags |= exclusive ? DISCL_EXCLUSIVE : DISCL_NONEXCLUSIVE;

	result = _did->SetCooperativeLevel(_hwnd, flags);
	if (result != DI_OK) {
		Log(Error, "Could not set direct input cooperative level for device \"%S\": 0x%08x", _instanceName.c_str(), result);
	}

	if (wasAcquired) {
		Acquire();
	}
}

void DInputDevice::SetAxisRange(int id, int min, int max)
{
	DIPROPRANGE config;
	ZeroMemory(&config, sizeof(DIPROPRANGE));

	config.diph.dwSize = sizeof(DIPROPRANGE);
	config.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	config.diph.dwObj = id;
	config.diph.dwHow = DIPH_BYID;
	config.lMin = min;
	config.lMax = max;
	HRESULT result = _did->SetProperty(DIPROP_RANGE, &config.diph);
	if (result != DI_OK) {
		Log(Error, "Could not set axis range for axis %d: 0x%08x", id, result);
	}
}

void DInputDevice::GetAxisRange(int id, int &min, int &max)
{
	DIPROPRANGE config;
	ZeroMemory(&config, sizeof(DIPROPRANGE));

	config.diph.dwSize = sizeof(DIPROPRANGE);
	config.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	config.diph.dwObj = id;
	config.diph.dwHow = DIPH_BYID;
	HRESULT result = _did->GetProperty(DIPROP_RANGE, &config.diph);
	if (result != DI_OK) {
		Log(Error, "Could not get axis range for axis %d: 0x%08x", id, result);
		max = USHRT_MAX;
		min = 0;
	}
	else {
		max = config.lMax;
		min = config.lMin;
	}
}

int DInputDevice::GetDeviceId() const
{
	return _deviceId;
}

const DInputDevice::DeviceObjectInfo &DInputDevice::GetObjectInfoList()
{
	return _infoList;
}

void DInputDevice::GetState(struct DeviceState &state)
{
	if (_dataPacketSize == 0) {
		Log(Warn, "Device \"%S\" Data packet size is 0", _instanceName.c_str());
		return;
	}
	bool wasAcquired = _acquired;
	if (!_acquired) {
		Acquire();
	}
	_did->Poll();

	ZeroMemory(_dataPacket, _dataPacketSize);
	HRESULT result = _did->GetDeviceState(_dataPacketSize, (LPVOID)_dataPacket);
	if (result != DI_OK) {
		if (result == DIERR_INPUTLOST) {
			_attached = false;
			Log(Debug, "Device \"%S\" disconnected", _instanceName.c_str());
		}
		else {
			Log(Error, "Error reading device \"%S\" input", _instanceName.c_str());
		}
	}
	else {
		int AxesOffset = ((_numButtons + 3) / 4) * 4;
		int POVsOffset = AxesOffset + _numAxes * 4;
		state.buttons.clear();
		state.axes.clear();
		state.povs.clear();
		for (int i = 0; i < _numButtons; i++) {
			state.buttons.push_back(_dataPacket[i] != 0);
		}
		for (int i = 0; i < _numAxes; i++) {
			state.axes.push_back(((int *)(_dataPacket + AxesOffset))[i]);
		}
		for (int i = 0; i < _numPOVs; i++) {
			state.axes.push_back(((int *)(_dataPacket + POVsOffset))[i]);
		}
	}
	
	if (!wasAcquired) {
		Unacquire();
	}
}

void DInputDevice::Poll()
{
	if (!_acquired) {
		Acquire();
	}
	_did->Poll();
}

void DInputDevice::GetCapabilities()
{
	DIDEVCAPS caps;
	HRESULT result;
	ZeroMemory(&caps, sizeof(DIDEVCAPS));
	caps.dwSize = sizeof(DIDEVCAPS);
	result = _did->GetCapabilities(&caps);
	if (result != DI_OK) {
		Log(Error, "Could not get device \"%S\" capabilities: 0x%08x", _instanceName.c_str(), result);
		return;
	}

	_attached = (caps.dwFlags & DIDC_ATTACHED) != 0;
	_numAxes = caps.dwAxes;
	_numButtons = caps.dwButtons;
	_numPOVs = caps.dwPOVs;
}

void DInputDevice::SetDataFormat()
{
	HRESULT result;
	bool wasAcquired = _acquired;
	if (_acquired) {
		Unacquire();
	}

	int numObjects = _numButtons + _numAxes + _numPOVs;
	int AxesOffset = ((_numButtons + 3) / 4) * 4;
	int POVsOffset = AxesOffset + _numAxes * 4;
	int dataPacketSize = POVsOffset + _numPOVs * 4;
	DIOBJECTDATAFORMAT *objectDataFormats = new DIOBJECTDATAFORMAT[numObjects];
	ZeroMemory(objectDataFormats, sizeof(DIOBJECTDATAFORMAT) * numObjects);
	// Setup buttons
	for (int i = 0; i < _numButtons; i++) {
		objectDataFormats[i].pguid = 0;
		objectDataFormats[i].dwOfs = i;
		objectDataFormats[i].dwType = _infoList.buttons[i].type;
		objectDataFormats[i].dwFlags = 0;
	}
	// Setup axes
	for (int i = 0; i < _numAxes; i++) {
		int idx = _numButtons + i;
		objectDataFormats[idx].pguid = 0;
		objectDataFormats[idx].dwOfs = AxesOffset + i * 4;
		objectDataFormats[idx].dwType = _infoList.axes[i].type;
		objectDataFormats[idx].dwFlags = 0;
	}
	// Setup POVs
	for (int i = 0; i < _numPOVs; i++) {
		int idx = _numButtons + _numAxes + i;
		objectDataFormats[idx].pguid = 0;
		objectDataFormats[idx].dwOfs = POVsOffset + i * 4;
		objectDataFormats[idx].dwType = _infoList.povs[i].type;
		objectDataFormats[idx].dwFlags = 0;
	}
	DIDATAFORMAT dataFormat;
	ZeroMemory(&dataFormat, sizeof(DIDATAFORMAT));
	dataFormat.dwSize = sizeof(DIDATAFORMAT);
	dataFormat.dwObjSize = sizeof(DIOBJECTDATAFORMAT);
	dataFormat.dwFlags = 0;
	dataFormat.dwDataSize = dataPacketSize;
	dataFormat.dwNumObjs = numObjects;
	dataFormat.rgodf = objectDataFormats;

	_dataPacketSize = dataPacketSize;
	if (_dataPacket) {
		delete[] _dataPacket;
		_dataPacket = nullptr;
	}
	_dataPacket = new unsigned char[_dataPacketSize];
	ZeroMemory(_dataPacket, _dataPacketSize);

	result = _did->SetDataFormat(&dataFormat);
	if (result != DI_OK) {
		Log(Error, "Could not set device \"%S\" data format: 0x%08x", _instanceName.c_str(), result);
		_dataPacketSize = 0;
	}
	
	delete[] objectDataFormats;
	
	if (wasAcquired) {
		Acquire();
	}
}

void DInputDevice::SetBufferSize(int size)
{
	DIPROPDWORD config;
	ZeroMemory(&config, sizeof(DIPROPDWORD));

	config.diph.dwSize = sizeof(DIPROPDWORD);
	config.diph.dwHeaderSize = sizeof(DIPROPHEADER);
	config.diph.dwObj = 0;
	config.diph.dwHow = DIPH_DEVICE;
	config.dwData = size;
	HRESULT result = _did->SetProperty(DIPROP_BUFFERSIZE, &config.diph);
	if (result != DI_OK) {
		Log(Error, "Could not set buffer size");
	}
}

BOOL FAR PASCAL EnumObjectsCallback(LPCDIDEVICEOBJECTINSTANCE lpddoi, LPVOID pvRef) {
	DInputDevice *device = (DInputDevice *)pvRef;
	DInputDevice::DeviceObjectInfo &infoList = const_cast<DInputDevice::DeviceObjectInfo &>(device->GetObjectInfoList());

	Log(Debug, "Device object name: %S", lpddoi->tszName);

	DInputDevice::ObjectInfo info;
	int type = DIDFT_GETTYPE(lpddoi->dwType);
	info.name = lpddoi->tszName;
	info.id = DIDFT_GETINSTANCE(lpddoi->dwType);
	info.type = lpddoi->dwType;
	if ((type & DIDFT_AXIS) != 0) {
		device->GetAxisRange(type, info.min, info.max);
		infoList.axes.push_back(info);
	}
	else if ((type & DIDFT_BUTTON) != 0) {
		info.min = 0;
		info.max = 1;
		infoList.buttons.push_back(info);
	}
	else if (type == DIDFT_POV) {
		info.min = 0;
		info.max = 0;
		infoList.povs.push_back(info);
	}

	return DIENUM_CONTINUE;
}

void DInputDevice::EnumObjects()
{
	_infoList.axes.clear();
	_infoList.buttons.clear();
	_infoList.povs.clear();
	HRESULT result = _did->EnumObjects(EnumObjectsCallback, this, DIDFT_AXIS | DIDFT_BUTTON | DIDFT_POV);
	if (result != DI_OK) {
		Log(Error, "Could not enumerate device \"%s\" objects: 0x%08x", _instanceName.c_str(), result);
	}
}
