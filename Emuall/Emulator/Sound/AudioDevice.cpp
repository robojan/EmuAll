
#include <AL/al.h>
#include <AL/alext.h>

#include "AudioDevice.h"
#include "AudioException.h"

AudioDevice::AudioDevice(std::string device) :
	_device(nullptr)
{
	// Get the device name
	const ALCchar *deviceId;
	if (device.empty()) {
		deviceId = nullptr;
	}
	else {
		deviceId = device.c_str();
	}

	// Open the audio device
	_device = alcOpenDevice(deviceId);
	if (_device == nullptr) {
		throw AudioException(alcGetError(NULL), "Could not open audio device", false);
	}
}

AudioDevice::AudioDevice(AudioDevice &other) :
	_device(other._device)
{
	other._device = nullptr;
}

AudioDevice::AudioDevice() :
	_device(nullptr)
{

}

AudioDevice::~AudioDevice()
{
	if (_device != nullptr) {
		alcCloseDevice(_device);
		_device = nullptr;
	}
}

AudioDevice & AudioDevice::operator=(AudioDevice &other)
{
	if (_device != nullptr) {
		alcCloseDevice(_device);
		_device = nullptr;
	}
	_device = other._device;
	other._device = nullptr;
	return *this;
}

std::vector<std::string> AudioDevice::GetDevices()
{
	std::vector<std::string> devices;
	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT") != AL_TRUE) {
		return devices;
	}

	const ALCchar *devicesList = alcGetString(nullptr, ALC_ALL_DEVICES_SPECIFIER);
	if (devicesList == nullptr) {
		return devices;
	}
	do {
		size_t len = strlen(devicesList);
		if (len > 0) {
			devices.emplace_back(devicesList);
		}
		devicesList += len + 1;
	} while (*devicesList != '\0');
	return devices;
}

std::string AudioDevice::GetDefaultDevice()
{
	if (alcIsExtensionPresent(nullptr, "ALC_ENUMERATE_ALL_EXT") != AL_TRUE) {
		return std::string();
	}
	const ALCchar *device = alcGetString(nullptr, ALC_DEFAULT_ALL_DEVICES_SPECIFIER);
	if (device == nullptr) {
		return std::string();
	}
	else {
		return device;
	}
}
