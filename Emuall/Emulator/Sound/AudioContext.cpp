#include <al/al.h>

#include "AudioContext.h"
#include "AudioException.h"

AudioContext::Attributes::Attributes()
{

}

AudioContext::Attributes::~Attributes()
{

}

void AudioContext::Attributes::Reset()
{
	_attributes.clear();
}

AudioContext::Attributes & AudioContext::Attributes::Frequency(int frequency)
{
	_attributes.push_back(ALC_FREQUENCY);
	_attributes.push_back(frequency);
	return *this;
}

AudioContext::Attributes & AudioContext::Attributes::Refresh(int frequency)
{
	_attributes.push_back(ALC_REFRESH);
	_attributes.push_back(frequency);
	return *this;
}

AudioContext::Attributes & AudioContext::Attributes::Sync(bool sync)
{
	_attributes.push_back(ALC_SYNC);
	_attributes.push_back(sync ? 1 : 0);
	return *this;
}

AudioContext::Attributes & AudioContext::Attributes::MonoSources(int numMonoSources)
{
	_attributes.push_back(ALC_MONO_SOURCES);
	_attributes.push_back(numMonoSources);
	return *this;
}

AudioContext::Attributes & AudioContext::Attributes::StereoSources(int numStereoSources)
{
	_attributes.push_back(ALC_STEREO_SOURCES);
	_attributes.push_back(numStereoSources);
	return *this;
}

void AudioContext::Attributes::FinishAttributes()
{
	_attributes.push_back(0);
}

const int * AudioContext::Attributes::GetAttrList() const
{
	return _attributes.size() > 0 ? _attributes.data() : nullptr;
}

AudioContext::AudioContext(AudioDevice &device, Attributes &attributes) :
	_context(nullptr), _device(device)
{
	_context = alcCreateContext(_device._device, attributes.GetAttrList());
	if (_context == nullptr) {
		throw AudioException(alcGetError(_device._device), "Could not create openAL context", false);
	}

	MakeCurrent();
}

AudioContext::AudioContext(AudioContext &other) :
	_context(other._context), _device(other._device)
{
	other._context = nullptr;
}

AudioContext::AudioContext() :
	_context(nullptr)
{

}

AudioContext::~AudioContext()
{
	if (_context != nullptr) {
		ReleaseContext();
		alcDestroyContext(_context);
		_context = nullptr;
	}
}

AudioContext & AudioContext::operator=(AudioContext &other)
{
	if (_context != nullptr) {
		ReleaseContext();
		alcDestroyContext(_context);
		_context = nullptr;
	}
	_context = other._context;
	other._context = nullptr;
	_device = other._device;
	return *this;
}

void AudioContext::MakeCurrent()
{
	if (alcMakeContextCurrent(_context) == ALC_FALSE) {
		throw AudioException(alcGetError(_device._device), "Could not make openAL context current", false);
	}
}

void AudioContext::ReleaseContext()
{
	if (alcMakeContextCurrent(nullptr) == ALC_FALSE) {
		throw AudioException(ALC_NO_ERROR, "Could not release openAL context", false);
	}
}

void AudioContext::Process()
{
	alcProcessContext(_context);
	ALCenum error = alcGetError(_device._device);
	if (error != ALC_NO_ERROR) {
		throw AudioException(error, "Could not process context", false);
	}
}

void AudioContext::Suspend()
{
	alcSuspendContext(_context);
	ALCenum error = alcGetError(_device._device);
	if (error != ALC_NO_ERROR) {
		throw AudioException(error, "Could not process context", false);
	}
}

void AudioContext::SetListenerGain(float gain)
{
	alListenerf(AL_GAIN, gain);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set listener gain");
	}
}

float AudioContext::GetListenerGain()
{
	float gain;
	alGetListenerf(AL_GAIN, &gain);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get listener gain");
	}
	return gain;
}

void AudioContext::SetListenerPosition(float x, float y, float z)
{
	alListener3f(AL_POSITION, x, y, z);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set listener position");
	}
}

void AudioContext::GetListenerPosition(float &x, float &y, float &z)
{
	alGetListener3f(AL_POSITION, &x, &y, &z);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get listener position");
	}
}

void AudioContext::SetListenerVelocity(float x, float y, float z)
{
	alListener3f(AL_VELOCITY, x, y, z);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set listener velocity");
	}
}

void AudioContext::GetListenerVelocity(float &x, float &y, float &z)
{
	alGetListener3f(AL_VELOCITY, &x, &y, &z);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not get listener velocity");
	}
}

void AudioContext::SetListenerOrientation(float x_at, float y_at, float z_at, float x_up, float y_up, float z_up)
{
	float orientation[6] = { x_at, y_at, z_at, x_up, y_up, z_up };
	alListenerfv(AL_ORIENTATION, orientation);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set listener orientation");
	}
}

void AudioContext::GetListenerOrientation(float &x_at, float &y_at, float &z_at, float &x_up, float &y_up, float &z_up)
{
	float orientation[6];
	alGetListenerfv(AL_ORIENTATION, orientation);
	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		throw AudioException(error, "Could not set listener orientation");
	}
	x_at = orientation[0];
	y_at = orientation[1];
	z_at = orientation[2];
	x_up = orientation[3];
	y_up = orientation[4];
	z_up = orientation[5];
}

