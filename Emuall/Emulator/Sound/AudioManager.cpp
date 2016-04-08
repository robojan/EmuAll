#include <math.h>

#include "../util/memDbg.h"
#include "../util/log.h"
#include "../util/Options.h"

#include "AudioManager.h"
#include "AudioDevice.h"
#include "AudioException.h"

void streamCallback(AudioBuffer::Format format, int freq, int elements, void *data, void *user);

AudioManager::AudioManager()
{
	try {
		std::string defaultDevice = AudioDevice::GetDefaultDevice();
		AudioDevice device(defaultDevice);
		AudioContext::Attributes attr;
		attr.Frequency(Options::GetInstance().audioOptions.sampleRate);
		attr.FinishAttributes();
		_context = AudioContext(device, attr);
		_context.MakeCurrent();
		_context.SetListenerPosition(0, 0, 0);
		_context.SetListenerVelocity(0, 0, 0);
	}
	catch (AudioException &e) {
		Log(Error, "%s\n%s", e.GetMsg(), e.GetStacktrace());
	}
}

AudioManager::~AudioManager()
{
	
}

void AudioManager::Tick()
{
	for (auto &stream : _streams) {
		stream.Tick();
	}
}

void AudioManager::Play()
{
	for (auto &stream : _streams) {
		stream.Play();
	}
}

void AudioManager::Pause()
{
	for (auto &stream : _streams) {
		stream.Pause();
	}
}

void AudioManager::AddAudioStream(AudioStream &stream)
{
	_streams.emplace_back(stream);
}

void AudioManager::ClearAudioStreams()
{
	_streams.clear();
}

void streamCallback(AudioBuffer::Format format, int freq, int elements, void *data, void *user)
{
	static int pos = 0;
	short *ptr = (short *)data;
	for (int i = 0; i < elements; i++) {
		ptr[i] = 10000 * sinf(pos++ * 1000.0f * 2.0f * 3.141592f / freq);
	}
}
