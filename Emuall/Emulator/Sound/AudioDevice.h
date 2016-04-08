#ifndef AUDIO_DEVICE_H_
#define AUDIO_DEVICE_H_

#include <string>
#include <vector>

#include <AL/alc.h>

class AudioDevice {
	friend class AudioContext;
public:
	AudioDevice(std::string device);
	AudioDevice(AudioDevice &other);
	AudioDevice();
	~AudioDevice();

	AudioDevice &operator=(AudioDevice &other);

	static std::vector<std::string> GetDevices();
	static std::string GetDefaultDevice();
private:

	ALCdevice *_device;
};

#endif
