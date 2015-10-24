#include "util/memDbg.h"
#include "util/log.h"
#include <AL/al.h>
#include <AL/alc.h>

#include "audio.h"


Audio::Audio()
{

}

Audio::~Audio()
{
	Close();
}

int Audio::Init(const std::string &deviceName)
{
	// Open the audio device
	// Revert to the default device if the given device could not be loaded.
	ALCdevice *device = alcOpenDevice(deviceName.empty() ? NULL : deviceName.c_str()); 
	if (!device)
	{
		// Could not open the audio device
		Log(Warn, "Could not open \"%s\"", deviceName.empty() ? "Default audio device" : deviceName.c_str());
		if (!deviceName.empty())
		{
			Log(Warn, "Trying default device");
			device = alcOpenDevice(NULL);
			if (!device)
			{
				Log(Error, "Could also not open default audio device");
				return -1;
			}
		}
		else {
			return -1;
		}
	}
	// Create an audio context
	ALCcontext *context = alcCreateContext(device, NULL);
	if (context == NULL || alcMakeContextCurrent(context) == ALC_FALSE)
	{
		// Could not create a context and make it current
		if (context != NULL)
			alcDestroyContext(context); 
		alcCloseDevice(device);
		Log(Error, "Could not set the context");
		return -2;
	}
	// Log
	Log(Message, "Opened audio device \"%s\"", alcGetString(device, ALC_DEVICE_SPECIFIER));
	Log(Message, "OpenAL %s loaded", alGetString(AL_VERSION));
	return 0;	
}

void Audio::Close()
{
	ALCcontext *context = alcGetCurrentContext(); // Get the current context
	if (!context) // When no context was loaded return
		return;

	ALCdevice *device = alcGetContextsDevice(context); // Get the attached device

	// Clean up openal
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

std::list<std::string> Audio::GetDevices()
{
	// Get list of devices
	std::list<std::string> devices;
	const char *devicelist = alcGetString(NULL, ALC_DEVICE_SPECIFIER);
	// The list is Null character split with double NULL as a terminator
	do {
		devices.push_back(std::string(devicelist));
		devicelist += strlen(devicelist) + 1; 
	} while (*devicelist != 0);
	return devices;
}